#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Scene/Resources.h"
#include <imgui.h>
#include <glm/gtx/euler_angles.hpp>

namespace Stulu {
	SceneRenderer::RuntimeData SceneRenderer::s_runtimeData;
	void SceneRenderer::init() {
		ST_PROFILING_FUNCTION();
		if (s_runtimeData.lightBuffer == nullptr)
			s_runtimeData.lightBuffer = UniformBuffer::create(sizeof(RuntimeData::LightData), 1);
		if (s_runtimeData.sceneDataBuffer == nullptr)
			s_runtimeData.sceneDataBuffer = UniformBuffer::create(sizeof(RuntimeData::SceneData), 2);
		if (Material::s_materialBuffer == nullptr)
			Material::s_materialBuffer = UniformBuffer::create(sizeof(float)*256, 3);



		{
			Stulu::Ref<Stulu::VertexBuffer> vertexBuffer;
			Stulu::Ref<Stulu::IndexBuffer> indexBuffer;
			s_runtimeData.fbDrawData.m_quadVertexArray = Stulu::VertexArray::create();
			float vertices[20]{
				-1.0f,-1.0f,.0f, 0.0f,0.0f,
				1.0f,-1.0f,.0f, 1.0f,0.0f,
				1.0f,1.0f,.0f, 1.0f,1.0f,
				-1.0f,1.0f,.0f, 0.0f,1.0f,
			};
			vertexBuffer = Stulu::VertexBuffer::create((uint32_t)(20 * sizeof(float)), vertices);
			vertexBuffer->setLayout({
		{ Stulu::ShaderDataType::Float3, "a_pos" },
		{ Stulu::ShaderDataType::Float2, "a_texCoord" },
				});
			s_runtimeData.fbDrawData.m_quadVertexArray->addVertexBuffer(vertexBuffer);
			uint32_t indices[6]{
				0,1,2,
				2,3,0
			};
			indexBuffer = Stulu::IndexBuffer::create((uint32_t)6, indices);
			s_runtimeData.fbDrawData.m_quadVertexArray->setIndexBuffer(indexBuffer);
			s_runtimeData.fbDrawData.m_quadShader = Shader::create("quadFullScreen", R"(
		#version 460
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec2 a_texCoords;
		out vec2 v_tex;
		void main()
		{
			v_tex=a_texCoords;
			gl_Position=vec4(a_pos, 1.0);
		}
		)", R"(
		#version 460
		in vec2 v_tex;
		layout (binding = 0) uniform sampler2D texSampler;
		out vec4 color;
		layout(std140, binding = 2) uniform postProcessing
		{
			float toneMappingExposure;
			float gamma;
			float env_lod;
			uint useSkybox;
		};
		void main()
		{
			color = texture2D(texSampler, v_tex).rgba;
			
			color = vec4(vec3(1.0f) - exp(-color.rgb * toneMappingExposure),color.a);//HDR tonemapping
			color = vec4(pow(color.rgb, vec3(1.0/gamma)),color.a); //gamma correct
		}
		)");
		}
	}

	void SceneRenderer::submit(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform) {
		ST_PROFILING_FUNCTION();
		if (!filter.mesh.hasMesh)
			return;
		Material* material;
		if (mesh.material) {
			material = mesh.material;
		}
		else {
			material = Resources::getDefaultMaterial();
		}
		if (mesh.m_enabledStencilBufferNextFrame) {
			s_stencilDrawList.push_back(RenderObject{ material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode });
		}
		else {
			if(material->isTransparent)
				s_transparentDrawList.push_back(RenderObject{
				material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode,
				glm::distance(s_runtimeData.camTransform.worldPosition,glm::vec3(transform.position)) });
			else
				s_drawList.push_back(RenderObject{ material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode });
		}
	}
	void SceneRenderer::submit(const Ref<VertexArray>& vertexArray, Material* material, const glm::mat4& transform) {
		s_drawList.push_back(RenderObject{ material,vertexArray,transform });
	}
	void SceneRenderer::submit(const RenderObject& object) {
		s_drawList.push_back(object);
	}

