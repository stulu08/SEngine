#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Resources/Resources.h"
#include "Stulu/Core/Time.h"
#include "Stulu/Scripting/EventCaller.h"

namespace Stulu {
	SceneRenderer::SceneRenderer(Scene* scene)
		: m_scene(scene) {
		m_shadowShader = Renderer::getShaderSystem()->GetShader("Renderer/ShadowCaster");

		resizeShadowMap();

		m_dispatcher = &Application::get().GetCpuDispatcher();
	}

	void SceneRenderer::Begin() {
		for (int i = 0; i < 32; i++) {
			Resources::WhiteTexture()->bind(i);
		}
		LoadLights();
		Register3dObjects();
		m_stats = RenderStats();
	}
	void SceneRenderer::End() {
		m_drawList.clear();
		m_transparentDrawList.clear();
	}

	void SceneRenderer::LoadLights() {
		m_lightBufferData = LightBufferData();
		GameObject MainLight = m_scene->getMainLight();

		for (auto& goID : m_scene->GetAllWith<LightComponent>()) {
			GameObject go(goID, m_scene);
			AddLight(go.getComponent<LightComponent>());
		}

		UploadLightBuffer();
	}
	void SceneRenderer::AddLight(const LightComponent& light) {
		if (m_lightBufferData.lightCount < ST_MAXLIGHTS) {
			m_lightBufferData.lights[m_lightBufferData.lightCount] = light.GetRenderData();
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
	void SceneRenderer::DrawSceneToCamera(TransformComponent& transform, CameraComponent& cameraComp, bool callEvents) {
		ST_PROFILING_SCOPE("Renderer - Main Pass");
		RenderCommand::setDefault();

		// find skybox
		TestMaterial* camSkyboxMaterial = nullptr;
		if (cameraComp.gameObject.IsValid()) {
			if (cameraComp.GetClearType() == ClearType::Skybox && cameraComp.gameObject.hasComponent<SkyBoxComponent>()) {
				const SkyBoxComponent& s = cameraComp.gameObject.getComponent<SkyBoxComponent>();
				if (s.material.IsValid()) {
					camSkyboxMaterial = *s.material;
				}

				m_sceneBufferData.skyBoxRotation = Math::createMat4(glm::vec3(.0f), glm::quat(glm::radians(s.rotation)), glm::vec3(1.0f));
				m_sceneBufferData.EnvironmentStrength = s.LightStrength;
				m_sceneBufferData.EnvironmentReflections = s.ReflectionIntensity;
			}
		}

		//transparent sorting
		std::sort(m_transparentDrawList.begin(), m_transparentDrawList.end(),
			[&](const RenderObject& a, const RenderObject& b) {
				return
					glm::distance(transform.GetWorldPosition(), glm::vec3(a.transform->GetWorldPosition()))
						>
					glm::distance(transform.GetWorldPosition(), glm::vec3(b.transform->GetWorldPosition()));
			});

		// upload scene data
		m_sceneBufferData.useSkybox = camSkyboxMaterial != nullptr;
		m_sceneBufferData.shaderFlags = m_scene->getData().shaderFlags;
		m_sceneBufferData.fogSettings = m_scene->getData().graphicsData.fog;
		m_sceneBufferData.ShadowSettingsPack = glm::vec4(0.0f);
		Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

		// shadow pass
		CascadeShadowPass(transform, cameraComp);

		//default render pass
		VFC::setEnabled(true);
		VFC::setCamera(cameraComp.CalculateFrustum(transform));
		Renderer::uploadCameraBufferData(
			cameraComp.GetProjection(), glm::inverse(transform.GetWorldTransform()), 
			transform.GetWorldPosition(), transform.GetWorldEulerRotation(), 
			cameraComp.GetNear(), cameraComp.GetFar());
		

		const Camera& nativeCam = cameraComp.GetNativeCamera();
		nativeCam.getRenderFrameBuffer()->bind();

		Clear(cameraComp);
		drawSkyBox(camSkyboxMaterial);

		drawScene();
		drawAll2d(transform, callEvents);

		nativeCam.getRenderFrameBuffer()->unbind();

		if (nativeCam.HasResultFrameBuffer())
			Renderer::BlibRenderBuffferToResultBuffer(nativeCam.getRenderFrameBuffer(), nativeCam.getResultFrameBuffer(), true, true, true);
	}

	void SceneRenderer::Clear() {
		m_sceneBufferData.clearColor = glm::vec4(.0f, .0f, .0f, 1.0f);
		RenderCommand::setClearColor(glm::vec4(.0f, .0f, .0f, 1.0f));
		RenderCommand::clear();
	}
	void SceneRenderer::Clear(CameraComponent& cam) {
		switch (cam.GetClearType())
		{
		case ClearType::Color:
			RenderCommand::setClearColor(cam.GetClearColor());
			m_sceneBufferData.clearColor = cam.GetClearColor();
			break;
		case ClearType::Skybox:
			RenderCommand::setClearColor(glm::vec4(.0f));
			m_sceneBufferData.clearColor = glm::vec4(.0f);
			break;
		}
		RenderCommand::clear();
	}



	void SceneRenderer::drawSceneShadow() {
		ST_PROFILING_SCOPE("Renderer - Schadow Pass");
		//opaque only
		RenderCommand::setCullMode(CullMode::BackAndFront);
		m_shadowShader->bind();

		for (const RenderObject& object : m_drawList) {
			if (!VFC::isInView(&object.transform->GetBounds())) {
				continue;
			}
			const glm::mat4& transform = object.transform->GetWorldTransform();
			Renderer::UploadModelData(transform, transform);
			RenderCommand::drawIndexed(object.meshFilter->GetMesh()->GetVertexArray());
			m_stats.shadowDrawCalls++;
		}
	}
	inline bool SceneRenderer::DrawObject(const RenderObject& object) {
		if (!VFC::isInView(&object.transform->GetBounds())) {
			return false;
		}
		// cull
		RenderCommand::setCullMode(object.meshRenderer->cullmode);
		
		// stencil
		uint8_t stencil = object.meshRenderer->StencilValue;
		if (stencil) {
			RenderCommand::StencilAlways(stencil, 1);
			RenderCommand::SetStencilValue(object.meshRenderer->StencilValue);
			object.meshRenderer->StencilValue = 0;
		}
		
		// model matrix
		const glm::mat4& transform = object.transform->GetWorldTransform();
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(
			Math::createMat4(object.transform->GetWorldPosition(), object.transform->GetWorldRotation(), { 1,1,1 })));
		Renderer::UploadModelData(transform, normalMatrix, (uint32_t)object.transform->gameObject.GetID());

		// render mesh(es)
		const auto& mesh = object.meshFilter->GetMesh();
		const size_t subMeshesCount = mesh->GetSubmeshes().size();

		// RenderCommandDraw will falback to full mesh if index out of range
		for (size_t index = 0; index < glm::max(subMeshesCount, 1ull); index++) {
			MaterialAsset material = object.meshRenderer->GetMaterial(index);
			if (!material.IsValid()) {
				material = Resources::DefaultMaterialAsset();
			}
			material->GetShader()->bind();
			material->RenderReady();

			m_stats.drawCalls++;
			mesh->RenderCommandDraw((int32_t)index, 0);
		}

		if (stencil) {
			RenderCommand::StencilAlways(stencil, 0);
			RenderCommand::SetStencilValue(0x00);
		}

		return true;
	}

	void SceneRenderer::drawScene() {
		ST_PROFILING_SCOPE("Renderer - 3D Pass");

		//opaque
		for (const RenderObject& object : m_drawList) {
			DrawObject(object);
		}

		//transparent
		for (const auto& object : m_transparentDrawList) {
			DrawObject(object);
		}
	}
	void SceneRenderer::drawSkyBox(TestMaterial* skybox) {
		if (skybox && !(m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayDepth)) {
			RenderCommand::setWireFrame(false);

			skybox->GetShader()->bind();
			skybox->RenderReady();

			if (!m_scene->getData().graphicsData.transparentBG) {
				RenderCommand::setDepthTesting(false);
				Renderer::RenderSkyBoxCube();
				RenderCommand::setDepthTesting(true);
			}

			if (m_sceneBufferData.shaderFlags & ST_ShaderViewFlags_DisplayVertices)
				RenderCommand::setWireFrame(true);
		}

	}

	void SceneRenderer::resizeShadowMap() {
		const auto& settings = m_scene->getData().graphicsData.shadows;

		FrameBufferSpecs specs;
		specs.width = settings.MapSize;
		specs.height = settings.MapSize;
		specs.samples = MSAASamples::Disabled;

		TextureSettings colorBuffer = TextureFormat::None;

		TextureSettings depthBuffer = TextureFormat::Depth32F;
		depthBuffer.filtering = TextureFiltering::Linear;
		depthBuffer.wrap = TextureWrap::ClampToBorder;
		depthBuffer.arraySize = (uint32_t)glm::clamp((int32_t)settings.CascadeSplits.size(), 2, ST_MAX_SHADOW_CASCADES);
		depthBuffer.border = COLOR_WHITE;

		m_shadowMap.reset();
		m_shadowMap = FrameBuffer::create(specs, colorBuffer, depthBuffer);
	}
	std::tuple<glm::mat4, glm::mat4, glm::vec2> SceneRenderer::GetLightSpaceMatrix(float nearPlane, float farPlane, const TransformComponent& cameraTransform, const CameraComponent& cameraComp, const TransformComponent& lightTransform) const {
		const auto& settings = m_scene->getData().graphicsData.shadows;

		const glm::mat4 cameraProj = glm::perspective(glm::radians(cameraComp.GetFov()), cameraComp.GetAspect(), nearPlane, farPlane);
		const glm::mat4 cameraView = glm::inverse(cameraTransform.GetWorldTransform());
		
		const auto cameraCorners = Math::GetFrustumCornersWorldSpace(cameraProj, cameraView);
		glm::vec3 center = glm::vec3(0, 0, 0);
		for (const glm::vec4& v : cameraCorners) {
			center += glm::vec3(v);
		}
		center /= cameraCorners.size();

		const glm::vec3 lightDir = glm::normalize(lightTransform.GetForward());
		const glm::mat4 lightView = glm::lookAt(center - lightDir, center, TRANSFORM_UP_DIRECTION);

		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());
		for (const glm::vec4& v : cameraCorners) {
			const glm::vec4 lv = lightView * v;
			min = glm::min(min, glm::vec3(lv));
			max = glm::max(max, glm::vec3(lv));
		}

		// add depth padding
		float zPadding = (max.z - min.z) * (settings.ZMult - 1.0f);
		min.z -= zPadding * 0.5f;
		max.z += zPadding * 0.5f;

		// texel snapping
		const glm::vec2 orthoSize = glm::vec2(max.x - min.x, max.y - min.y);
		const float shadowMapResolution = (float)settings.MapSize;
		const glm::vec2 texelSize = orthoSize / shadowMapResolution;

		glm::vec3 shadowCenter = (min + max) * 0.5f;
		shadowCenter.x = std::floor(shadowCenter.x / texelSize.x) * texelSize.x;
		shadowCenter.y = std::floor(shadowCenter.y / texelSize.y) * texelSize.y;

		const float halfWidth = orthoSize.x * 0.5f;
		const float halfHeight = orthoSize.y * 0.5f;

		min.x = shadowCenter.x - halfWidth;
		max.x = shadowCenter.x + halfWidth;
		min.y = shadowCenter.y - halfHeight;
		max.y = shadowCenter.y + halfHeight;

		const glm::mat4 lightProjection = glm::ortho(min.x, max.x, min.y, max.y, min.z, max.z);
		return { lightProjection, lightView, glm::vec2(glm::abs(max.x - min.x), glm::abs(max.y - min.y)) };
	}

