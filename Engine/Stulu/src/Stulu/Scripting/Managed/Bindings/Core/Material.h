#pragma once
#include "AssetWrapper.h"

namespace StuluBindings {
	class Material : public AssetWrapper<Stulu::MaterialAsset> {
	public:
		static inline MonoString* GetName(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				return Stulu::Mono::String::New(getCoreDomain(), asset->GetName());
			}
			return Stulu::Mono::String::New(getCoreDomain(), "Unnamed Material");
		}

		static inline bool HasProperty(uint64_t id, Stulu::Mono::String name) {
			if (auto asset = SaveGetAsset(id)) {
				return asset->HasProperity(name.ToUtf8());
			}
			return false;
		}
		static inline int32_t PropertyCount(uint64_t id) {
			if (auto asset = SaveGetAsset(id)) {
				return (int32_t)asset->GetProperities().size();
			}
			return 0;
		}
		static inline int32_t GetPropertyType(uint64_t id, int32_t index) {
			if (auto asset = SaveGetAsset(id)) {
				const auto& props = asset->GetProperities();
				if (index < props.size()) 
					return (int32_t)props.at(index)->GetType();
			}
			return (int32_t)Stulu::MaterialPropertyType::None;
		}
		static inline MonoString* GetPropertyName(uint64_t id, int32_t index) {
			using namespace Stulu;

			if (auto asset = SaveGetAsset(id)) {
				const auto& props = asset->GetProperities();
				if (index < props.size())
					return Mono::String::New(getCoreDomain(), props.at(index)->GetName());
			}
			return Mono::String::New(getCoreDomain(), "");
		}


		template<class T>
		static inline typename T::NativeType GetPropertyValue(uint64_t id, Stulu::Mono::String name) {
			if (auto asset = SaveGetAsset(id)) {
				return asset->GetProperityAs<T>(name.ToUtf8())->GetValue();
			}
			return T::NativeType();
		}
		template<class T>
		static inline void SetPropertyValue(uint64_t id, Stulu::Mono::String name, typename T::NativeType* val) {
			if (auto asset = SaveGetAsset(id))
				asset->SetAndApplyPropertiy<T>(name.ToUtf8(), *val);
		}

		template<class T>
		static inline uint64_t GetPropertyValueSampler(uint64_t id, Stulu::Mono::String name) {
			if (auto asset = SaveGetAsset(id)) {
				const auto& textureAsset = asset->GetProperityAs<T>(name.ToUtf8())->GetValue();
				if(textureAsset)
					return textureAsset.GetUUID();
			}
			return 0;
		}

		template<class T>
		static inline void SetPropertyValueSampler(uint64_t id, Stulu::Mono::String name, uint64_t* val) {
			if (auto asset = SaveGetAsset(id)) {
				const auto& textureAsset = GetManager().GetAsset<T::NativeType>((Stulu::UUID)*val);
				if (textureAsset)
					asset->SetAndApplyPropertiy<T>(name.ToUtf8(), textureAsset);
			}
		}
	};
}