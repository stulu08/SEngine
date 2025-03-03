#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Core/Resources.h"
#include "Stulu/Core/Time.h"

namespace Stulu {
	SceneRenderer::SceneRenderer(Scene* scene)
		: m_scene(scene) {
		m_postProcShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Shader");
		m_shadowShader = Renderer::getShaderSystem()->GetShader("Renderer/ShadowCaster");

		m_filterShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Filter");
		m_downSampleShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Down");
		m_upSampleShader = Renderer::getShaderSystem()->GetShader("Renderer/PostProcessing/Bloom/Up");

		resizeShadowMap();
	}

	void SceneRenderer::Begin() {
		for (int i = 0; i < 32; i++) {
			Resources::getWhiteTexture()->bind(i);
		}
	}
	void SceneRenderer::End() {
		m_drawList.clear();
		m_transparentDrawList.clear();
	}

	void SceneRenderer::LoadLights() {
		for (auto& goID : m_scene->getAllGameObjectsWith<LightComponent, TransformComponent>()) {
			GameObject go(goID, m_scene);
			if (go == m_scene->getMainLight()) {
				m_shadowCaster = m_lightBufferData.lightCount;
			}
			AddLight(go.getComponent<TransformComponent>(), go.getComponent<LightComponent>());
		}
		UploadLightBuffer();
		m_lightBufferData = LightBufferData();
		m_shadowCaster = 0;
	}
	void SceneRenderer::AddLight(const TransformComponent& transform, const LightComponent& light) {
		if (m_lightBufferData.lightCount < ST_MAXLIGHTS) {
			
			m_lightBufferData.lights[m_lightBufferData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
			m_lightBufferData.lights[m_lightBufferData.lightCount].positionAndType = glm::vec4(transform.worldPosition, light.lightType);
			m_lightBufferData.lights[m_lightBufferData.lightCount].rotation = glm::vec4(transform.forward, 1.0f);
			m_lightBufferData.lights[m_lightBufferData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
			m_lightBufferData.lightCount++;
		}
	}
	void SceneRenderer::UploadLightBuffer() {
		Renderer::getBuffer(BufferBinding::Light)->setData(&m_lightBufferData, sizeof(LightBufferData));
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam) {
		CameraComponent camComp = mainCam.gameObject.getComponent<CameraComponent>();
		camComp.cam = sceneCam.getCamera();
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam) {
		CameraComponent camComp = CameraComponent(CameraMode::Perspective);
		camComp.gameObject = GameObject(entt::null, m_scene);
		camComp.cam = sceneCam.getCamera();
		camComp.settings.zFar = sceneCam.m_zFar;
		camComp.settings.zNear = sceneCam.m_zNear;
		camComp.settings.fov = sceneCam.m_fov;
		camComp.settings.aspectRatio = sceneCam.m_aspectRatio;
		camComp.frustum = sceneCam.getFrustum();
		camComp.postProcessing = &sceneCam.getPostProcessingData();
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp) {
		ST_PROFILING_SCOPE("Renderer - Main Pass");

		const Ref<Camera>& cam = cameraComp.getNativeCamera();
		const glm::mat4& view = glm::inverse(transform.transform);
		const glm::mat4& proj = cameraComp.getProjection();

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
		m_sceneBufferData.env_lod = m_scene->m_data.graphicsData.env_lod;
		m_sceneBufferData.skyboxMapType = camSkyBoxMapType;
		m_sceneBufferData.useSkybox = camSkyBoxTexture != nullptr;
		m_sceneBufferData.shaderFlags = m_scene->m_data.shaderFlags;
		m_sceneBufferData.skyBoxRotation = skyboxRotation;
		m_sceneBufferData.lightSpaceMatrix = glm::mat4(1.0f);
		m_sceneBufferData.shadowCasterPos = glm::vec3(0.0f);
		m_sceneBufferData.shadowCaster = -1;
		Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

		//transparent sorting
		std::sort(m_transparentDrawList.begin(), m_transparentDrawList.end(), [&](auto& a, auto& b) {
			return glm::distance(transform.worldPosition, glm::vec3(a.transform[3])) > glm::distance(transform.worldPosition, glm::vec3(b.transform[3]));//sort the vector backwards
			});
		
		//shadow pass
		GameObject lightObject = m_scene->getMainLight();
		if(lightObject && m_shadowCaster > -1) {
			TransformComponent& lTC = lightObject.getComponent<TransformComponent>();
			LightComponent& light = lightObject.getComponent<LightComponent>();

			float halfDistance = m_scene->getData().graphicsData.shadowDistance / 2.0f;
			float halfFarDist = m_scene->getData().graphicsData.shadowFar/2.0f;
			float farDist = halfFarDist * 2.0f;
			glm::vec3 lightPos = transform.position + (halfFarDist * (-lTC.forward));
			glm::mat4 lightView = glm::inverse(Math::createMat4(lightPos, lTC.worldRotation, glm::vec3(1.0f)));
			glm::mat4 lightProj = glm::ortho(-halfDistance, halfDistance, halfDistance, -halfDistance, 0.01f, farDist);

			m_sceneBufferData.shadowCaster = m_shadowCaster;
			m_sceneBufferData.lightSpaceMatrix = lightProj * lightView;
			m_sceneBufferData.shadowCasterPos = lightPos;
			Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

			VFC::setEnabled(true);
			VFC::setCamera(VFC::createFrustum_ortho(-halfDistance, halfDistance, -halfDistance, halfDistance, 0.01f, farDist, lightPos, lTC.worldRotation));
			Renderer::uploadCameraBufferData(lightProj, lightView, lTC.worldPosition, lTC.eulerAnglesWorldDegrees, 0.01f, farDist);
			
			m_shadowMap->bind();
			RenderCommand::clear();
			drawSceneShadow();
			m_shadowMap->unbind();
			VFC::setEnabled(true);

			
			m_shadowMap->getDepthAttachment()->bind(4);
		}

		//default render pass
		VFC::setCamera(cameraComp.getFrustum());
		cam->bindFrameBuffer();
		Renderer::uploadCameraBufferData(proj, view, transform.worldPosition, transform.eulerAnglesWorldDegrees, cameraComp.settings.zNear, cameraComp.settings.zFar);
		drawSkyBox(camSkyBoxTexture);
		drawScene();
		cam->unbindFrameBuffer();
	}

	void SceneRenderer::Clear() {
		m_sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}
	void SceneRenderer::Clear(CameraComponent& cam) {
		RenderCommand::setDefault();
		switch (cam.settings.clearType)
		{
		case CameraComponent::ClearType::Color:
			RenderCommand::setClearColor(cam.settings.clearColor);
			m_sceneBufferData.clearColor = cam.settings.clearColor;
			break;
		case CameraComponent::ClearType::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
			m_sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
			break;
		}
		RenderCommand::clear();
	}

	


	void SceneRenderer::drawSceneShadow() {
		ST_PROFILING_SCOPE("Renderer - Schadow Pass");

		//opaque
		RenderCommand::setCullMode(CullMode::BackAndFront);
		m_shadowShader->bind();
		for (const RenderObject& object : m_drawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
					continue;
				}
			}
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}
	}
	void SceneRenderer::drawScene() {
		ST_PROFILING_SCOPE("Renderer - 3D Pass");

		UUID last = UUID();
		
		//opaque
		for (const RenderObject& object : m_drawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
					continue;
				}
			}

