#include "st_pch.h"
#include "SceneSerializer.h"
#include "Stulu/Scene/YAML.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/ScriptCore/MonoObjectInstance.h"
#include "Stulu/ScriptCore/AssemblyManager.h"

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
		if (gameObject.hasComponent<ScriptingComponent>()) {
			out << YAML::Key << "ScriptingComponent";
			out << YAML::BeginMap << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;

			auto& comp = gameObject.getComponent<ScriptingComponent>();
			for (Ref<MonoObjectInstance> instance : comp.runtimeScripts) {
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << instance->getClassName();
				out << YAML::Key << "Namespace" << YAML::Value << instance->getNameSpace();

				out << YAML::Key << "Fields" << YAML::Value << YAML::BeginSeq;
				for (auto& [name,field] : instance->getFields()) {
					out << YAML::BeginMap;
					out << YAML::Key << "Name" << name;
					out << YAML::Key << "Type" << (int32_t)field.type;
					switch (field.type) {
					case MonoObjectInstance::MonoClassMember::Type::Vector4_t:
						out << YAML::Key << "Value" << *((glm::vec4*)field.value);
						break;
					case MonoObjectInstance::MonoClassMember::Type::Vector3_t:
						out << YAML::Key << "Value" << *((glm::vec3*)field.value);
						break;
					case MonoObjectInstance::MonoClassMember::Type::Vector2_t:
						out << YAML::Key << "Value" << *((glm::vec2*)field.value);
						break;
					case MonoObjectInstance::MonoClassMember::Type::float_t:
						out << YAML::Key << "Value" << *((float*)field.value);
						break;
					case MonoObjectInstance::MonoClassMember::Type::int_t:
						out << YAML::Key << "Value" << *((int32_t*)field.value);
						break;
					case MonoObjectInstance::MonoClassMember::Type::uint_t:
						out << YAML::Key << "Value" << *((uint32_t*)field.value);
						break;
					}
					out << YAML::EndMap;
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}

			out << YAML::EndSeq << YAML::EndMap;
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
				out << YAML::Key << "texture" << YAML::Value << (uint64_t)spriteRendererComponent.texture->uuid;
			out << YAML::EndMap;
		}
		if (gameObject.hasComponent<LightComponent>()) {
			out << YAML::Key << "LightComponent";
			out << YAML::BeginMap;
			auto& light = gameObject.getComponent<LightComponent>();
			out << YAML::Key << "color" << YAML::Value << light.color;
			out << YAML::Key << "lightType" << YAML::Value << (int)light.lightType;
			out << YAML::Key << "strength" << YAML::Value << light.strength;
			out << YAML::Key << "areaRadius" << YAML::Value << light.areaRadius;
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
			out << YAML::Key << "cullMode" << YAML::Key << (uint32_t)meshren.cullmode;
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
			if(meshren.texture)
				out << YAML::Key << "texture" << YAML::Value << (uint64_t)meshren.texture->uuid;
			out << YAML::Key << "mapType" << YAML::Value << (int32_t)meshren.mapType;
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
		out << YAML::Key << "Settings" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "gamma" << YAML::Value << m_scene->m_data.gamma;
		out << YAML::Key << "toneMappingExposure" << YAML::Value << m_scene->m_data.toneMappingExposure;
		out << YAML::Key << "env_lod" << YAML::Value << m_scene->m_data.env_lod;
		out << YAML::Key << "framebuffer16bit" << YAML::Value << m_scene->m_data.framebuffer16bit;
		out << YAML::Key << "enablePhsyics3D" << YAML::Value << m_scene->m_data.enablePhsyics3D;
		out << YAML::Key << "physicsData.gravity" << YAML::Value << m_scene->m_data.physicsData.gravity;
		out << YAML::Key << "physicsData.length" << YAML::Value << m_scene->m_data.physicsData.length;
		out << YAML::Key << "physicsData.speed" << YAML::Value << m_scene->m_data.physicsData.speed;
		out << YAML::Key << "physicsData.workerThreads" << YAML::Value << m_scene->m_data.physicsData.workerThreads;
		out << YAML::EndMap;
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
	bool SceneSerializer::deSerialze(const std::string& path) {
		try {
			YAML::Node data = YAML::LoadFile(path);
			std::string name = data["Scene"].as<std::string>();

			if (data["Settings"]) {
				YAML::Node settings = data["Settings"];

				if (settings["gamma"])
					m_scene->m_data.gamma = settings["gamma"].as<float>();
				if (settings["toneMappingExposure"])
					m_scene->m_data.toneMappingExposure = settings["toneMappingExposure"].as<float>();
				if (settings["framebuffer16bit"])
					m_scene->m_data.framebuffer16bit = settings["framebuffer16bit"].as<bool>();
				if (settings["env_lod"])
					m_scene->m_data.env_lod = settings["env_lod"].as<float>();
				if (settings["enablePhsyics3D"])
					m_scene->m_data.enablePhsyics3D = settings["enablePhsyics3D"].as<bool>();
				if (settings["physicsData.gravity"])
					m_scene->m_data.physicsData.gravity = settings["physicsData.gravity"].as<glm::vec3>();
				if (settings["physicsData.length"])
					m_scene->m_data.physicsData.length = settings["physicsData.length"].as<float>();
				if (settings["physicsData.speed"])
					m_scene->m_data.physicsData.speed = settings["physicsData.speed"].as<float>();
				if (settings["physicsData.workerThreads"])
					m_scene->m_data.physicsData.workerThreads = settings["physicsData.workerThreads"].as<uint32_t>();
			}

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
						tc.rotation = transformComponentNode["rotation"].as<glm::quat>();
						tc.scale = transformComponentNode["scale"].as<glm::vec3>();
					}
					auto scriptingComponent = gameObject["ScriptingComponent"];
					if (scriptingComponent) {
						for (auto inst : scriptingComponent["Scripts"]) {
							auto& comp = deserialized.saveAddComponent<ScriptingComponent>();
							MonoClass* exists = mono_class_from_name(Application::get().getAssemblyManager()->getAssembly()->getImage(), inst["Namespace"].as<std::string>().c_str(), inst["Name"].as<std::string>().c_str());
							if (exists) {
								Ref<MonoObjectInstance> object = createRef<MonoObjectInstance>(inst["Namespace"].as<std::string>(), inst["Name"].as<std::string>(), Application::get().getAssemblyManager()->getAssembly().get());
								object->loadAll();
								Application::get().getAssemblyManager()->getAssembly()->registerObject(object);
								for (auto field : inst["Fields"]) {
									std::string name = field["Name"].as<std::string>();
									auto& fieldList = object->getFields();
									if (fieldList.find(name) != fieldList.end()) {
										auto type = (MonoObjectInstance::MonoClassMember::Type)field["Type"].as<int32_t>();
										if (fieldList[name].type == type) {
											if (type == MonoObjectInstance::MonoClassMember::Type::float_t) {
												*((float*)fieldList[name].value) = field["Value"].as<float>();
											}
											else if (type == MonoObjectInstance::MonoClassMember::Type::int_t) {
												*((int32_t*)fieldList[name].value) = field["Value"].as<int32_t>();
											}
											else if (type == MonoObjectInstance::MonoClassMember::Type::uint_t) {
												*((uint32_t*)fieldList[name].value) = field["Value"].as<uint32_t>();
											}
											else if (type == MonoObjectInstance::MonoClassMember::Type::Vector2_t) {
												*((glm::vec2*)fieldList[name].value) = field["Value"].as<glm::vec2>();
											}
											else if (type == MonoObjectInstance::MonoClassMember::Type::Vector3_t) {
												*((glm::vec3*)fieldList[name].value) = field["Value"].as<glm::vec3>();
											}
											else if (type == MonoObjectInstance::MonoClassMember::Type::Vector4_t) {
												*((glm::vec4*)fieldList[name].value) = field["Value"].as<glm::vec4>();
											}
										}
									}
								}
								object->setAllClassFields();
								comp.runtimeScripts.push_back(object);
							}
						}
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
						if (spriteRendererNode["texture"])
							src.texture = std::any_cast<Ref<Texture2D>>(AssetsManager::get(UUID(spriteRendererNode["texture"].as<uint64_t>())).data);
					}

					auto lightComponentNode = gameObject["LightComponent"];
					if (lightComponentNode) {
						auto& light = deserialized.addComponent<LightComponent>();
						light.color = lightComponentNode["color"].as<glm::vec3>();
						light.lightType = (LightComponent::Type)lightComponentNode["lightType"].as<int>();
						light.strength = lightComponentNode["strength"].as<float>();
						light.areaRadius = lightComponentNode["areaRadius"].as<float>();
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
						if (meshRendererComponentNode["cullMode"]) {
							meshrenC.cullmode = (CullMode)meshRendererComponentNode["cullMode"].as<uint32_t>();
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
						if (skyBoxComponentNode["texture"])
							skyBox.texture = std::any_cast<Ref<CubeMap>>(AssetsManager::get(UUID(skyBoxComponentNode["texture"].as<uint64_t>())).data);
						if (skyBoxComponentNode["mapType"])
							skyBox.mapType = (SkyBoxComponent::MapType)skyBoxComponentNode["mapType"].as<int32_t>();
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
			return true;
		}
		catch (YAML::Exception ex) {
			CORE_ERROR("YAML exception:\n{0}", ex.what())
			return false;
		}
		return true;
	}
}