	void SceneRenderer::beginScene(GameObject object) {
		ST_PROFILING_FUNCTION();
		auto& cam = object.getComponent<CameraComponent>();
		auto& transform = object.getComponent<TransformComponent>();
		switch (cam.settings.clearType)
		{
		case CameraComponent::Color:
			RenderCommand::setClearColor(cam.settings.clearColor);
			break;
		case CameraComponent::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f));
			break;
		}
		s_runtimeData.cam = cam.getNativeCamera();
		s_runtimeData.camTransform = transform;
		s_runtimeData.proj = cam.getProjection();
		s_runtimeData.view = glm::inverse(transform.transform);

		s_runtimeData.reflectionFrameBuffer = cam.reflectionFrameBuffer;
		s_runtimeData.reflectionMap = cam.reflectionMap;

		if (cam.settings.clearType == CameraComponent::Skybox && object.hasComponent<SkyBoxComponent>()) {
			auto& s = object.getComponent<SkyBoxComponent>();
			s_runtimeData.camSkyBox.texture = s.texture;
			s_runtimeData.camSkyBox.mapType = (uint32_t)s.mapType;
		}
		Renderer2D::beginScene();
	}
	void SceneRenderer::beginScene(const SceneCamera& cam, GameObject mainCam) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		s_runtimeData.cam = cam.getCamera();
		s_runtimeData.camTransform = cam.getTransform();
		s_runtimeData.proj = cam.getCamera()->getProjectionMatrix();
		s_runtimeData.view = glm::inverse(cam.getTransform().transform);

		s_runtimeData.reflectionFrameBuffer = cam.reflectionFrameBuffer;
		s_runtimeData.reflectionMap = cam.reflectionMap;

		if (mainCam) {
			CameraComponent mcam = mainCam.getComponent<CameraComponent>();
			if (mcam.settings.clearType == CameraComponent::Skybox && mainCam.hasComponent<SkyBoxComponent>()) {
				auto& s = mainCam.getComponent<SkyBoxComponent>();
				s_runtimeData.camSkyBox.texture = s.texture;
				s_runtimeData.camSkyBox.mapType = (uint32_t)s.mapType;
			}
		}
		Renderer2D::beginScene();
	}
	glm::mat4 calcView(uint32_t face, TransformComponent& transform) {
		glm::mat4 local = glm::mat4(1.0f);
		switch (face) {
		case 0://+x
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(180.0f, 90.0f, .0f))), glm::vec3(1.0f));
			break;
		case 1://-x
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(180.0f, -90.0f, .0f))), glm::vec3(1.0f));
			break;
		case 2://+y
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), glm::vec3(1.0f));
			break;
		case 3://-y
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(-90.0f, .0f, .0f))), glm::vec3(1.0f));
			break;
		case 4://+z
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(180.0f, .0f, .0f))), glm::vec3(1.0f));
			break;
		case 5://-y
			local = Math::createMat4(transform.worldPosition, glm::quat(glm::radians(glm::vec3(180.0f, 180.0f, .0f))), glm::vec3(1.0f));
			break;
		}
		return glm::inverse(local);
	}
	void SceneRenderer::endScene() {
		ST_PROFILING_FUNCTION();

		//transparent sorting
		std::sort(s_transparentDrawList.begin(), s_transparentDrawList.end(), [](auto& a, auto& b) {
			return a.camDistance > b.camDistance;//sort the vector backwards
			});
		RenderCommand::setDefault();
		//reflection map pass
		//inspired by GTA 5 reflections https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
		if (s_runtimeData.useReflectionMap && s_runtimeData.reflectionFrameBuffer && s_runtimeData.reflectionMap) {
			//						  fb,    index
			static std::unordered_map<void*, uint32_t> mapIndexMap;
			uint32_t& reflectionRenderIndex = mapIndexMap[s_runtimeData.cam.get()];

			s_runtimeData.reflectionFrameBuffer->bind();
			s_runtimeData.reflectionFrameBuffer->attachCubeMapFace(s_runtimeData.reflectionMap, reflectionRenderIndex);
			Renderer::beginScene(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 200.0f), calcView(reflectionRenderIndex, s_runtimeData.camTransform), s_runtimeData.camTransform.worldPosition, s_runtimeData.camTransform.eulerAnglesWorldDegrees);
			drawSceneToReflectionMap();
			Renderer::endScene();
			s_runtimeData.reflectionFrameBuffer->unbind();
			if (reflectionRenderIndex >= 5)
				reflectionRenderIndex = 0;
			else
				reflectionRenderIndex++;
		}
		//default render pass
		s_runtimeData.cam->bindFrameBuffer();
		Renderer::beginScene(s_runtimeData.proj, s_runtimeData.view, s_runtimeData.camTransform.worldPosition, s_runtimeData.camTransform.eulerAnglesWorldDegrees);
		drawScene();
		Renderer2D::endScene();
		Renderer::endScene();
		s_runtimeData.cam->unbindFrameBuffer();
		

		s_drawList.clear();
		s_transparentDrawList.clear();
		s_stencilDrawList.clear();
		//apply post processing by drawing the texture to a new texture
		//draw scene to texture
		{
			RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
			RenderCommand::clear();
			RenderCommand::setCullMode(CullMode::Back);
			s_runtimeData.cam->getFrameBuffer()->getTexture()->bind(0);
			s_runtimeData.fbDrawData.m_quadShader->bind();
			s_runtimeData.fbDrawData.m_quadVertexArray->bind();
			s_runtimeData.cam->bindFrameBuffer();
			RenderCommand::drawIndexed(s_runtimeData.fbDrawData.m_quadVertexArray);
			s_runtimeData.cam->unbindFrameBuffer();
		}


		s_runtimeData.camSkyBox.texture = nullptr;
		s_runtimeData.reflectionFrameBuffer = nullptr;
		s_runtimeData.reflectionMap = nullptr;
	}

	void SceneRenderer::calculateLights(entt::basic_view<entt::entity, entt::exclude_t<>, TransformComponent, LightComponent> view) {
		ST_PROFILING_FUNCTION();
		s_runtimeData.lightData = RuntimeData::LightData();
		for (auto gameObject : view)
		{
			auto [transform, light] = view.get<TransformComponent, LightComponent>(gameObject);
			if (s_runtimeData.lightData.lightCount < MAXLIGHTS) {
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].positionAndType = glm::vec4(transform.worldPosition, light.lightType);
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].rotation = glm::vec4(transform.forward, 1.0f);
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
				s_runtimeData.lightData.lightCount++;
			}
		}
	}
	void SceneRenderer::uploadBuffers(const SceneData& data) {
		ST_PROFILING_FUNCTION();
		s_runtimeData.lightBuffer->setData(&s_runtimeData.lightData, sizeof(RuntimeData::LightData));

		s_runtimeData.bufferData.gamma = data.gamma;
		s_runtimeData.bufferData.toneMappingExposure = data.toneMappingExposure;
		s_runtimeData.bufferData.useSkybox = (s_runtimeData.camSkyBox.texture == nullptr ? 0 : 1);
		s_runtimeData.bufferData.env_lod = data.env_lod;
		s_runtimeData.useReflectionMap = data.useReflectionMapReflections;

		s_runtimeData.sceneDataBuffer->setData(&s_runtimeData.bufferData, sizeof(RuntimeData::SceneData));
	}

	void SceneRenderer::drawScene() {
		ST_PROFILING_FUNCTION();
		RenderCommand::clear();
		drawSkyBox();
		UUID last = UUID();
		
		if (s_runtimeData.useReflectionMap)
			s_runtimeData.reflectionMap->bind(2);

		//opaque
		for (const RenderObject& object : s_drawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}

		//transparent
		for (const auto& object : s_transparentDrawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}

		//stencil
		RenderCommand::setStencil(StencilMode::WriteToBuffer);
		for (const RenderObject& object : s_stencilDrawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}
		RenderCommand::setStencil(StencilMode::DisableWriting);
	}
	void SceneRenderer::drawSceneToReflectionMap() {
		ST_PROFILING_FUNCTION();
		RenderCommand::clear();

		drawSkyBox();

		UUID last = UUID();
		//opaque
		for (const RenderObject& object : s_drawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}

		//transparent
		for (const auto& object : s_transparentDrawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}

		for (const RenderObject& object : s_stencilDrawList) {
			if (last != object.material->getUUID()) {
				object.material->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}
	}

	void SceneRenderer::drawSkyBox() {
		ST_PROFILING_FUNCTION();
		if (s_runtimeData.bufferData.useSkybox == 1) {
			auto& shader = Resources::getSkyBoxShader();
			shader->bind();
			shader->setInt("mapType", (int32_t)s_runtimeData.camSkyBox.mapType);
			s_runtimeData.camSkyBox.texture->bind();
			s_runtimeData.camSkyBox.texture->draw();
		}
		
	}
}