			if (last != object.material->getUUID()) {
				object.material->getShader()->bind();
				object.material->uploadData();
				last = object.material->getUUID();
			}
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}

		//transparent
		for (const auto& object : m_transparentDrawList) {
			if (object.boundingBox) {
				if (!VFC::isInView(object.boundingBox)) {
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
		if (skybox && !(m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayDepth)) {
			RenderCommand::setWireFrame(false);
			auto& shader = Resources::getSkyBoxShader();
			shader->bind();
			skybox->bind(0);
			skybox->draw();
			if (m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayVertices)
				RenderCommand::setWireFrame(true);
		}
		
	}

	void SceneRenderer::resizeShadowMap() {
		FrameBufferSpecs specs;
		specs.width = (uint32_t)m_scene->m_data.graphicsData.shadowMapSize;
		specs.height = (uint32_t)m_scene->m_data.graphicsData.shadowMapSize;
		specs.samples = 1;

		TextureSettings colorBuffer;
		colorBuffer.format = TextureFormat::None;
		colorBuffer.wrap = TextureWrap::ClampToBorder;
		colorBuffer.border = COLOR_WHITE;

		TextureSettings depthBuffer;
		depthBuffer.format = TextureFormat::Depth32F;
		depthBuffer.filtering = TextureFiltering::Bilinear;
		depthBuffer.wrap = TextureWrap::ClampToBorder;
		depthBuffer.border = COLOR_WHITE;

		m_shadowMap.reset();
		m_shadowMap = FrameBuffer::create(specs, colorBuffer, depthBuffer);
	}

	void SceneRenderer::drawAll2d(const TransformComponent& camera) {
		ST_PROFILING_SCOPE("Renderer - 2D Pass");

		struct Entry {
			entt::entity id;
			enum Type {
				Quad, Circle
			}type;
		};
		glm::vec3 camPos = camera.worldPosition;
		auto quadview = m_scene->m_registry.view<TransformComponent, SpriteRendererComponent>();
		auto circleView = m_scene->m_registry.view<TransformComponent, CircleRendererComponent>();
		std::vector<Entry> drawList;
		drawList.reserve(quadview.size_hint() + circleView.size_hint());
		for (auto gameObject : quadview) {
			drawList.push_back({ gameObject, Entry::Quad });
		}
		for (auto gameObject : circleView) {
			drawList.push_back({ gameObject, Entry::Circle });
		}
		std::sort(drawList.begin(), drawList.end(), [=](const Entry& left, const Entry& right)->bool {
			GameObject le = { left.id, m_scene };
			GameObject re = { right.id, m_scene };
			return glm::distance(camPos, le.getComponent<TransformComponent>().worldPosition) > glm::distance(camPos, re.getComponent<TransformComponent>().worldPosition);
		});
		for (Entry& entry : drawList) {
			if (entry.type == Entry::Quad) {
				auto [transform, sprite] = quadview.get(entry.id);
				if (sprite.texture)
					Renderer2D::drawTexturedQuad(transform.transform, sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color);
				else
					Renderer2D::drawQuad(transform.transform, sprite.color);
			}
			else {
				auto [transform, sprite] = circleView.get(entry.id);
				Renderer2D::drawCircle(transform.transform, sprite.color, sprite.thickness, sprite.fade);
			}
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

		Ref<BoundingBox> boundingBox;
		if (filter.mesh.mesh->getBoundingBox()) {
			boundingBox = filter.mesh.mesh->getBoundingBox()->copy();
			boundingBox->applyTransform(transform);
		}

		if (material->isTransparent())
			m_transparentDrawList.push_back(RenderObject{
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, boundingBox });
		else
			m_drawList.push_back(RenderObject{
			material,filter.mesh.mesh->getVertexArray(),transform.transform, normalMatrix, mesh.cullmode, boundingBox });
	}
	void SceneRenderer::RegisterObject(const Ref<VertexArray>& vertexArray, const Ref<Material>& mat, const glm::mat4& transform, const Ref<BoundingBox>& transformedBoundingBox) {
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
			m_transparentDrawList.push_back(RenderObject{material,vertexArray,transform, glm::transpose(glm::inverse(transform)),CullMode::Back,transformedBoundingBox });
		else
			m_drawList.push_back(RenderObject{ material,vertexArray,transform, glm::transpose(glm::inverse(transform)), CullMode::Back,transformedBoundingBox });
	}
	void SceneRenderer::RegisterObject(RenderObject&& object) {
		m_drawList.push_back(object);
	}

	void SceneRenderer::GenSceneTexture(const Ref<FrameBuffer>& sceneFbo) {
		ST_PROFILING_SCOPE("Renderer - Generating Texture");

		auto& cams = m_scene->getAllGameObjectsAsGroupWith<TransformComponent, CameraComponent>();

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
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getColorAttachment(), *cam.postProcessing);
				else
					ApplyPostProcessing(sceneFbo, cam.getFrameBuffer()->getColorAttachment());
			}
		}
	}

	void SceneRenderer::ApplyPostProcessing(SceneCamera& sceneCam) {
		if (sceneCam.getPostProcessingUsingMain() && Scene::activeScene()) {
			GameObject mainCam = Scene::activeScene()->getMainCamera();
			if (mainCam && mainCam.hasComponent<PostProcessingComponent>()) {
				PostProcessingComponent comp = mainCam.getComponent<PostProcessingComponent>();

				comp.data.bloom = sceneCam.getPostProcessingData().bloom;
				ApplyPostProcessing(sceneCam.getCamera()->getFrameBuffer(), comp.data);
				sceneCam.getPostProcessingData().bloom = comp.data.bloom;

				return;
			}
		}

		ApplyPostProcessing(sceneCam.getCamera()->getFrameBuffer(), sceneCam.getPostProcessingData());
	}
	void SceneRenderer::ApplyPostProcessing(CameraComponent& camera) {
		if (camera.postProcessing)
			ApplyPostProcessing(camera.getFrameBuffer(), *camera.postProcessing);
		else
			ApplyPostProcessing(camera.getFrameBuffer());
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data) {
		ApplyPostProcessing(frameBuffer, frameBuffer->getColorAttachment(), data);
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Ref<Texture2D>& source, PostProcessingData& data) {
		ST_PROFILING_SCOPE("Renderer - PostProcessing");
		const auto& settings = data.settings;

		m_postProcessingBufferData.time = Time::time;
		m_postProcessingBufferData.delta = Time::deltaTime;
		m_postProcessingBufferData.enableGammaCorrection = settings.gammaCorrection.enabled ? 1.0f : 0.0f;
		m_postProcessingBufferData.toneMappingExposure = settings.gammaCorrection.exposure;
		m_postProcessingBufferData.gamma = settings.gammaCorrection.gamma;
		m_postProcessingBufferData.bloomStrength = settings.bloom.enabled ? settings.bloom.intensity : 0.0f;
		Renderer::getBuffer(BufferBinding::PostProcessing)->setData(&m_postProcessingBufferData, sizeof(PostProcessingBufferData));

		//bloom
		Ref<Texture> bloomResult = Resources::getBlackTexture();
		if (settings.bloom.enabled) {
			bloomResult = DoBloom(destination, source, data);
		}

		//gamma correction, tonemapping, apply bloom
		{
			source->bind(0);
			bloomResult->bind(1);

			Renderer::ScreenQuad(destination, m_postProcShader);
			return;
		}
	}
	Ref<Texture> SceneRenderer::DoBloom(const Ref<FrameBuffer>& destination, const Ref<Texture2D>& source, PostProcessingData& postProcData) {
		ST_PROFILING_SCOPE("Renderer - Bloom");
		auto& data = postProcData.bloom;
		const auto& settings = postProcData.settings.bloom;

		uint32_t width = (uint32_t)((float)source->getWidth() * settings.resolutionScale);
		uint32_t height = (uint32_t)((float)source->getHeight() * settings.resolutionScale);

		if (width <= 1 || height <= 1 || !settings.enabled)
			return Resources::getBlackTexture();

		if (data.downSample == nullptr || data.downSample->getWidth() != width || data.downSample->getHeight() != height) {
			float logs = glm::log2((float)glm::max(width, height)) + glm::min((float)settings.diffusion, (float)BLOOM_MAX_SAMPLES) - BLOOM_MAX_SAMPLES;
			int32_t logs_i = (int)glm::floor(logs);

			data.downSample.reset();
			data.upSample.reset();
			data.samples = glm::clamp(logs_i, 1, BLOOM_MAX_SAMPLES);
			data.sampleScale = 0.5f + logs - logs_i;

			TextureSettings settings;
			settings.format = TextureFormat::RGBA32F;
			settings.filtering = TextureFiltering::Trilinear;
			settings.wrap = TextureWrap::ClampToEdge;
			settings.levels = data.samples;

			data.downSample = Texture2D::create(width, height, settings);
			data.upSample = Texture2D::create(width, height, settings);

		}
		struct Data {
			glm::vec4 filter;
			glm::vec2 desRes;
			float sampleScale;
			float clamp;
			uint32_t iteration;
		}updata;

		static Ref<ShaderStorageBuffer> storageBuffer = ShaderStorageBuffer::create(sizeof(Data), 6);

		
		float soft = settings.threshold * settings.knee;
		updata.filter.x = settings.threshold;
		updata.filter.y = settings.threshold - soft;
		updata.filter.z = 2.0f * soft;
		updata.filter.w = 0.25f / (soft + 0.00001f);
		updata.sampleScale = data.sampleScale;
		updata.clamp = settings.clamp;
		updata.iteration = 0;

		for (int i = 0; i < (int)data.samples; i++) {
			Ref<Shader> shader = (i == 0) ? m_filterShader : m_downSampleShader;
			Ref<Texture2D> sourceTex = (i == 0) ? source : data.downSample;

			updata.desRes = glm::vec2(data.downSample->getMipWidth(i), data.downSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			shader->setTexture("destination", 2, data.downSample, i, AccesMode::WriteOnly);
			shader->Dispatch({ data.downSample->getMipWidth(i), data.downSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);
			updata.iteration = i;
		}

		for (int i = (int)data.samples - 2; i >= 0; i--) {
			Ref<Texture2D> sourceTex = (i == (int)data.samples - 2) ? data.downSample : data.upSample;

			updata.desRes = glm::vec2(data.upSample->getMipWidth(i), data.upSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			data.downSample->bind(1);
			m_upSampleShader->setTexture("destination", 2, data.upSample, i, AccesMode::WriteOnly);
			m_upSampleShader->Dispatch({ data.upSample->getMipWidth(i), data.upSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);

			updata.iteration = i;
		}

		return data.upSample;
	}

}