#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Scene/Resources.h"

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
		Renderer::beginScene(cam.getProjection(), glm::inverse(transform.transform), transform.worldPosition, Math::QuaternionToEuler(transform.rotation));
		s_runtimeData.cam = cam.getNativeCamera();
		s_runtimeData.camPos = transform.worldPosition;
		if (cam.settings.clearType == CameraComponent::Skybox && object.hasComponent<SkyBoxComponent>()) {
			auto& s = object.getComponent<SkyBoxComponent>();
			s_runtimeData.camSkyBox.texture = s.texture;
			s_runtimeData.camSkyBox.mapType = (uint32_t)s.mapType;
		}

	}
	void SceneRenderer::beginScene(const SceneCamera& cam, GameObject mainCam) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		Renderer::beginScene(cam.getCamera()->getProjectionMatrix(), glm::inverse(cam.getTransform().transform), cam.getTransform().worldPosition, Math::QuaternionToEuler(cam.getTransform().rotation));
		s_runtimeData.cam = cam.getCamera();
		s_runtimeData.camPos = cam.getTransform().worldPosition;
		if (mainCam) {
			CameraComponent mcam = mainCam.getComponent<CameraComponent>();
			if (mcam.settings.clearType == CameraComponent::Skybox && mainCam.hasComponent<SkyBoxComponent>()) {
				auto& s = mainCam.getComponent<SkyBoxComponent>();
				s_runtimeData.camSkyBox.texture = s.texture;
				s_runtimeData.camSkyBox.mapType = (uint32_t)s.mapType;
			}
		}
	}
	void SceneRenderer::endScene() {
		ST_PROFILING_FUNCTION();
		s_runtimeData.cam->bindFrameBuffer();
		drawScene();
		Renderer::endScene();
		s_runtimeData.cam->unbindFrameBuffer();
		s_runtimeData.camSkyBox.texture = nullptr;
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

		s_runtimeData.sceneDataBuffer->setData(&s_runtimeData.bufferData, sizeof(RuntimeData::SceneData));
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
				glm::distance(s_runtimeData.camPos,glm::vec3(transform.position)) });
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
	void SceneRenderer::drawScene() {
		ST_PROFILING_FUNCTION();
		RenderCommand::setDefault();
		RenderCommand::clear();
		drawSkyBox();
		flush();
	}
	void SceneRenderer::flush() {
		ST_PROFILING_FUNCTION();
		UUID last = UUID();
		
		//opaque
		for (const RenderObject& object : s_drawList) {
			if (last != object.material->getUUID())
				object.material->bind();
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}
		s_drawList.clear();
		
		
		//transparent
		std::sort(s_transparentDrawList.begin(), s_transparentDrawList.end(), [](auto& a, auto& b) {
			return a.camDistance > b.camDistance;//sort the vector backwards
		});

		for (const auto& object : s_transparentDrawList) {
			if (last != object.material->getUUID())
				object.material->bind();
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}
		s_transparentDrawList.clear();
		
		//stencil
		RenderCommand::setStencil(StencilMode::WriteToBuffer);
		for (const RenderObject& object : s_stencilDrawList) {
			if (last != object.material->getUUID())
				object.material->bind();
			RenderCommand::setCullMode(object.cullmode);
			Renderer::submit(object.vertexArray, object.transform);
		}
		RenderCommand::setStencil(StencilMode::DisableWriting);
		s_stencilDrawList.clear();
		
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