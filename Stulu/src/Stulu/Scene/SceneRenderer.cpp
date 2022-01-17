#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Scene/Resources.h"

namespace Stulu {
	SceneRenderer::RuntimeData SceneRenderer::s_runtimeData;
	void SceneRenderer::init(Scene* scene) {
		ST_PROFILING_FUNCTION();
		s_scene = scene;
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
		Renderer::beginScene(cam.getProjection(), glm::inverse(transform.getWorldSpaceTransform()), transform.position, transform.rotation);
		s_runtimeData.cam = cam.getNativeCamera();
		s_runtimeData.cam->bindFrameBuffer();
		RenderCommand::clear();

		if (cam.settings.clearType == CameraComponent::Skybox) {
			if (object.hasComponent<SkyBoxComponent>())
				drawSkyBox(object.getComponent<SkyBoxComponent>());
		}
	}
	void SceneRenderer::beginScene(const SceneCamera& cam) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		Renderer::beginScene(cam.getCamera()->getProjectionMatrix(), glm::inverse(cam.getTransform().getTransform()), cam.getTransform().position, cam.getTransform().rotation);
		s_runtimeData.cam = cam.getCamera();
		cam.getCamera()->bindFrameBuffer();
		RenderCommand::clear();
	}
	void SceneRenderer::endScene() {
		ST_PROFILING_FUNCTION();
		Renderer::endScene();
		s_lastMaterial = UUID::null;
		s_runtimeData.cam->unbindFrameBuffer();
	}
	void SceneRenderer::calculateLights() {
		ST_PROFILING_FUNCTION();
		s_runtimeData.lightData = RuntimeData::LightData();
		auto view = s_scene->m_registry.view<TransformComponent, LightComponent>();
		for (auto gameObject : view)
		{
			auto [transform, light] = view.get<TransformComponent, LightComponent>(gameObject);
			if (s_runtimeData.lightData.lightCount < MAXLIGHTS) {
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].colorAndStrength = glm::vec4(light.color, light.strength);
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].positionAndType = glm::vec4(transform.position, light.lightType);
				s_runtimeData.lightData.lights[s_runtimeData.lightData.lightCount].rotation = glm::vec4(transform.forwardDirection(), 1.0f);
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
		if(s_lastMaterial != material->getUUID())
			material->bind();
		Renderer::submit(filter.mesh.mesh->getVertexArray(), transform);
		s_lastMaterial = material->getUUID();
	}
	void SceneRenderer::drawSkyBox(SkyBoxComponent& skybox) {
		ST_PROFILING_FUNCTION();
		if (!skybox.texture) {
			return;
		}
		auto& mesh = Resources::getSkyBoxMesh();
		Stulu::RenderCommand::setDepthFunc(true);
		skybox.texture->bind(1);
		Resources::getSkyBoxShader()->bind();
		mesh->getVertexArray()->bind();
		RenderCommand::drawIndexed(mesh->getVertexArray(), 0);
		Stulu::RenderCommand::setDepthFunc(false);
	}
}