	void SceneRenderer::CascadeShadowPass(const TransformComponent& transform, const CameraComponent& cameraComp) {
		ST_PROFILING_SCOPE("Renderer - Cascade Shadow");

		GameObject lightObject = m_scene->getMainLight();
		if (lightObject) {
			const LightComponent& light = lightObject.getComponent<LightComponent>();
			const TransformComponent& lTC = lightObject.getComponent<TransformComponent>();
			auto& settings = m_scene->getData().graphicsData.shadows;
			const size_t cascadeCount = glm::min(settings.CascadeSplits.size(), (size_t)ST_MAX_SHADOW_CASCADES);

			if (settings.CascadeSplits.size() != cascadeCount) {
				CORE_WARN("Invalid CascadeSplits provided!");
				settings.CascadeSplits = Math::CalculateCascadeSplits(cascadeCount, settings.FarPlane);
			}

			const float lightSize = light.Directional.Size;
			
			m_sceneBufferData.ShadowSettingsPack = glm::vec4(
				settings.BlendingDistance, (float)settings.SampleQuality, 
				(float)cascadeCount, lightSize / (float)settings.MapSize);

			m_sceneBufferData.ShadowSettingsPack2 = glm::vec4(
				light.Shadows.Soft ? 1.0f : 0.0f, (float)settings.PCSSQuality, light.Shadows.Bias, 1.0f);

			for (size_t i = 0; i < cascadeCount + 1; i++) {
				float nearPlane = 0.0f, FarPlane = 0.0f;
				if (i == 0) {
					nearPlane = settings.NearPlane;
					FarPlane = settings.CascadeSplits[i];
				}
				else if (i < cascadeCount) {
					nearPlane = settings.CascadeSplits[i - 1];
					FarPlane = settings.CascadeSplits[i];
				}
				// last cascade
				else {
					nearPlane = settings.CascadeSplits[i - 1];
					FarPlane = settings.FarPlane;
				}
				auto [lightProj, lightView, frustumSize] = GetLightSpaceMatrix(nearPlane, FarPlane, transform, cameraComp, lTC);

				m_sceneBufferData.cascadePlaneDistances[i] = glm::vec4(FarPlane, nearPlane, 0.0f, 0.0);
				m_sceneBufferData.lightSpaceMatrices[i] = lightProj * lightView;

				if (i == cascadeCount) {
					glm::vec2 size = frustumSize;
					// i dont now how to impl vfc for csm, but it isnt really needed due to the range
					// this here basicly only does a far check which is enough
					auto f = VFC::createFrustum_ortho(
						-size.x, size.x,
						-size.y, size.y,
						settings.NearPlane, settings.FarPlane * 2.0f,
						transform.GetWorldPosition() + - lTC.GetForward() * settings.FarPlane * 0.5f, lTC.GetWorldRotation()
					);
					VFC::setCamera(f);
					VFC::setEnabled(true);
				}
			}

			Renderer::getBuffer(BufferBinding::Scene)->setData(&m_sceneBufferData, sizeof(SceneBufferData));

			m_shadowMap->bind();
			RenderCommand::clear();
			drawSceneShadow();
			m_shadowMap->unbind();

			m_shadowMap->getDepthAttachment()->bind(ST_CASCADE_SHADOW_TEXTURE_BIND_MAP);
		}
		else {
			Resources::WhiteTexture()->bind(ST_CASCADE_SHADOW_TEXTURE_BIND_MAP);
		}
	}

