#pragma once
namespace StuluBindings {
	class Material {
	public:
		static inline MonoString* GetName(uint64_t id) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (asset.IsValid() && !asset.IsMemoryLoaded())
				return Stulu::Mono::String::New(getCoreDomain(), std::filesystem::path(asset.Path()).stem().string());
			return Stulu::Mono::String::New(getCoreDomain(), "Unnamed Material");
		}

		static inline bool HasProperty(uint64_t id, Stulu::Mono::String name) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return false;
			return asset->HasProperity(name.ToUtf8());
		}
		static inline int32_t PropertyCount(uint64_t id) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return 0;
			return (int32_t)asset->GetProperities().size();
		}
		static inline int32_t GetPropertyType(uint64_t id, int32_t index) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return 0;
			const auto& props = asset->GetProperities();
			if (index >= props.size()) return (int32_t)MaterialPropertyType::None;
			return (int32_t)props.at(index)->GetType();
		}
		static inline MonoString* GetPropertyName(uint64_t id, int32_t index) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return 0;
			const auto& props = asset->GetProperities();
			if (index >= props.size()) return Mono::String::New(getCoreDomain(), "");
			return Mono::String::New(getCoreDomain(), props.at(index)->GetName());
		}


		template<class T>
		static inline typename T::NativeType GetPropertyValue(uint64_t id, Stulu::Mono::String name) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return T::NativeType();
			return asset->GetProperityAs<T>(name.ToUtf8())->GetValue();
		}
		template<class T>
		static inline void SetPropertyValue(uint64_t id, Stulu::Mono::String name, typename T::NativeType* val) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return;
			asset->SetAndApplyPropertiy<T>(name.ToUtf8(), *val);
		}

		template<class T>
		static inline uint64_t GetPropertyValueSampler(uint64_t id, Stulu::Mono::String name) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return 0;
			const auto& textureAsset = asset->GetProperityAs<T>(name.ToUtf8())->GetValue();
			if (!textureAsset.IsValid()) return 0;
			return textureAsset.GetUUID();
		}

		template<class T>
		static inline void SetPropertyValueSampler(uint64_t id, Stulu::Mono::String name, uint64_t* val) {
			using namespace Stulu;
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id);
			if (!asset.IsValid()) return;
			const auto& textureAsset = AssetsManager::GlobalInstance().GetAsset<T::NativeType>((Stulu::UUID)*val);
			if (!textureAsset.IsValid()) return;
			asset->SetAndApplyPropertiy<T>(name.ToUtf8(), textureAsset);
		}
	};
}