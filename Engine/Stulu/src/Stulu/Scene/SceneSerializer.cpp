#include "st_pch.h"
#include "SceneSerializer.h"

#include "Stulu.h"


namespace Stulu {

	void SceneSerializer::SerializerGameObject(YAML::Emitter& out, GameObject gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << (uint64_t)gameObject.GetID();

		BEGIN_SERIALIZE_COMPONENT(GameObjectBaseComponent);
		{
			SERIALIZE_PROPERTY(SerializedGameObjectBaseComponent, tag);
			SERIALIZE_PROPERTY(SerializedGameObjectBaseComponent, name);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(TransformComponent);
		{
			SERIALIZE_PROPERTY(SerializedTransformComponent, position);
			SERIALIZE_PROPERTY(SerializedTransformComponent, rotation);
			SERIALIZE_PROPERTY(SerializedTransformComponent, scale);
			if(SerializedTransformComponent.HasParent())
				out << YAML::Key << "parent" << YAML::Value << (uint64_t)SerializedTransformComponent.GetParent().GetID();
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(ScriptingComponent);
		{
			out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
			for (Ref<MonoObjectInstance> instance : SerializedScriptingComponent.runtimeScripts) {
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << instance->getClass().GetName();
				out << YAML::Key << "Namespace" << YAML::Value << instance->getClass().GetNamespace();

				out << YAML::Key << "Fields" << YAML::Value << YAML::BeginSeq;
				for (auto& field : instance->getFields()) {
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << field->getName();
					out << YAML::Key << "Type" << (uint32_t)field->getType();
					field->Serialize(out);
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}
		END_SERIALIZE_COMPONENT();
		
		BEGIN_SERIALIZE_COMPONENT(CameraComponent);
		{
			SERIALIZE_ENUMPROP(SerializedCameraComponent, m_mode);
			SERIALIZE_ENUMPROP(SerializedCameraComponent, m_clearType);
			SERIALIZE_PROPERTY(SerializedCameraComponent, m_depth);
			SERIALIZE_PROPERTY(SerializedCameraComponent, m_fov);
			SERIALIZE_PROPERTY(SerializedCameraComponent, m_near);
			SERIALIZE_PROPERTY(SerializedCameraComponent, m_far);
			SERIALIZE_PROPERTY(SerializedCameraComponent, m_clearColor);
			SERIALIZE_UUIDPROP(SerializedCameraComponent, m_renderTarget);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(SpriteRendererComponent);
		{
			SERIALIZE_PROPERTY(SerializedSpriteRendererComponent, color);
			SERIALIZE_PROPERTY(SerializedSpriteRendererComponent, tiling);
			SERIALIZE_TEXTURE(SerializedSpriteRendererComponent, texture);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(CircleRendererComponent);
		{
			SERIALIZE_PROPERTY(SerializedCircleRendererComponent, color);
			SERIALIZE_PROPERTY(SerializedCircleRendererComponent, thickness);
			SERIALIZE_PROPERTY(SerializedCircleRendererComponent, fade);
		}
		END_SERIALIZE_COMPONENT();
		
		BEGIN_SERIALIZE_COMPONENT(LightComponent);
		{
			SERIALIZE_PROPERTY(SerializedLightComponent, color);
			SERIALIZE_ENUMPROP(SerializedLightComponent, lightType);
			SERIALIZE_PROPERTY(SerializedLightComponent, strength);
			SERIALIZE_PROPERTY(SerializedLightComponent, areaRadius);
			SERIALIZE_PROPERTY(SerializedLightComponent, spotLight_cutOff);
			SERIALIZE_PROPERTY(SerializedLightComponent, spotLight_outerCutOff);
		}
		END_SERIALIZE_COMPONENT();
		
		BEGIN_SERIALIZE_COMPONENT(MeshRendererComponent);
		{
			SERIALIZE_MATERIAL(SerializedMeshRendererComponent, material);
			SERIALIZE_ENUMPROP(SerializedMeshRendererComponent, cullmode);
		}
		END_SERIALIZE_COMPONENT();
		
		BEGIN_SERIALIZE_COMPONENT(MeshFilterComponent);
		{
			SERIALIZE_MESH(SerializedMeshFilterComponent, mesh);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(SkyBoxComponent);
		{
			SERIALIZE_PROPERTY(SerializedSkyBoxComponent, rotation);
			SERIALIZE_TEXTURE(SerializedSkyBoxComponent, texture);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(BoxColliderComponent);
		{
			SERIALIZE_PROPERTY(SerializedBoxColliderComponent, dynamicFriction);
			SERIALIZE_PROPERTY(SerializedBoxColliderComponent, staticFriction);
			SERIALIZE_PROPERTY(SerializedBoxColliderComponent, restitution);
			SERIALIZE_PROPERTY(SerializedBoxColliderComponent, size);
			SERIALIZE_PROPERTY(SerializedBoxColliderComponent, offset);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(SphereColliderComponent);
		{
			SERIALIZE_PROPERTY(SerializedSphereColliderComponent, dynamicFriction);
			SERIALIZE_PROPERTY(SerializedSphereColliderComponent, staticFriction);
			SERIALIZE_PROPERTY(SerializedSphereColliderComponent, restitution);
			SERIALIZE_PROPERTY(SerializedSphereColliderComponent, radius);
			SERIALIZE_PROPERTY(SerializedSphereColliderComponent, offset);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(CapsuleColliderComponent);
		{
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, dynamicFriction);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, staticFriction);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, restitution);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, radius);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, height);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, offset);
			SERIALIZE_PROPERTY(SerializedCapsuleColliderComponent, horizontal);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(MeshColliderComponent);
		{
			SERIALIZE_PROPERTY(SerializedMeshColliderComponent, dynamicFriction);
			SERIALIZE_PROPERTY(SerializedMeshColliderComponent, staticFriction);
			SERIALIZE_PROPERTY(SerializedMeshColliderComponent, restitution);
			SERIALIZE_PROPERTY(SerializedMeshColliderComponent, convex);
			SERIALIZE_MESH(SerializedMeshColliderComponent, mesh);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(RigidbodyComponent);
		{
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, useGravity);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, rotationX);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, rotationY);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, moveX);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, moveY);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, moveZ);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, kinematic);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, retainAccelaration);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, mass);
			SERIALIZE_PROPERTY(SerializedRigidbodyComponent, massCenterPos);
		}
		END_SERIALIZE_COMPONENT();

		BEGIN_SERIALIZE_COMPONENT(PostProcessingComponent);
		{
			const auto& settings = SerializedPostProcessingComponent.data.settings;

			const auto& gammaCorrection = SerializedPostProcessingComponent.data.settings.gammaCorrection;
			SERIALIZE_BEGINMAP("Gamma Correction");
				SERIALIZE_PROPERTY(gammaCorrection, enabled);
				SERIALIZE_PROPERTY(gammaCorrection, gamma);
				SERIALIZE_PROPERTY(gammaCorrection, exposure);
			SERIALIZE_ENDMAP();

			const auto& bloom = SerializedPostProcessingComponent.data.settings.bloom;
			SERIALIZE_BEGINMAP("Bloom");
				SERIALIZE_PROPERTY(bloom, enabled);
				SERIALIZE_PROPERTY(bloom, intensity);
				SERIALIZE_PROPERTY(bloom, threshold);
				SERIALIZE_PROPERTY(bloom, knee);
				SERIALIZE_PROPERTY(bloom, clamp);
				SERIALIZE_PROPERTY(bloom, resolutionScale);
				SERIALIZE_PROPERTY(bloom, diffusion);
				SERIALIZE_PROPERTY(bloom, maxSamples);
			SERIALIZE_ENDMAP();
		}
		END_SERIALIZE_COMPONENT();

		m_scene->getCaller()->SerializerGameObject(out, gameObject);

		out << YAML::EndMap;
	}

	void SceneSerializer::serialze(const std::string& path) {
		Scene* backupScene = StuluBindings::GetCurrentScene();
		StuluBindings::SetCurrentScene(m_scene.get());

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;

		SERIALIZE(m_scene->getData(), shaderFlags);
		SERIALIZE(m_scene->getData().graphicsData, env_lod);
		SERIALIZE(m_scene->getData().graphicsData, shadowDistance);
		SERIALIZE(m_scene->getData().graphicsData, shadowFar);
		SERIALIZE(m_scene->getData().graphicsData, shadowMapSize);
		SERIALIZE(m_scene->getData(), enablePhsyics3D);
		SERIALIZE(m_scene->getData(), physicsData.gravity);
		SERIALIZE(m_scene->getData(), physicsData.length);
		SERIALIZE(m_scene->getData(), physicsData.speed);
		SERIALIZE(m_scene->getData(), physicsData.workerThreads);

		out << YAML::EndMap;
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto [id, comp] : m_scene->getRegistry().storage<GameObjectBaseComponent>().each()) {
			GameObject go = { id, m_scene.get() };
			if (!go.IsValid())
				return;

			SerializerGameObject(out, go);
		}
		([&](entt::entity id) {
				
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);

		StuluBindings::SetCurrentScene(backupScene);
	}


	bool SceneSerializer::deSerialze(const std::string& path) {
		if (!FileExists(path.c_str()))
			return false;



		Scene* backupScene = StuluBindings::GetCurrentScene();
		StuluBindings::SetCurrentScene(m_scene.get());

		try {
			YAML::Node data = YAML::LoadFile(path);
			std::string SceneName = data["Scene"].as<std::string>();
			if (data["Settings"]) {
				YAML::Node settings = data["Settings"];

				DESERIALIZE(m_scene->getData(), shaderFlags, settings);

				DESERIALIZE(m_scene->getData().graphicsData, env_lod, settings);
				DESERIALIZE(m_scene->getData().graphicsData, shadowDistance, settings);
				DESERIALIZE(m_scene->getData().graphicsData, shadowFar, settings);
				DESERIALIZE(m_scene->getData().graphicsData, shadowMapSize, settings);
				
				DESERIALIZE(m_scene->getData(), enablePhsyics3D, settings);
				DESERIALIZE(m_scene->getData(), physicsData.gravity, settings);
				DESERIALIZE(m_scene->getData(), physicsData.length, settings);
				DESERIALIZE(m_scene->getData(), physicsData.speed, settings);
				DESERIALIZE(m_scene->getData(), physicsData.workerThreads, settings);
			}

			auto gos = data["GameObjects"];
			if (gos) {
				for (auto gameObject : gos) {
					GameObject deserialized = m_scene->createGameObject((entt::entity)gameObject["GameObject"].as<uint64_t>());
					
					BEGIN_DESERIALIZE_COMPONENT(GameObjectBaseComponent);
					{
						DESERIALIZE_PROPERTY(AddedGameObjectBaseComponent, name);
						DESERIALIZE_PROPERTY(AddedGameObjectBaseComponent, tag);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(TransformComponent);
					{
						DESERIALIZE_PROPERTY(AddedTransformComponent, position);
						DESERIALIZE_PROPERTY(AddedTransformComponent, rotation);
						DESERIALIZE_PROPERTY(AddedTransformComponent, scale);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(CameraComponent);
					{
						DESERIALIZE_ENUMPROP(AddedCameraComponent, m_mode);
						DESERIALIZE_ENUMPROP(AddedCameraComponent, m_clearType);
						DESERIALIZE_PROPERTY(AddedCameraComponent, m_depth);
						DESERIALIZE_PROPERTY(AddedCameraComponent, m_fov);
						DESERIALIZE_PROPERTY(AddedCameraComponent, m_near);
						DESERIALIZE_PROPERTY(AddedCameraComponent, m_far);
						DESERIALIZE_PROPERTY(AddedCameraComponent, m_clearColor);
						DESERIALIZE_UUIDPROP(AddedCameraComponent, m_renderTarget);
						
						AddedCameraComponent.UpdateCamera();
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(SpriteRendererComponent);
					{
						DESERIALIZE_PROPERTY(AddedSpriteRendererComponent, tiling);
						DESERIALIZE_PROPERTY(AddedSpriteRendererComponent, color);
						DESERIALIZE_TEXTURE(AddedSpriteRendererComponent, texture);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(PostProcessingComponent);
					{
						auto& settings = AddedPostProcessingComponent.data.settings;

						DESERIALIZE_BEGINMAP("Gamma Correction");
							auto& gammaCorrection = settings.gammaCorrection;
							DESERIALIZE_PROPERTY(gammaCorrection, enabled);
							DESERIALIZE_PROPERTY(gammaCorrection, gamma);
							DESERIALIZE_PROPERTY(gammaCorrection, exposure);
						DESERIALIZE_ENDMAP();

						DESERIALIZE_BEGINMAP("Bloom");
							auto& bloom = settings.bloom;
							DESERIALIZE_PROPERTY(bloom, enabled);
							DESERIALIZE_PROPERTY(bloom, intensity);
							DESERIALIZE_PROPERTY(bloom, threshold);
							DESERIALIZE_PROPERTY(bloom, knee);
							DESERIALIZE_PROPERTY(bloom, clamp);
							DESERIALIZE_PROPERTY(bloom, resolutionScale);
							DESERIALIZE_PROPERTY(bloom, diffusion);
							DESERIALIZE_PROPERTY(bloom, maxSamples);
						DESERIALIZE_ENDMAP();

					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(CircleRendererComponent);
					{
						DESERIALIZE_PROPERTY(AddedCircleRendererComponent, color);
						DESERIALIZE_PROPERTY(AddedCircleRendererComponent, thickness);
						DESERIALIZE_PROPERTY(AddedCircleRendererComponent, fade);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(LightComponent);
					{
						DESERIALIZE_PROPERTY(AddedLightComponent, color);
						DESERIALIZE_ENUMPROP(AddedLightComponent, lightType);
						DESERIALIZE_PROPERTY(AddedLightComponent, strength);
						DESERIALIZE_PROPERTY(AddedLightComponent, areaRadius);
						DESERIALIZE_PROPERTY(AddedLightComponent, spotLight_cutOff);
						DESERIALIZE_PROPERTY(AddedLightComponent, spotLight_outerCutOff);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(MeshRendererComponent);
					{
						DESERIALIZE_ENUMPROP(AddedMeshRendererComponent, cullmode);
						DESERIALIZE_MATERIAL(AddedMeshRendererComponent, material);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(MeshFilterComponent);
					{
						DESERIALIZE_MESH(AddedMeshFilterComponent, mesh);
					}
					END_DESERIALIZE_COMPONENT();
					
					BEGIN_DESERIALIZE_COMPONENT(SkyBoxComponent);
					{
						DESERIALIZE_SKYBOX(AddedSkyBoxComponent, texture);
						DESERIALIZE_PROPERTY(AddedSkyBoxComponent, rotation);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(BoxColliderComponent);
					{
						DESERIALIZE_PROPERTY(AddedBoxColliderComponent, dynamicFriction);
						DESERIALIZE_PROPERTY(AddedBoxColliderComponent, staticFriction);
						DESERIALIZE_PROPERTY(AddedBoxColliderComponent, restitution);
						DESERIALIZE_PROPERTY(AddedBoxColliderComponent, size);
						DESERIALIZE_PROPERTY(AddedBoxColliderComponent, offset);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(SphereColliderComponent);
					{
						DESERIALIZE_PROPERTY(AddedSphereColliderComponent, dynamicFriction);
						DESERIALIZE_PROPERTY(AddedSphereColliderComponent, staticFriction);
						DESERIALIZE_PROPERTY(AddedSphereColliderComponent, restitution);
						DESERIALIZE_PROPERTY(AddedSphereColliderComponent, radius);
						DESERIALIZE_PROPERTY(AddedSphereColliderComponent, offset);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(CapsuleColliderComponent);
					{
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, dynamicFriction);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, staticFriction);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, restitution);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, radius);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, height);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, offset);
						DESERIALIZE_PROPERTY(AddedCapsuleColliderComponent, horizontal);
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(MeshColliderComponent);
					{
						DESERIALIZE_PROPERTY(AddedMeshColliderComponent, dynamicFriction);
						DESERIALIZE_PROPERTY(AddedMeshColliderComponent, staticFriction);
						DESERIALIZE_PROPERTY(AddedMeshColliderComponent, restitution);
						DESERIALIZE_PROPERTY(AddedMeshColliderComponent, convex);

						if (componentNode["mesh"]) {
							UUID id = componentNode["mesh"].as<uint64_t>();
							if (AssetsManager::existsAndType(id, AssetType::Mesh)) {
								AddedMeshColliderComponent.mesh = std::any_cast<MeshAsset>(AssetsManager::get(id).data);

								if (AddedMeshColliderComponent.convex && AddedMeshColliderComponent.mesh.mesh->getVerticesCount() > 255 * 3) {
									AddedMeshColliderComponent.convexMesh = createRef<Mesh>(Mesh::copyAndLimit(AddedMeshColliderComponent.mesh.mesh, 255 * 3));
								}
								else {
									AddedMeshColliderComponent.convexMesh = AddedMeshColliderComponent.mesh.mesh;
								}
							}
						}
					}
					END_DESERIALIZE_COMPONENT();

					BEGIN_DESERIALIZE_COMPONENT(RigidbodyComponent);
					{
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, useGravity);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, rotationX);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, rotationY);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, rotationZ);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, moveX);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, moveY);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, moveZ);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, kinematic);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, retainAccelaration);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, mass);
						DESERIALIZE_PROPERTY(AddedRigidbodyComponent, massCenterPos);
					}
					END_DESERIALIZE_COMPONENT();

					m_scene->getCaller()->DeserializerGameObject(gameObject, deserialized, path);
				}
			}

			//handle childs and parents
			if (gos) {
				for (auto gameObject : gos) {
					GameObject deserialized = GameObject::GetById((entt::entity)gameObject["GameObject"].as<uint64_t>(), m_scene.get());

					auto transformComponentNode = gameObject["TransformComponent"];
					if (transformComponentNode) {
						if (transformComponentNode["parent"]) {
							deserialized.getComponent<TransformComponent>().SetParent(GameObject::GetById((entt::entity)transformComponentNode["parent"].as<uint64_t>(), m_scene.get()));
						}
					}

					BEGIN_DESERIALIZE_COMPONENT(ScriptingComponent);
					{
						auto manager = Application::get().getAssemblyManager();
						for (auto inst : componentNode["Scripts"]) {
							Mono::Class exists = Mono::Class::FromName(manager->getAppAssembly()->getImage(), inst["Namespace"].as<std::string>(), inst["Name"].as<std::string>());
							if (exists) {
								Ref<MonoObjectInstance> object = createRef<MonoObjectInstance>(exists, manager->getAppAssembly().get());
								for (YAML::detail::iterator_value field : inst["Fields"]) {
									std::string name = field["Name"].as<std::string>();
									PropertyType type = (PropertyType)field["Type"].as<uint32_t>();

									for (auto fieldItem : object->getFields()) {
										if (fieldItem->getName() != name || fieldItem->getType() != type)
											continue;
										fieldItem->Deserialize(field);

									}
								}
								AddedScriptingComponent.runtimeScripts.push_back(object);
							}
						}
					}
					END_DESERIALIZE_COMPONENT();

				}
			}
			return true;
		}
		catch (YAML::Exception ex) {
			StuluBindings::SetCurrentScene(backupScene);
			CORE_ERROR("{1}: YAML exception at {2}\n{0}", ex.what(), path, ex.mark.line);
			return false;
		}

		StuluBindings::SetCurrentScene(backupScene);
		return true;
	}
}