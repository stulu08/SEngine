#include "st_pch.h"
#include "SceneSerializer.h"
#include "Stulu/Scene/YAML.h"

#include "Stulu/Scene/Components.h"
namespace Stulu {

	static void SerializerGameObject(YAML::Emitter& out, GameObject gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << gameObject.getId();
		if (gameObject.hasComponent<GameObjectBaseComponent>()) {
			out << YAML::Key << "GameObjectBaseComponent";
			out << YAML::BeginMap;
			std::string& tag = gameObject.getComponent<GameObjectBaseComponent>().tag;
			std::string& name = gameObject.getComponent<GameObjectBaseComponent>().name;
			out << YAML::Key << "name" << YAML::Value << name;
			out << YAML::Key << "tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto& trans = gameObject.getComponent<TransformComponent>();
			out << YAML::Key << "position" << YAML::Value << trans.position;
			out << YAML::Key << "rotation" << YAML::Value << trans.rotation;
			out << YAML::Key << "scale" << YAML::Value << trans.scale;
			if (gameObject.getComponent<TransformComponent>().parent) {
				out << YAML::Key << "parent" << YAML::Value << trans.parent.getId();
			}
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<CameraComponent>()) {
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			auto& camera = gameObject.getComponent<CameraComponent>();
			auto& settings = camera.settings;
			out << YAML::Key << "Camera Settings" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "clearType" << YAML::Value << (int)settings.clearType;
			out << YAML::Key << "clearColor" << YAML::Value << settings.clearColor;
			out << YAML::Key << "fov" << YAML::Value << settings.fov;
			out << YAML::Key << "zoom" << YAML::Value << settings.zoom;
			out << YAML::Key << "zNear" << YAML::Value << settings.zNear;
			out << YAML::Key << "zFar" << YAML::Value << settings.zFar;
			out << YAML::Key << "staticAspect" << YAML::Value << settings.staticAspect;
			out << YAML::Key << "aspectRatio" << YAML::Value << settings.aspectRatio;
			out << YAML::Key << "textureWidth" << YAML::Value << settings.textureWidth;
			out << YAML::Key << "textureHeight" << YAML::Value << settings.textureHeight;
			out << YAML::EndMap;
			out << YAML::Key << "mode" << YAML::Value << (int)camera.mode;
			out << YAML::Key << "depth" << YAML::Value << camera.depth;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<SpriteRendererComponent>()) {
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			auto& spriteRendererComponent = gameObject.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "color" << YAML::Value << spriteRendererComponent.color;
			out << YAML::Key << "tiling" << YAML::Value << spriteRendererComponent.tiling;
			if (spriteRendererComponent.texture)
				out << YAML::Key << "texture" << YAML::Value << (uint64_t)spriteRendererComponent.texture;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<LightComponent>()) {
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& light = gameObject.getComponent<LightComponent>();
			out << YAML::Key << "color" << YAML::Value << light.color;
			out << YAML::Key << "lightType" << YAML::Value << (int)light.lightType;
			out << YAML::Key << "strength" << YAML::Value << light.strength;
			out << YAML::Key << "spotLight_cutOff" << YAML::Value << light.spotLight_cutOff;
			out << YAML::Key << "spotLight_outerCutOff" << YAML::Value << light.spotLight_outerCutOff;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<MeshRendererComponent>()) {
			out << YAML::Key << "MeshRendererComponent";
			out << YAML::BeginMap;
			auto& meshren = gameObject.getComponent<MeshRendererComponent>();
			if(meshren.material)
				out << YAML::Key << "material" << YAML::Value << (uint64_t)meshren.material->getUUID();
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<MeshFilterComponent>()) {
			out << YAML::Key << "MeshFilterComponent";
			out << YAML::BeginMap;
			auto& meshren = gameObject.getComponent<MeshFilterComponent>();
			out << YAML::Key << "mesh" << YAML::Value << (uint64_t)meshren.mesh.uuid;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<SkyBoxComponent>()) {
			out << YAML::Key << "SkyBoxComponent";
			out << YAML::BeginMap;
			auto& meshren = gameObject.getComponent<SkyBoxComponent>();
			if(meshren.material)
				out << YAML::Key << "material" << YAML::Value << (uint64_t)meshren.material->getUUID();
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<BoxColliderComponent>()) {
			out << YAML::Key << "BoxColliderComponent";
			out << YAML::BeginMap;
			auto& component = gameObject.getComponent<BoxColliderComponent>();
			out << YAML::Key << "dynamicFriction" << YAML::Value << component.dynamicFriction;
			out << YAML::Key << "staticFriction" << YAML::Value << component.staticFriction;
			out << YAML::Key << "restitution" << YAML::Value << component.restitution;
			out << YAML::Key << "size" << YAML::Value << component.size;
			out << YAML::Key << "offset" << YAML::Value << component.offset;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<SphereColliderComponent>()) {
			out << YAML::Key << "SphereColliderComponent";
			out << YAML::BeginMap;
			auto& component = gameObject.getComponent<SphereColliderComponent>();
			out << YAML::Key << "dynamicFriction" << YAML::Value << component.dynamicFriction;
			out << YAML::Key << "staticFriction" << YAML::Value << component.staticFriction;
			out << YAML::Key << "restitution" << YAML::Value << component.restitution;
			out << YAML::Key << "radius" << YAML::Value << component.radius;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<MeshColliderComponent>()) {
			out << YAML::Key << "MeshColliderComponent";
			out << YAML::BeginMap;
			auto& component = gameObject.getComponent<MeshColliderComponent>();
			out << YAML::Key << "dynamicFriction" << YAML::Value << component.dynamicFriction;
			out << YAML::Key << "staticFriction" << YAML::Value << component.staticFriction;
			out << YAML::Key << "restitution" << YAML::Value << component.restitution;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<RigidbodyComponent>()) {
			out << YAML::Key << "RigidbodyComponent";
			out << YAML::BeginMap;
			auto& component = gameObject.getComponent<RigidbodyComponent>();
			out << YAML::Key << "rbType" << YAML::Value << (int)component.rbType;
			out << YAML::Key << "useGravity" << YAML::Value << component.useGravity;
			out << YAML::Key << "rotationX" << YAML::Value << component.rotationX;
			out << YAML::Key << "rotationY" << YAML::Value << component.rotationY;
			out << YAML::Key << "rotationZ" << YAML::Value << component.rotationZ;
			out << YAML::Key << "kinematic" << YAML::Value << component.kinematic;
			out << YAML::Key << "retainAccelaration" << YAML::Value << component.retainAccelaration;
			out << YAML::Key << "mass" << YAML::Value << component.mass;
			out << YAML::Key << "massCenterPos" << YAML::Value << component.massCenterPos;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialze(const std::string& path) {

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		m_scene->m_registry.each([&](entt::entity id) {
				GameObject go = { id, m_scene.get() };
				if (go == GameObject::null)
					return;
				
				SerializerGameObject(out, go);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		FILE* file = fopen(path.c_str(), "w");
		fprintf(file, out.c_str());
		fclose(file);

	}
	void SceneSerializer::deSerialze(const std::string& path) {
		/*FILE* file = fopen(path.c_str(), "rb");
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);
		char* fileContent = (char*)malloc(fileSize + 1);
		fread(fileContent, 1, fileSize, file);
		fclose(file);
		fileContent[fileSize] = 0;*/

		YAML::Node data = YAML::LoadFile(path);
		std::string name = data["Scene"].as<std::string>();

		auto gos = data["GameObjects"];
		if (gos) {
			for (auto gameObject : gos) {
				uint64_t uuid = gameObject["GameObject"].as<uint64_t>();

				std::string name;
				std::string tag;
				auto base = gameObject["GameObjectBaseComponent"];
				if (base) {
					tag = base["tag"].as<std::string>();
					name = base["name"].as<std::string>();

				}


				GameObject deserialized = m_scene->createGameObject(name, uuid);
				deserialized.getComponent<GameObjectBaseComponent>().tag = tag;

				auto transformComponentNode = gameObject["TransformComponent"];
				if (transformComponentNode) {
					auto& tc = deserialized.getComponent<TransformComponent>();
					tc.position = transformComponentNode["position"].as<glm::vec3>();
					tc.rotation = transformComponentNode["rotation"].as<glm::vec3>();
					tc.scale = transformComponentNode["scale"].as<glm::vec3>();
				}

				auto cameraComponentNode = gameObject["CameraComponent"];
				if (cameraComponentNode) {
					auto& cc = deserialized.addComponent<CameraComponent>();
					auto& settings = cameraComponentNode["Camera Settings"];

					cc.settings.clearType = (CameraComponent::ClearType)settings["clearType"].as<int>();
					cc.settings.clearColor = settings["clearColor"].as<glm::vec4>();
					cc.settings.fov = settings["fov"].as<float>();
					cc.settings.zoom = settings["zoom"].as<float>();
					cc.settings.zNear = settings["zNear"].as<float>();
					cc.settings.zFar = settings["zFar"].as<float>();
					cc.settings.staticAspect = settings["staticAspect"].as<bool>();
					cc.settings.aspectRatio = settings["aspectRatio"].as<float>();
					cc.settings.textureWidth = settings["textureWidth"].as<uint32_t>();
					cc.settings.textureHeight = settings["textureHeight"].as<uint32_t>();


					cc.mode = (CameraMode)cameraComponentNode["mode"].as<int>();
					cc.depth = (CameraMode)cameraComponentNode["depth"].as<int>();
					cc.updateMode();
					cc.updateProjection();
					cc.updateSize();
				}

				auto spriteRendererNode = gameObject["SpriteRendererComponent"];
				if (spriteRendererNode) {
					auto& src = deserialized.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererNode["color"].as<glm::vec4>();
					src.tiling = spriteRendererNode["tiling"].as<glm::vec2>();
					if(spriteRendererNode["texture"])
						src.texture = spriteRendererNode["texture"].as<uint64_t>();
				}

				auto lightComponentNode = gameObject["LightComponent"];
				if (lightComponentNode) {
					auto& light = deserialized.addComponent<LightComponent>();
					light.color = lightComponentNode["color"].as<glm::vec3>();
					light.lightType = (LightComponent::Type)lightComponentNode["lightType"].as<int>();
					light.strength = lightComponentNode["strength"].as<float>();
					light.spotLight_cutOff = lightComponentNode["spotLight_cutOff"].as<float>();
					light.spotLight_outerCutOff = lightComponentNode["spotLight_outerCutOff"].as<float>();
				}

				auto meshRendererComponentNode = gameObject["MeshRendererComponent"];
				if (meshRendererComponentNode) {
					auto& meshrenC = deserialized.saveAddComponent<MeshRendererComponent>();
					if (meshRendererComponentNode["material"]) {
						UUID id = meshRendererComponentNode["material"].as<uint64_t>();
						if (AssetsManager::existsAndType(id, AssetType::Material))
							deserialized.getComponent<MeshRendererComponent>().material = AssetsManager::get(id).data._Cast<Material>();
					}
				}

				auto meshFilterComponentNode = gameObject["MeshFilterComponent"];
				if (meshFilterComponentNode) {
					auto& meshFilter = deserialized.saveAddComponent<MeshFilterComponent>();
					if (meshFilterComponentNode["mesh"]) {
						UUID id = meshFilterComponentNode["mesh"].as<uint64_t>();
						if (AssetsManager::existsAndType(id, AssetType::Mesh))
							deserialized.getComponent<MeshFilterComponent>().mesh = std::any_cast<MeshAsset>(AssetsManager::get(id).data);
					}
				}

				auto skyBoxComponentNode = gameObject["SkyBoxComponent"];
				if (skyBoxComponentNode) {
					auto& skyBox = deserialized.addComponent<SkyBoxComponent>();
					if(skyBoxComponentNode["material"])
						skyBox.material = AssetsManager::get(UUID(skyBoxComponentNode["material"].as<uint64_t>())).data._Cast<Material>();
				}

				auto boxColliderComponentNode = gameObject["BoxColliderComponent"];
				if (boxColliderComponentNode) {
					auto& collider = deserialized.addComponent<BoxColliderComponent>();
					collider.dynamicFriction = boxColliderComponentNode["dynamicFriction"].as<float>();
					collider.staticFriction = boxColliderComponentNode["staticFriction"].as<float>();
					collider.restitution = boxColliderComponentNode["restitution"].as<float>();

					collider.size = boxColliderComponentNode["size"].as<glm::vec3>();
					collider.offset = boxColliderComponentNode["offset"].as<glm::vec3>();
				}

				auto sphereColliderComponentNode = gameObject["SphereColliderComponent"];
				if (sphereColliderComponentNode) {
					auto& collider = deserialized.addComponent<SphereColliderComponent>();
					collider.dynamicFriction = sphereColliderComponentNode["dynamicFriction"].as<float>();
					collider.staticFriction = sphereColliderComponentNode["staticFriction"].as<float>();
					collider.restitution = sphereColliderComponentNode["restitution"].as<float>();

					collider.radius = sphereColliderComponentNode["radius"].as<float>();
				}

				auto meshColliderComponentNode = gameObject["MeshColliderComponent"];
				if (meshColliderComponentNode) {
					auto& collider = deserialized.addComponent<MeshColliderComponent>();
					collider.dynamicFriction = meshColliderComponentNode["dynamicFriction"].as<float>();
					collider.staticFriction = meshColliderComponentNode["staticFriction"].as<float>();
					collider.restitution = meshColliderComponentNode["restitution"].as<float>();
				}

				auto rigidbodyComponentNode = gameObject["RigidbodyComponent"];
				if (rigidbodyComponentNode) {
					auto& body = deserialized.addComponent<RigidbodyComponent>();
					body.rbType = (RigidbodyComponent::Type)rigidbodyComponentNode["rbType"].as<int>();
					body.useGravity = rigidbodyComponentNode["useGravity"].as<bool>();
					body.rotationX = rigidbodyComponentNode["rotationX"].as<bool>();
					body.rotationY = rigidbodyComponentNode["rotationY"].as<bool>();
					body.rotationZ = rigidbodyComponentNode["rotationZ"].as<bool>();
					body.kinematic = rigidbodyComponentNode["kinematic"].as<bool>();
					body.retainAccelaration = rigidbodyComponentNode["retainAccelaration"].as<bool>();
					body.mass = rigidbodyComponentNode["mass"].as<float>();
					body.massCenterPos = rigidbodyComponentNode["massCenterPos"].as<glm::vec3>();
				}
			}
		}

		//handle childs and parents
		if (gos) {
			for (auto gameObject : gos) {
				UUID uuid = gameObject["GameObject"].as<uint64_t>();
				GameObject go = GameObject::getById(uuid, m_scene.get());

				auto transformComponentNode = gameObject["TransformComponent"];
				if (transformComponentNode) {
					if (transformComponentNode["parent"]) {
						go.getComponent<TransformComponent>().parent = GameObject::getById(UUID(transformComponentNode["parent"].as<uint64_t>()), m_scene.get());
					}
				}
				
			}
		}
	}
}