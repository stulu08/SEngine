#include "st_pch.h"
#include "SceneRenderer.h"

#include "Stulu/Scene/Resources.h"

namespace Stulu {
	SceneRenderer::Data SceneRenderer::m_runtimeData;

	void SceneRenderer::init() {
		ST_PROFILING_FUNCTION();
#if OPENGL
		AssetsManager::update(UUID(12), { AssetType::Material,Material(AssetsManager::get(UUID(9)),
			MaterialData(std::vector<MaterialDataType>{
			MaterialDataType{ShaderDataType::Float,.0f,"metallic"},
			MaterialDataType{ShaderDataType::Float3,glm::vec3(.9f),"albedo"},
			MaterialDataType{ShaderDataType::Float,.0f,"roughness"},
			MaterialDataType{ShaderDataType::Float,.3f,"ao"},})),"",UUID(12) });
		m_runtimeData.defaultMaterial = UUID(12);
		if(m_runtimeData.sceneDataUniformBuffer == nullptr)
			m_runtimeData.sceneDataUniformBuffer = UniformBuffer::create(sizeof(Data::SceneRuntimeData), 0);
#endif
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
		Renderer2D::beginScene();

		m_runtimeData.sceneData.viewMatrix = glm::inverse(transform.getWorldSpaceTransform());
		m_runtimeData.sceneData.projMatrix = cam.cam->getProjectionMatrix();
		m_runtimeData.sceneData.viewProjectionMatrix = m_runtimeData.sceneData.projMatrix * m_runtimeData.sceneData.viewMatrix;
		glm::vec3 pos, rot, scale;
		Math::decomposeTransform(transform, pos, rot, scale);
		m_runtimeData.sceneData.cameraPosition = pos;
		m_runtimeData.sceneData.cameraRotation = rot;
		//may use this to upload the material in a ubo
		/*
		size_t size = sizeof(glm::mat4) * 3 + sizeof(glm::vec3) * 2;
		size_t offset = 0;
		void* data = malloc(size);

		char* p = (char*)data;

		memcpy(p + offset, &m_runtimeData.sceneData.viewProjectionMatrix, sizeof(glm::mat4));
		offset += sizeof(glm::mat4);

		memcpy(p + offset, &m_runtimeData.sceneData.viewMatrix, sizeof(glm::mat4));
		offset += sizeof(glm::mat4);

		memcpy(p + offset, &m_runtimeData.sceneData.projMatrix, sizeof(glm::mat4));
		offset += sizeof(glm::mat4);

		memcpy(p + offset, &m_runtimeData.sceneData.cameraPosition, sizeof(glm::vec3));
		offset += sizeof(glm::vec3);

		memcpy(p + offset, &m_runtimeData.sceneData.cameraRotation, sizeof(glm::vec3));
		offset += sizeof(glm::vec3);
		m_runtimeData.sceneDataUniformBuffer->setData(data, size);
		free(data);
		*/ 
		m_runtimeData.sceneDataUniformBuffer->setData(&m_runtimeData.sceneData, sizeof(Data::SceneRuntimeData));
		m_runtimeData.cam = cam.cam;
		cam.cam->bindFrameBuffer();
		RenderCommand::clear();

		if (cam.settings.clearType == CameraComponent::Skybox) {
			if(object.hasComponent<SkyBoxComponent>())
				drawSkyBox(object.getComponent<SkyBoxComponent>());
		}
	}
	void SceneRenderer::beginScene(const SceneCamera& cam) {
		ST_PROFILING_FUNCTION();
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f), 1.0f));
		Renderer2D::beginScene();

		m_runtimeData.sceneData.viewMatrix = glm::inverse(cam.getTransform().getTransform());
		m_runtimeData.sceneData.projMatrix = cam.getCamera()->getProjectionMatrix();
		m_runtimeData.sceneData.viewProjectionMatrix = m_runtimeData.sceneData.projMatrix * m_runtimeData.sceneData.viewMatrix;
		m_runtimeData.sceneData.cameraPosition = cam.getTransform().position;
		m_runtimeData.sceneData.cameraRotation = cam.getTransform().rotation;
		m_runtimeData.sceneDataUniformBuffer->setData(&m_runtimeData.sceneData, sizeof(Data::SceneRuntimeData));
		m_runtimeData.cam = cam.getCamera();
		cam.getCamera()->bindFrameBuffer();
		RenderCommand::clear();
	}
	void SceneRenderer::endScene() {
		ST_PROFILING_FUNCTION();
		Renderer2D::endScene();
		m_runtimeData.cam->unbindFrameBuffer();
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
			material = AssetsManager::get(m_runtimeData.defaultMaterial).data._Cast<Material>();
		}
		material->bind();
		material->getShader()->setMat4("u_transform", transform);
		filter.mesh.mesh->getVertexArray()->bind();
		RenderCommand::drawIndexed(filter.mesh.mesh->getVertexArray(),0);

		if (mesh.material) {
			material->unbind();
		}
	}
	void SceneRenderer::drawSkyBox(SkyBoxComponent& skybox) {
		ST_PROFILING_FUNCTION();
		if (!skybox.material) {
			return;
		}

		Material* material = skybox.material;

		auto& mesh = Resources::getSkyBoxMesh();
		Stulu::RenderCommand::setDepthFunc(true);

		material->bind();
		for (auto& dataType : material->getData().dataTypes) {
			if (dataType.type == ShaderDataType::Sampler) {
				Asset& asset = AssetsManager::get(std::any_cast<UUID>(dataType.data));

				switch (asset.type)
				{
				case Stulu::AssetType::CubeMap:
					asset.data._Cast<Ref<CubeMap>>()->get()->bind(1);
					break;
				default:
					break;
				}
				break;
			}
		}


		mesh->getVertexArray()->bind();
		RenderCommand::drawIndexed(mesh->getVertexArray(), 0);

		material->unbind();
		Stulu::RenderCommand::setDepthFunc(false);
	}
}