#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Core/Resources.h"
#include <imgui.h>
#include <glm/gtx/euler_angles.hpp>

#include "Stulu/Core/Time.h"

namespace Stulu {
	SceneRenderer::RuntimeData SceneRenderer::s_runtimeData;

	SceneRenderer::LightData SceneRenderer::s_lightData;
	SceneRenderer::PostProcessingData SceneRenderer::s_postProcessingData;
	SceneRenderer::ShaderSceneData SceneRenderer::s_shaderSceneData;

	void SceneRenderer::init() {
		ST_PROFILING_FUNCTION();
		if (s_runtimeData.lightBuffer == nullptr)
			s_runtimeData.lightBuffer = UniformBuffer::create(sizeof(LightData), 1);
		
		if (s_runtimeData.postProcessungBuffer == nullptr)
			s_runtimeData.postProcessungBuffer = UniformBuffer::create(sizeof(PostProcessingData), 2);
		
		if (s_runtimeData.sceneDataBuffer == nullptr)
			s_runtimeData.sceneDataBuffer = UniformBuffer::create(sizeof(ShaderSceneData), 3);

		if (Material::s_materialBuffer == nullptr)
			Material::s_materialBuffer = UniformBuffer::create(sizeof(uint32_t) * 4000, 4);//max = 16KB = 4B * 4000

		if (!s_runtimeData.fbDrawData.m_quadVertexArray) {
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
		}
		if (!s_runtimeData.fbDrawData.m_quadShader) {
			s_runtimeData.fbDrawData.m_quadShader = Shader::create("quadFullScreen", R"(
		#version 460
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec2 a_texCoords;
		out vec2 v_tex;
		layout(std140, binding = 0) uniform matrices
		{
			float z;
		};
		void main()
		{
			v_tex=a_texCoords;
			vec3 pos = a_pos;
			pos.z = z;
			gl_Position=vec4(pos, 1.0);
		}
		)", R"(
		#version 460
		in vec2 v_tex;
		layout (binding = 0) uniform sampler2D texSampler;
		out vec4 a_color;
		void main()
		{
			vec4 color = texture2D(texSampler, v_tex);
			if(color.a == 0.0f)
				discard;
			a_color = color;
		}
		)");
		}
	}

	void SceneRenderer::Begin() {
		ST_PROFILING_FUNCTION();
		ST_PROFILING_RENDERDATA_RESET();
		ST_PROFILING_RENDERDATA_BEGIN();
	}

	void SceneRenderer::End() {
		ST_PROFILING_FUNCTION();
		ST_PROFILING_RENDERDATA_END();
		s_runtimeData.drawList.clear();
		s_runtimeData.transparentDrawList.clear();
		s_runtimeData.stencilDrawList.clear();
	}

	void SceneRenderer::LoadLights(Scene* scene) {
		for (auto& goID : scene->getAllGameObjectsWith<LightComponent, TransformComponent>()) {
			GameObject go(goID, scene);
			AddLight(go.getComponent<TransformComponent>(), go.getComponent<LightComponent>());
		}
		UploadLightBuffer();
		s_lightData = LightData();
	}

