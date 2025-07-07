#pragma once
namespace StuluBindings {
	class MeshRendererComponent {
	public:
		static inline void SetMaterialInternal(uint64_t go, uint64_t material, int32_t index) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshRendererComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshRendererComponent>();
				component.material = Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::MaterialAsset>(material);
			}
		}

		static inline uint64_t GetMaterialInternal(uint64_t go, int32_t index) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshRendererComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshRendererComponent>();
				return component.material.GetUUID();
			}
			return Stulu::UUID::null;
		}

		static inline void SetCullModeInternal(uint64_t go, uint32_t mode) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshRendererComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshRendererComponent>();
				component.cullmode = (Stulu::CullMode)mode;
			}
		}

		static inline uint32_t GetCullModeInternal(uint64_t go) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshRendererComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshRendererComponent>();
				return (uint32_t)component.cullmode;
			}
			return -1;
		}
		
	};

	class MeshFilterComponent {
	public:
		static inline void SetMeshInternal(uint64_t go, uint64_t meshID) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshFilterComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshFilterComponent>();
				component.SetMesh(Stulu::AssetsManager::GlobalInstance().GetAsset<Stulu::MeshAsset>(meshID));
			}
		}

		static inline uint64_t GetMeshInternal(uint64_t go) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if (gameObject.hasComponent<Stulu::MeshFilterComponent>()) {
				auto& component = gameObject.getComponent<Stulu::MeshFilterComponent>();
				return component.GetMesh().GetUUID();
			}
			return Stulu::UUID::null;
		}

	};
}