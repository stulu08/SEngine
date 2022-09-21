#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Core/Resources.h"
#include <imgui.h>
#include <glm/gtx/euler_angles.hpp>

#include "Stulu/Core/Time.h"

namespace Stulu {
	SceneRenderer::RuntimeData SceneRenderer::s_data;

	void SceneRenderer::init() {
		ST_PROFILING_FUNCTION();
		if (!s_data.quadPostProcShader) {
			s_data.quadPostProcShader = Shader::create("quadFullScreenPP", R"(
		#version 460
		layout (location = 0) in vec3 a_pos;
		layout (location = 1) in vec2 a_texCoords;
		out vec2 v_tex;
		layout(std140, binding = 1) uniform model
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
		##include "Stulu/Functions"
		##include "Stulu/Bindings"
		layout (binding = 0) uniform sampler2D texSampler;
		out vec4 a_color;
		void main()
		{
			vec4 color = texture2D(texSampler, v_tex);
			if(color.a == 0.0f)
				discard;
			
			a_color = gammaCorrect(color, gamma, toneMappingExposure);
		}
		)");
		}
	}

	void SceneRenderer::Begin() {
		ST_PROFILING_FUNCTION();
	}
	void SceneRenderer::End() {
		ST_PROFILING_FUNCTION();
		s_data.drawList.clear();
		s_data.transparentDrawList.clear();
	}