	void SceneRenderer::AddLight(const TransformComponent& transform, const LightComponent& light) {
		ST_PROFILING_FUNCTION();
		if (s_lightData.lightCount < ST_MAXLIGHTS) {
			s_lightData.lights[s_lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
			s_lightData.lights[s_lightData.lightCount].positionAndType = glm::vec4(transform.worldPosition, light.lightType);
			s_lightData.lights[s_lightData.lightCount].rotation = glm::vec4(transform.forward, 1.0f);
			s_lightData.lights[s_lightData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
			s_lightData.lightCount++;
		}
	}
	void SceneRenderer::UploadLightBuffer() {
		ST_PROFILING_FUNCTION();
		s_runtimeData.lightBuffer->setData(&s_lightData, sizeof(LightData));
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
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam) {
		ST_PROFILING_FUNCTION();
		CameraComponent camComp = mainCam.gameObject.getComponent<CameraComponent>();
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		DrawSceneToCamera(sceneCam.m_transform, camComp, false);

	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, Scene* scene) {
		ST_PROFILING_FUNCTION();
		CameraComponent camComp = CameraComponent(CameraMode::Perspective, false);
		camComp.gameObject = GameObject(entt::null, scene);
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		DrawSceneToCamera(sceneCam.m_transform, camComp, false);
	}
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp, bool bdrawSkyBox) {
		ST_PROFILING_FUNCTION();
		const Ref<Camera>& cam = cameraComp.getNativeCamera();
		const glm::mat4& view = glm::inverse(transform.transform);
		const glm::mat4& proj = cameraComp.getProjection();

		const Ref<FrameBuffer>& reflectionFrameBuffer = cameraComp.reflectionFrameBuffer;
		Ref<CubeMap> reflectionMap = cameraComp.reflectionMap;
		bool useReflectionMap = cameraComp.gameObject.getScene()->m_data.useReflectionMapReflections;

		Ref<SkyBox> camSkyBoxTexture = nullptr;
		uint32_t camSkyBoxMapType = 0;
		if ((entt::entity)cameraComp.gameObject != entt::null && cameraComp.settings.clearType == CameraComponent::Skybox && cameraComp.gameObject.hasComponent<SkyBoxComponent>()) {
			auto& s = cameraComp.gameObject.getComponent<SkyBoxComponent>();
			camSkyBoxTexture = s.texture;
			camSkyBoxMapType = (uint32_t)s.mapType;
		}
		//upload shader scene data
		s_shaderSceneData.gamma = cameraComp.gameObject.getScene()->m_data.gamma;
		s_shaderSceneData.toneMappingExposure = cameraComp.gameObject.getScene()->m_data.toneMappingExposure;
		s_shaderSceneData.env_lod = cameraComp.gameObject.getScene()->m_data.env_lod;
		s_shaderSceneData.enableGammaCorrection = 1;
		s_shaderSceneData.skyboxMapType = camSkyBoxMapType;
		s_shaderSceneData.useSkybox = camSkyBoxTexture != nullptr;
		s_shaderSceneData.shaderFlags = cameraComp.gameObject.getScene()->m_data.shaderFlags;
		s_runtimeData.sceneDataBuffer->setData(&s_shaderSceneData, sizeof(s_shaderSceneData));

		//transparent sorting
		std::sort(s_runtimeData.transparentDrawList.begin(), s_runtimeData.transparentDrawList.end(), [&](auto& a, auto& b) {
			return glm::distance(transform.worldPosition, glm::vec3(a.transform[3])) > glm::distance(transform.worldPosition, glm::vec3(b.transform[3]));//sort the vector backwards
			});


		//reflection map pass
		//https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
		if (useReflectionMap && reflectionFrameBuffer && reflectionMap) {
			s_shaderSceneData.enableGammaCorrection = 0;
			s_runtimeData.sceneDataBuffer->setData(&s_shaderSceneData, sizeof(s_shaderSceneData));
			//						  fb,    index
			static std::unordered_map<void*, uint32_t> mapIndexMap;
			uint32_t& reflectionRenderIndex = mapIndexMap[cam.get()];
			reflectionFrameBuffer->bind();
			reflectionFrameBuffer->attachCubeMapFace(reflectionMap, reflectionRenderIndex);
			Renderer::uploadBufferData(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 200.0f), calcView(reflectionRenderIndex, transform), transform.worldPosition, transform.eulerAnglesWorldDegrees);
			RenderCommand::clear();
			drawSkyBox(camSkyBoxTexture);
			drawScene(nullptr);
			reflectionFrameBuffer->unbind();
			if (reflectionRenderIndex >= 5)
				reflectionRenderIndex = 0;
			else
				reflectionRenderIndex++;
			s_shaderSceneData.enableGammaCorrection = 1;
			s_runtimeData.sceneDataBuffer->setData(&s_shaderSceneData, sizeof(s_shaderSceneData));
			
		}else
			reflectionMap = nullptr;
		//default render pass
		cam->bindFrameBuffer();
		Renderer::uploadBufferData(proj, view, transform.worldPosition, transform.eulerAnglesWorldDegrees);
		if (bdrawSkyBox)
			drawSkyBox(camSkyBoxTexture);
		else if (camSkyBoxTexture)
			camSkyBoxTexture->bind();
		drawScene(reflectionMap);
		cam->unbindFrameBuffer();
	}

	void SceneRenderer::Clear() {
		s_shaderSceneData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}

	void SceneRenderer::Clear(CameraComponent& cam) {
		RenderCommand::setDefault();
		switch (cam.settings.clearType)
		{
		case CameraComponent::Color:
			RenderCommand::setClearColor(cam.settings.clearColor);
			s_shaderSceneData.clearColor = cam.settings.clearColor;
			break;
		case CameraComponent::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
			s_shaderSceneData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
			break;
		}
		RenderCommand::clear();
	}