	void SceneRenderer::Register3dObjects() {
		auto group = m_scene->GetAllWith<MeshFilterComponent, TransformComponent, MeshRendererComponent>();
		for (auto gameObject : group) {
			if (group.get<MeshFilterComponent>(gameObject).GetMesh().IsValid()) {
				RegisterObject(group.get<MeshRendererComponent>(gameObject), group.get<MeshFilterComponent>(gameObject), group.get<TransformComponent>(gameObject));
			}
		}
	}

	void SceneRenderer::drawAll2d(const TransformComponent& camera, bool callEvents) {
		ST_PROFILING_SCOPE("Renderer - 2D Pass");

		struct Entry {
			entt::entity id;
			enum Type {
				Quad, Circle
			}type;
		};
		const glm::vec3 camPos = camera.GetWorldPosition();
		auto quadview = m_scene->GetAllWith<TransformComponent, SpriteRendererComponent>();
		auto circleView = m_scene->GetAllWith<TransformComponent, CircleRendererComponent>();

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

		Renderer2D::begin();

		for (Entry& entry : drawList) {
			if (entry.type == Entry::Quad) {
				auto [transform, sprite] = quadview.get(entry.id);
				if (sprite.texture.IsLoaded())
					Renderer2D::drawTexturedQuad(transform.GetWorldTransform(), *sprite.texture, sprite.texture->getSettings().tiling * sprite.tiling, sprite.color, (uint32_t)entry.id);
				else
					Renderer2D::drawQuad(transform.GetWorldTransform(), sprite.color, (uint32_t)entry.id);
			}
			else {
				auto [transform, sprite] = circleView.get(entry.id);
				Renderer2D::drawCircle(transform.GetWorldTransform(), sprite.color, sprite.thickness, sprite.fade, (uint32_t)entry.id);
			}
		}

		if (callEvents && m_scene->getCaller()) {
			m_scene->getCaller()->onRender2D();
		}

		Renderer2D::flush();
	}