	void SceneRenderer::LoadLights(Scene* scene) {
		ST_PROFILING_FUNCTION();
		for (auto& goID : scene->getAllGameObjectsWith<LightComponent, TransformComponent>()) {
			GameObject go(goID, scene);
			AddLight(go.getComponent<TransformComponent>(), go.getComponent<LightComponent>());
		}
		UploadLightBuffer();

		s_data.lightBufferData = LightBufferData();
	}
	void SceneRenderer::AddLight(const TransformComponent& transform, const LightComponent& light) {
		if (s_data.lightBufferData.lightCount < ST_MAXLIGHTS) {
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].positionAndType = glm::vec4(transform.worldPosition, light.lightType);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].rotation = glm::vec4(transform.forward, 1.0f);
			s_data.lightBufferData.lights[s_data.lightBufferData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
			s_data.lightBufferData.lightCount++;
		}
	}
	void SceneRenderer::UploadLightBuffer() {
		ST_PROFILING_FUNCTION();
		Renderer::getBuffer(BufferBinding::Light)->setData(&s_data.lightBufferData, sizeof(LightBufferData));
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
		CameraComponent camComp = mainCam.gameObject.getComponent<CameraComponent>();
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, Scene* scene) {
		CameraComponent camComp = CameraComponent(CameraMode::Perspective, false);
		camComp.gameObject = GameObject(entt::null, scene);
		camComp.cam = sceneCam.getCamera();
		camComp.reflectionFrameBuffer = sceneCam.reflectionFrameBuffer;
		camComp.reflectionMap = sceneCam.reflectionMap;
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp) {
		ST_PROFILING_FUNCTION();
		const Ref<Camera>& cam = cameraComp.getNativeCamera();
		const glm::mat4& view = glm::inverse(transform.transform);
		const glm::mat4& proj = cameraComp.getProjection();

		const Ref<FrameBuffer>& reflectionFrameBuffer = cameraComp.reflectionFrameBuffer;
		Ref<CubeMap> reflectionMap = cameraComp.reflectionMap;
		bool useReflectionMap = cameraComp.gameObject.getScene()->m_data.graphicsData.useReflectionMapReflections;

		Ref<SkyBox> camSkyBoxTexture = nullptr;
		uint32_t camSkyBoxMapType = 0;
		glm::mat4 skyboxRotation = glm::mat4(1.0f);
		if ((entt::entity)cameraComp.gameObject != entt::null) {
			if (cameraComp.settings.clearType == CameraComponent::ClearType::Skybox && cameraComp.gameObject.hasComponent<SkyBoxComponent>()) {
				auto& s = cameraComp.gameObject.getComponent<SkyBoxComponent>();
				camSkyBoxTexture = s.texture;
				camSkyBoxMapType = (uint32_t)s.mapType;
				skyboxRotation = Math::createMat4(glm::vec3(.0f), glm::quat(glm::radians(s.rotation)), glm::vec3(1.0f));
			}
			if (cameraComp.gameObject.hasComponent<PostProcessingComponent>()) {
				cameraComp.postProcessing = &cameraComp.gameObject.getComponent<PostProcessingComponent>().data;
			}
		}
		
		//upload shader scene data
		s_data.sceneBufferData.env_lod = cameraComp.gameObject.getScene()->m_data.graphicsData.env_lod;
		s_data.sceneBufferData.skyboxMapType = camSkyBoxMapType;
		s_data.sceneBufferData.useSkybox = camSkyBoxTexture != nullptr;
		s_data.sceneBufferData.shaderFlags = cameraComp.gameObject.getScene()->m_data.shaderFlags;
		s_data.sceneBufferData.skyBoxRotation = skyboxRotation;
		Renderer::getBuffer(BufferBinding::Scene)->setData(&s_data.sceneBufferData, sizeof(SceneBufferData));

		//transparent sorting
		std::sort(s_data.transparentDrawList.begin(), s_data.transparentDrawList.end(), [&](auto& a, auto& b) {
			return glm::distance(transform.worldPosition, glm::vec3(a.transform[3])) > glm::distance(transform.worldPosition, glm::vec3(b.transform[3]));//sort the vector backwards
			});

		
		bool renderedReflection = false;
		//reflection map pass
		//https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
		if (useReflectionMap && reflectionFrameBuffer && reflectionMap) {
			//						  fb,    index
			static std::unordered_map<void*, uint32_t> mapIndexMap;
			uint32_t& reflectionRenderIndex = mapIndexMap[cam.get()];
			reflectionFrameBuffer->bind();
			reflectionFrameBuffer->attachCubeMapFace(reflectionMap, reflectionRenderIndex);
			VFC::setEnabled(false);
			Renderer::uploadCameraBufferData(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 200.0f), calcView(reflectionRenderIndex, transform), transform.worldPosition, transform.eulerAnglesWorldDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
			RenderCommand::clear();
			drawSkyBox(camSkyBoxTexture);
			drawScene(nullptr);
			reflectionFrameBuffer->unbind();
			if (reflectionRenderIndex >= 5)
				reflectionRenderIndex = 0;
			else
				reflectionRenderIndex++;

			renderedReflection = true;
			VFC::setEnabled(true);

		}else
			reflectionMap = nullptr;
		VFC::setCamera(cameraComp.getFrustum());
		//default render pass
		cam->bindFrameBuffer();
		Renderer::uploadCameraBufferData(proj, view, transform.worldPosition, transform.eulerAnglesWorldDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
		drawSkyBox(camSkyBoxTexture);
		if (renderedReflection) {
			//rotation already applied
			s_data.sceneBufferData.skyBoxRotation = glm::mat4(1.0f);
			Renderer::getBuffer(BufferBinding::Scene)->setData(&s_data.sceneBufferData, sizeof(SceneBufferData));
		}
		drawScene(reflectionMap);
		cam->unbindFrameBuffer();
	}

	void SceneRenderer::Clear() {
		s_data.sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}
	void SceneRenderer::Clear(CameraComponent& cam) {
		RenderCommand::setDefault();
		switch (cam.settings.clearType)
		{
		case CameraComponent::ClearType::Color:
			RenderCommand::setClearColor(cam.settings.clearColor);
			s_data.sceneBufferData.clearColor = cam.settings.clearColor;
			break;
		case CameraComponent::ClearType::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
			s_data.sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
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
	
		
		sceneFbo->bind();
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
		RenderCommand::clear();
		sceneFbo->unbind();
		for (auto& goID : cams) {
			CameraComponent& cam = cams.get<CameraComponent>(goID);
			if (!cam.settings.isRenderTarget) {
				if (cam.postProcessing)
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getTexture(), *cam.postProcessing);
				else
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getTexture());
			}
		}
	}

	void SceneRenderer::ApplyPostProcessing(SceneCamera& sceneCam) {
		ApplyPostProcessing(sceneCam.getCamera()->getFrameBuffer(), sceneCam.getPostProcessingData());
	}
	void SceneRenderer::ApplyPostProcessing(CameraComponent& camera) {
		if(camera.postProcessing)
			ApplyPostProcessing(camera.getFrameBuffer(), *camera.postProcessing);
		else
			ApplyPostProcessing(camera.getFrameBuffer());
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data) {
		ApplyPostProcessing(frameBuffer, frameBuffer->getTexture(), data);
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& data) {
		ST_PROFILING_FUNCTION();
		s_data.postProcessingBufferData.time = Time::time;
		s_data.postProcessingBufferData.delta = Time::deltaTime;
		s_data.postProcessingBufferData.toneMappingExposure = data.exposure;
		s_data.postProcessingBufferData.gamma = data.gamma;
		Renderer::getBuffer(BufferBinding::PostProcessing)->setData(&s_data.postProcessingBufferData, sizeof(PostProcessingBufferData));


		//bloom
		if(data.bloomData.enabled)
			DoBloom(destination, source, data);

		//gamma correction and tonemapping
		{

			source->bind(0);
			s_data.quadPostProcShader->bind();
			destination->bind();
			float z = -1.0f;
			Renderer::getBuffer(BufferBinding::Model)->setData(&z, sizeof(float));
			RenderCommand::drawIndexed(Resources::getFullscreenVA(), 0);
			destination->unbind();
		}
	}

	void SceneRenderer::DoBloom(const Ref<FrameBuffer>& destination, const Ref<Texture>& source, PostProcessingData& postProcData) {
		ST_PROFILING_SCOPE("Bloom Pass");
		static Ref<ComputeShader> filterShader;
		if (!filterShader) {
			filterShader = ComputeShader::create("BloomFilterShader", R"(
#version 430 core
layout(local_size_x = 10, local_size_y = 10, local_size_z = 1) in;

layout(rgba16f, binding = 0) uniform readonly image2D source;
layout(rgba16f, binding = 1) uniform writeonly image2D destination;

layout(location = 1) uniform float u_treshold;

float when_gt(float x, float y) { return max(sign(x-y), float(0.0));}

void main() {
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = imageLoad(source, pixelCoord).rgba;
	color.r *= when_gt(color.r, u_treshold);
	color.g *= when_gt(color.g, u_treshold);
	color.b *= when_gt(color.b, u_treshold);
	imageStore(destination, pixelCoord, color);
}
)");
		}
		uint32_t width = source->getWidth();
		uint32_t height = source->getHeight();
		auto& data = postProcData.bloomData;


		if (width == 1 || height == 1)
			return;
		
		
		static float bloomTreshold = 0.0f;
		if (bloomTreshold != data.bloomTreshold) {
			bloomTreshold = data.bloomTreshold;
			filterShader->setFloat("u_treshold", data.bloomTreshold);
		}
		if (!data.downSample[0] || (data.downSample[0]->getWidth() != width || data.downSample[0]->getHeight() != height)) {
			data.samples = 0;
			TextureSettings settings;
			settings.format = (int)TextureSettings::Format::RGBA16F;
			for (uint32_t i = 0; i < 5; i++) {
				uint32_t texWidth = (uint32_t)(width / glm::pow(2, i));
				uint32_t texHeight = (uint32_t)(height / glm::pow(2, i));

				if (texWidth <= 5 || texHeight <= 5)
					break;

				data.downSample[i].reset();
				data.downSample[i] = Texture2D::create(texWidth, texHeight, settings);

				data.upSample[i].reset();
				data.upSample[i] = Texture2D::create(texWidth, texHeight, settings);

				data.samples++;
			}

		}
		filterShader->setTexture("source", 0, source, 0, AccesMode::ReadOnly);
		filterShader->setTexture("destination", 1, data.downSample[0], 0, AccesMode::WriteOnly);

		filterShader->Dispatch({ width, height, 1 }, ComputeShader::Usage::ShaderImageAcces);


	}

	void SceneRenderer::drawScene(const Ref<CubeMap>& reflectionMap) {
		ST_PROFILING_FUNCTION();
		UUID last = UUID();
		
		if (reflectionMap) {
			reflectionMap->bind(2);
			SkyBox::genrateBRDFLUT(ST_REFLECTION_MAP_SIZE)->bind(3);
		}

		//opaque
		for (const RenderObject& object : s_data.drawList) {
			if (object.boundingBox && object.transformComp) {
				if (!VFC::isInView(object.boundingBox, *object.transformComp)) {
					continue;
				}
			}

			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}

		//transparent
		for (const auto& object : s_data.transparentDrawList) {
			if (object.boundingBox && object.transformComp) {
				if (!VFC::isInView(object.boundingBox, *object.transformComp)) {
					continue;
				}
			}
			if (last != object.material->getUUID()) {
				object.material->uploadData();
				object.material->getShader()->bind();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}
	}
	void SceneRenderer::drawSkyBox(const Ref<CubeMap>& skybox) {
		ST_PROFILING_FUNCTION();  
		if (skybox && !(s_data.sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayDepth)) {
			RenderCommand::setWireFrame(false);
			auto& shader = Resources::getSkyBoxShader();
			shader->bind();
			skybox->bind();
			skybox->draw();
			if (s_data.sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayVertices)
				RenderCommand::setWireFrame(true);
		}
		
	}

	void SceneRenderer::RegisterObject(MeshRendererComponent& mesh, MeshFilterComponent& filter, TransformComponent& transform) {
		if (!filter.mesh.hasMesh)
			return;
		Ref<Material> material;
		if (mesh.material) {
			material = mesh.material;
		}
		else {
			material = Resources::getDefaultMaterial();
		}


		glm::mat4 normalMatrix = glm::transpose(glm::inverse(Math::createMat4(transform.worldPosition, transform.worldRotation, { 1,1,1 })));

		if (material->isTransparent())
			s_data.transparentDrawList.push_back(RenderObject{
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, filter.mesh.mesh->getBoundingBox(), &transform });
		else
			s_data.drawList.push_back(RenderObject{ 
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, filter.mesh.mesh->getBoundingBox(), &transform });
	}
	void SceneRenderer::RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& mat, const glm::mat4& transform) {
		if (!vertexArray)
			return;
		Ref<Material> material;
		if (mat) {
			material = mat;
		}
		else {
			material = Resources::getDefaultMaterial();
		}

		if (material->isTransparent())
			s_data.transparentDrawList.push_back(RenderObject{material,vertexArray,transform, glm::transpose(glm::inverse(transform)),CullMode::Back });
		else
			s_data.drawList.push_back(RenderObject{ material,vertexArray,transform, glm::transpose(glm::inverse(transform)), CullMode::Back });
	}
	void SceneRenderer::RegisterObject(const RenderObject& object) {
		s_data.drawList.push_back(object);
	}
}