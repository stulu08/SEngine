#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Resources/Resources.h"
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

		m_dispatcher = &Application::get().GetCpuDispatcher();
	}

	void SceneRenderer::Begin() {
		for (int i = 0; i < 32; i++) {
			Resources::WhiteTexture()->bind(i);
		}
		LoadLights();
		Register3dObjects();
	}
	void SceneRenderer::End() {
		m_drawList.clear();
		m_transparentDrawList.clear();
	}

	void SceneRenderer::LoadLights() {
		for (auto& goID : m_scene->GetAllWith<LightComponent, TransformComponent>()) {
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
			m_lightBufferData.lights[m_lightBufferData.lightCount].positionAndType = glm::vec4(transform.GetWorldPosition(), light.lightType);
			m_lightBufferData.lights[m_lightBufferData.lightCount].rotation = glm::vec4(transform.GetForward(), 1.0f);
			m_lightBufferData.lights[m_lightBufferData.lightCount].spotLightData = glm::vec4(glm::cos(glm::radians(light.spotLight_cutOff)), glm::cos(glm::radians(light.spotLight_outerCutOff)), 1.0f, light.areaRadius);
			m_lightBufferData.lightCount++;
		}
	}
	void SceneRenderer::UploadLightBuffer() {
		Renderer::getBuffer(BufferBinding::Light)->setData(&m_lightBufferData, sizeof(LightBufferData));
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam, CameraComponent& mainCam) {
		CameraComponent camComp = mainCam.gameObject.getComponent<CameraComponent>();
		camComp.gameObject = mainCam.gameObject;
		camComp.m_cam = sceneCam.getCamera();
		camComp.m_far = sceneCam.m_zFar;
		camComp.m_near = sceneCam.m_zNear;
		camComp.m_fov = sceneCam.m_fov;
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(SceneCamera& sceneCam) {
		CameraComponent camComp = CameraComponent(sceneCam.getCamera());
		camComp.gameObject = GameObject(entt::null, m_scene);
		camComp.m_far = sceneCam.m_zFar;
		camComp.m_near = sceneCam.m_zNear;
		camComp.m_fov = sceneCam.m_fov;
		DrawSceneToCamera(sceneCam.m_transform, camComp);
	}
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp) {
		ST_PROFILING_SCOPE("Renderer - Main Pass");

		const Camera& cam = cameraComp.GetNativeCamera();
		const glm::mat4& view = glm::inverse(transform.GetWorldTransform());
		const glm::mat4& proj = cameraComp.GetProjection();

		SkyBox* camSkyBoxTexture = nullptr;
		glm::mat4 skyboxRotation = glm::mat4(1.0f);
		if (cameraComp.gameObject.IsValid()) {
			if (cameraComp.GetClearType() == ClearType::Skybox && cameraComp.gameObject.hasComponent<SkyBoxComponent>()) {
				auto& s = cameraComp.gameObject.getComponent<SkyBoxComponent>();
				camSkyBoxTexture = *s.texture;
				skyboxRotation = Math::createMat4(glm::vec3(.0f), glm::quat(glm::radians(s.rotation)), glm::vec3(1.0f));
			}
		}
		
		//upload shader scene data
		m_sceneBufferData.env_lod = m_scene->getData().graphicsData.env_lod;
		m_sceneBufferData.useSkybox = camSkyBoxTexture != nullptr;
		m_sceneBufferData.shaderFlags = m_scene->getData().shaderFlags;
		m_sceneBufferData.skyBoxRotation = skyboxRotation;
		m_sceneBufferData.lightSpaceMatrix = glm::mat4(1.0f);
		m_sceneBufferData.shadowCasterPos = glm::vec3(0.0f);
		m_sceneBufferData.shadowCaster = -1;
		Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

		//transparent sorting
		std::sort(m_transparentDrawList.begin(), m_transparentDrawList.end(), [&](auto& a, auto& b) {
			return glm::distance(transform.GetWorldPosition(), glm::vec3(a.transform[3])) > glm::distance(transform.GetWorldPosition(), glm::vec3(b.transform[3]));//sort the vector backwards
			});
		
		//shadow pass
		GameObject lightObject = m_scene->getMainLight();
		if(lightObject && m_shadowCaster > -1) {
			TransformComponent& lTC = lightObject.getComponent<TransformComponent>();
			LightComponent& light = lightObject.getComponent<LightComponent>();

			float halfDistance = m_scene->getData().graphicsData.shadowDistance / 2.0f;
			float halfFarDist = m_scene->getData().graphicsData.shadowFar/2.0f;
			float farDist = halfFarDist * 2.0f;
			glm::vec3 lightPos = transform.position + (halfFarDist * (-lTC.GetForward()));
			glm::mat4 lightView = glm::inverse(Math::createMat4(lightPos, lTC.GetWorldRotation(), glm::vec3(1.0f)));
			glm::mat4 lightProj = glm::ortho(-halfDistance, halfDistance, halfDistance, -halfDistance, 0.01f, farDist);

			m_sceneBufferData.shadowCaster = m_shadowCaster;
			m_sceneBufferData.lightSpaceMatrix = lightProj * lightView;
			m_sceneBufferData.shadowCasterPos = lightPos;
			Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

			VFC::setEnabled(true);
			VFC::setCamera(VFC::createFrustum_ortho(-halfDistance, halfDistance, -halfDistance, halfDistance, 0.01f, farDist, lightPos, lTC.GetWorldRotation()));
			Renderer::uploadCameraBufferData(lightProj, lightView, lTC.GetWorldPosition(), lTC.GetWorldEulerRotation(), 0.01f, farDist);
			
			m_shadowMap->bind();
			RenderCommand::clear();
			drawSceneShadow();
			m_shadowMap->unbind();
			VFC::setEnabled(true);

			
			m_shadowMap->getDepthAttachment()->bind(4);
		}

		//default render pass
		VFC::setCamera(cameraComp.CalculateFrustum(transform));
		cam.bindFrameBuffer();
		Renderer::uploadCameraBufferData(proj, view, transform.GetWorldPosition(), transform.GetWorldEulerRotation(), cameraComp.GetNear(), cameraComp.GetFar());
		drawSkyBox(camSkyBoxTexture);
		drawScene();
		cam.unbindFrameBuffer();
	}

	void SceneRenderer::Clear() {
		m_sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}
	void SceneRenderer::Clear(CameraComponent& cam) {
		RenderCommand::setDefault();
		switch (cam.GetClearType())
		{
		case ClearType::Color:
			RenderCommand::setClearColor(cam.GetClearColor());
			m_sceneBufferData.clearColor = cam.GetClearColor();
			break;
		case ClearType::Skybox:
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
			object.material->GetShader()->bind();
			object.material->RenderReady();

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
			object.material->GetShader()->bind();
			object.material->RenderReady();
			RenderCommand::setCullMode(object.cullmode);
			if (object.vertexArray) {
				Renderer::submit(object.vertexArray, nullptr, object.transform, object.normalMatrix);
			}
		}
	}
	void SceneRenderer::drawSkyBox(const CubeMap* skybox) {
		if (skybox && !(m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayDepth)) {
			RenderCommand::setWireFrame(false);
			Resources::SkyBoxShader()->bind();
			skybox->bind(0);
			skybox->draw();
			if (m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayVertices)
				RenderCommand::setWireFrame(true);
		}
		
	}

	void SceneRenderer::resizeShadowMap() {
		FrameBufferSpecs specs;
		specs.width = (uint32_t)m_scene->getData().graphicsData.shadowMapSize;
		specs.height = (uint32_t)m_scene->getData().graphicsData.shadowMapSize;
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

	void SceneRenderer::Register3dObjects() {
		auto group = m_scene->GetRegistry().view<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
		for (auto gameObject : group) {
			if (group.get<MeshFilterComponent>(gameObject).mesh.IsValid()) {
				RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
			}
		}
	}

	void SceneRenderer::drawAll2d(const TransformComponent& camera) {
		ST_PROFILING_SCOPE("Renderer - 2D Pass");

		struct Entry {
			entt::entity id;
			enum Type {
				Quad, Circle
			}type;
		};
		const glm::vec3 camPos = camera.GetWorldPosition();
		auto quadview = m_scene->GetRegistry().view<TransformComponent, SpriteRendererComponent>();
		auto circleView = m_scene->GetRegistry().view<TransformComponent, CircleRendererComponent>();

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
			return glm::distance(camPos, le.getComponent<TransformComponent>().GetWorldPosition()) > glm::distance(camPos, re.getComponent<TransformComponent>().GetWorldPosition());
		});

		for (Entry& entry : drawList) {
			if (entry.type == Entry::Quad) {
				auto [transform, sprite] = quadview.get(entry.id);
				if (sprite.texture.IsLoaded())
					Renderer2D::drawTexturedQuad(transform.GetWorldTransform(), *sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color);
				else
					Renderer2D::drawQuad(transform.GetWorldTransform(), sprite.color);
			}
			else {
				auto [transform, sprite] = circleView.get(entry.id);
				Renderer2D::drawCircle(transform.GetWorldTransform(), sprite.color, sprite.thickness, sprite.fade);
			}
		}
	}

	void SceneRenderer::RegisterObject(MeshRendererComponent& meshRenderer, MeshFilterComponent& meshFilter, TransformComponent& transform) {
		if (!meshFilter.mesh.IsLoaded())
			return;
		Mesh* mesh = *meshFilter.mesh;
		TestMaterial* material = meshRenderer.material.IsLoaded() ? *meshRenderer.material : Resources::DefaultMaterial();

		glm::mat4 normalMatrix = glm::transpose(glm::inverse(Math::createMat4(transform.GetWorldPosition(), transform.GetWorldRotation(), {1,1,1})));

		Ref<BoundingBox> boundingBox;
		if (mesh->GetBoundingBox()) {
			boundingBox = mesh->GetBoundingBox()->copy();
			boundingBox->applyTransform(transform);
		}

		if (material->IsTransparent())
			m_transparentDrawList.push_back(RenderObject {
				material,
				mesh->GetVertexArray(),
				transform.GetWorldTransform(), 
				normalMatrix, 
				meshRenderer.cullmode,
				boundingBox
				});
		else
			m_drawList.push_back(RenderObject {
				material,
				mesh->GetVertexArray(),
				transform.GetWorldTransform(),
				normalMatrix,
				meshRenderer.cullmode,
				boundingBox
			});
	}
	void SceneRenderer::RegisterObject(const Ref<VertexArray>& vertexArray, TestMaterial* providedMaterial, const glm::mat4& transform, const Ref<BoundingBox>& transformedBoundingBox) {
		if (!vertexArray)
			return;
		TestMaterial* material = ((providedMaterial != nullptr) ? providedMaterial : Resources::DefaultMaterial());

		if (material->IsTransparent())
			m_transparentDrawList.push_back(RenderObject{material,vertexArray,transform, glm::transpose(glm::inverse(transform)),CullMode::Back,transformedBoundingBox });
		else
			m_drawList.push_back(RenderObject{ material,vertexArray,transform, glm::transpose(glm::inverse(transform)), CullMode::Back,transformedBoundingBox });
	}
	void SceneRenderer::RegisterObject(RenderObject&& object) {
		m_drawList.push_back(object);
	}

	void SceneRenderer::GenSceneTexture(const Ref<FrameBuffer>& sceneFbo) {
		ST_PROFILING_SCOPE("Renderer - Generating Texture");

		auto& cams = m_scene->GetAllAsGroupWith<TransformComponent, CameraComponent>();

		cams.sort([&](const entt::entity a, const entt::entity b) {
			int32_t dA = cams.get<CameraComponent>(a).GetDepth();
			int32_t dB = cams.get<CameraComponent>(b).GetDepth();
			return dA < dB;
		});


		sceneFbo->bind();
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, .0f));
		RenderCommand::clear();
		sceneFbo->unbind();

		for (auto& goID : cams) {
			CameraComponent& cam = cams.get<CameraComponent>(goID);
			GameObject gameObject = cam.gameObject;
			PostProcessingData* postProcessing = nullptr;

			if (!postProcessing && gameObject.hasComponent<PostProcessingComponent>()) {
				postProcessing = &gameObject.getComponent<PostProcessingComponent>().data;
			}

			if (!cam.IsRenderTarget()) {
				if (postProcessing) {
					ApplyPostProcessing(sceneFbo, cam.GetFrameBuffer()->getColorAttachment().get(), *postProcessing);
				}
				else {
					ApplyPostProcessing(sceneFbo, cam.GetFrameBuffer()->getColorAttachment().get());
				}
			}
		}
	}

	void SceneRenderer::ApplyPostProcessing(SceneCamera& sceneCam) {
		if (sceneCam.getPostProcessingUsingMain() && m_scene) {
			GameObject mainCam = m_scene->getMainCamera();
			if (mainCam && mainCam.hasComponent<PostProcessingComponent>()) {
				PostProcessingComponent comp = mainCam.getComponent<PostProcessingComponent>();

				comp.data.bloom = sceneCam.getPostProcessingData().bloom;
				ApplyPostProcessing(sceneCam.getCamera().getFrameBuffer(), comp.data);
				sceneCam.getPostProcessingData().bloom = comp.data.bloom;

				return;
			}
		}

		ApplyPostProcessing(sceneCam.getCamera().getFrameBuffer(), sceneCam.getPostProcessingData());
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingData& data) {
		ApplyPostProcessing(frameBuffer, frameBuffer->getColorAttachment().get(), data);
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingData& data) {
		ST_PROFILING_SCOPE("Renderer - PostProcessing");
		const auto& settings = data.settings;

		m_postProcessingBufferData.time = Time::applicationRuntime;
		m_postProcessingBufferData.delta = Time::deltaTime;
		m_postProcessingBufferData.enableGammaCorrection = settings.gammaCorrection.enabled ? 1.0f : 0.0f;
		m_postProcessingBufferData.toneMappingExposure = settings.gammaCorrection.exposure;
		m_postProcessingBufferData.gamma = settings.gammaCorrection.gamma;
		m_postProcessingBufferData.bloomStrength = settings.bloom.enabled ? settings.bloom.intensity : 0.0f;
		Renderer::getBuffer(BufferBinding::PostProcessing)->setData(&m_postProcessingBufferData, sizeof(PostProcessingBufferData));

		//bloom
		Texture* bloomResult = Resources::BlackTexture();
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
	Texture* SceneRenderer::DoBloom(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingData& postProcData) {
		ST_PROFILING_SCOPE("Renderer - Bloom");
		auto& data = postProcData.bloom;
		const auto& settings = postProcData.settings.bloom;

		uint32_t width = (uint32_t)((float)source->getWidth() * settings.resolutionScale);
		uint32_t height = (uint32_t)((float)source->getHeight() * settings.resolutionScale);

		if (width <= 1 || height <= 1 || !settings.enabled)
			return Resources::BlackTexture();

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
			const Texture2D* sourceTex = (i == 0) ? source : data.downSample.get();

			updata.desRes = glm::vec2(data.downSample->getMipWidth(i), data.downSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			shader->setTexture("destination", 2, data.downSample, i, AccesMode::WriteOnly);
			shader->Dispatch({ data.downSample->getMipWidth(i), data.downSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);
			updata.iteration = i;
		}

		for (int i = (int)data.samples - 2; i >= 0; i--) {
			const Texture2D* sourceTex = (i == (int)data.samples - 2) ? data.downSample.get() : data.upSample.get();

			updata.desRes = glm::vec2(data.upSample->getMipWidth(i), data.upSample->getMipHeight(i)) - glm::vec2(1, 1);
			storageBuffer->setData(&updata, sizeof(Data));

			sourceTex->bind(0);
			data.downSample->bind(1);
			m_upSampleShader->setTexture("destination", 2, data.upSample, i, AccesMode::WriteOnly);
			m_upSampleShader->Dispatch({ data.upSample->getMipWidth(i), data.upSample->getMipHeight(i), 1 }, ComputeUsage::ShaderImageAcces | ComputeUsage::TextureFetch);

			updata.iteration = i;
		}

		return data.upSample.get();
	}

}