	void SceneRenderer::RegisterObject(MeshRendererComponent& meshRenderer, MeshFilterComponent& meshFilter, TransformComponent& transform) {
		if (!meshFilter.GetMesh().IsLoaded())
			return;
		if (meshRenderer.GetMaterial().IsValid() && meshRenderer.GetMaterial()->IsTransparent()) {
			m_transparentDrawList.push_back(RenderObject{
				&meshRenderer,
				&meshFilter,
				&transform
			});
		}
		else {
			m_drawList.push_back(RenderObject{
				&meshRenderer,
				&meshFilter,
				&transform
				});
		}
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

			// post process
			if (gameObject.hasComponent<PostProcessingComponent>()) {
				ApplyPostProcessing(cam.GetResultFrameBuffer(), &gameObject.getComponent<PostProcessingComponent>());
			}

			// blib
			cam.GetResultTexture()->bind(0);
			Renderer::ScreenQuad(sceneFbo, Resources::FullscreenShader());
		}
	}

	void SceneRenderer::ApplyPostProcessing(SceneCamera& sceneCam) {
		if (sceneCam.getPostProcessingUsingMain() && m_scene) {
			GameObject mainCam = m_scene->getMainCamera();
			if (mainCam && mainCam.hasComponent<PostProcessingComponent>()) {
				PostProcessingComponent* comp = &mainCam.getComponent<PostProcessingComponent>();
				ApplyPostProcessing(sceneCam.getCamera().getResultFrameBuffer(), comp);
				return;
			}
		}

		ApplyPostProcessing(sceneCam.getCamera().getResultFrameBuffer(), &sceneCam.getPostProcessingData());
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& frameBuffer, PostProcessingComponent* data) {
		ApplyPostProcessing(frameBuffer, frameBuffer->getColorAttachment().get(), data);
	}
	void SceneRenderer::ApplyPostProcessing(const Ref<FrameBuffer>& destination, const Texture2D* source, PostProcessingComponent* data) {
		ST_PROFILING_SCOPE("Renderer - PostProcessing");
		
		if (!data) {
			GammaCorrectionEffect effect;
			effect.Apply(destination, source);
			return;
		}

		for (const auto& effect : data->m_effects) {
			if (effect && effect->IsEnabled()) {
				effect->Apply(destination, source);
			}
		}
	}
}