	void SceneRenderer::GenSceneTexture(const Ref<FrameBuffer>& sceneFbo, const Ref<Scene>& scene) {
		ST_PROFILING_FUNCTION();
		auto& cams = scene->getAllGameObjectsAsGroupWith<TransformComponent, CameraComponent>();

		cams.sort([&](const entt::entity a, const entt::entity b) {
			int32_t dA = cams.get<CameraComponent>(a).depth;
			int32_t dB = cams.get<CameraComponent>(b).depth;
			return dA < dB;
			});

		s_postProcessingData.time = Time::time;
		s_postProcessingData.delta = Time::deltaTime;
		s_runtimeData.postProcessungBuffer->setData(&s_postProcessingData, sizeof(s_postProcessingData));

		sceneFbo->bind();
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
		RenderCommand::clear();
		sceneFbo->unbind();
		for (auto& goID : cams) {
			CameraComponent& cam = cams.get<CameraComponent>(goID);
			if (!cam.settings.isRenderTarget) {
				Renderer::uploadBufferData(&cam.depth, sizeof(cam.depth));

				cam.getFrameBuffer()->getTexture()->bind(0);
				s_runtimeData.fbDrawData.m_quadShader->bind();
				
				sceneFbo->bind();
				RenderCommand::drawIndexed(s_runtimeData.fbDrawData.m_quadVertexArray);
				sceneFbo->unbind();
			}
		}
	}

	void SceneRenderer::drawScene(const Ref<CubeMap>& reflectionMap) {
		ST_PROFILING_FUNCTION();
		UUID last = UUID();
		
		if (reflectionMap) {
			reflectionMap->bind(2);
			SkyBox::genrateBRDFLUT(ST_REFLECTION_MAP_SIZE)->bind(3);
		}

		//opaque
		for (const RenderObject& object : s_runtimeData.drawList) {
			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, nullptr, object.transform);
		}

		//transparent
		for (const auto& object : s_runtimeData.transparentDrawList) {
			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.material->getShader(), object.transform);
		}

		//stencil
		RenderCommand::setStencil(StencilMode::WriteToBuffer);
		for (const RenderObject& object : s_runtimeData.stencilDrawList) {
			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, nullptr, object.transform);
		}
		RenderCommand::setStencil(StencilMode::DisableWriting);
	}
	void SceneRenderer::drawSkyBox(const Ref<CubeMap>& skybox) {
		ST_PROFILING_FUNCTION();
		if (skybox) {
			auto& shader = Resources::getSkyBoxShader();
			shader->bind();
			skybox->bind();
			skybox->draw();
		}
		
	}
	void SceneRenderer::RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform) {
		ST_PROFILING_FUNCTION();
		if (!filter.mesh.hasMesh)
			return;
		Ref<Material> material;
		if (mesh.material) {
			material = mesh.material;
		}
		else {
			material = Resources::getDefaultMaterial();
		}
		if (mesh.m_enabledStencilBufferNextFrame) {
			s_runtimeData.stencilDrawList.push_back(RenderObject{ material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode });
		}
		else {
			if (material->isTransparent())
				s_runtimeData.transparentDrawList.push_back(RenderObject{
				material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode });
			else
				s_runtimeData.drawList.push_back(RenderObject{ material,filter.mesh.mesh->getVertexArray(),transform.transform, mesh.cullmode });
		}
		ST_PROFILING_RENDERDATA_ADDINDICES(filter.mesh.mesh->getIndicesCount());
		ST_PROFILING_RENDERDATA_ADDVERTICES(filter.mesh.mesh->getVerticesCount());
	}
	void SceneRenderer::RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& mat, const glm::mat4& transform, bool stencil) {
		ST_PROFILING_FUNCTION();
		if (!vertexArray)
			return;
		Ref<Material> material;
		if (mat) {
			material = mat;
		}
		else {
			material = Resources::getDefaultMaterial();
		}

		if (stencil) {
			s_runtimeData.stencilDrawList.push_back(RenderObject{ material,vertexArray,transform,CullMode::Back });
		}
		else {
			if (material->isTransparent())
				s_runtimeData.transparentDrawList.push_back(RenderObject{
				material,vertexArray,transform,CullMode::Back });
			else
				s_runtimeData.drawList.push_back(RenderObject{ material,vertexArray,transform, CullMode::Back });
		}


		s_runtimeData.drawList.push_back(RenderObject{ material,vertexArray,transform });
	}
	void SceneRenderer::RegisterObject(const RenderObject& object) {
		s_runtimeData.drawList.push_back(object);
	}
}