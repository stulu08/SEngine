#pragma once
#include "Stulu/Renderer/Material/TestMaterial.h"
#include "Stulu/Resources/Resources.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Serialization/YAML.h"
#include "Stulu/Resources/Assets/TextureAssets.h"
#include <string>

namespace Stulu {
	enum class MaterialPropertyType : uint16_t {
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		UInt,
		Color,
		Sampler2D, 
		SkyBox,
		None
	};
	enum class DefaultMaterialTexture {
		Black,
		White
	};
	// helper to compute std140 base alignment
	inline size_t Std140BaseAlignment(size_t componentCount) {
		switch (componentCount) {
		case 0: return 0;
		case 1: return 4;   // float
		case 2: return 8;   // vec2
		default:            // vec3 or vec4
			return 16;
		}
	}

	// helper to compute the size taken in std140
	inline size_t Std140Size(size_t componentCount) {
		switch (componentCount) {
		case 0: return 0;
		case 1: return 4;
		case 2: return 8;
		default: // 3 or 4
			return 16;
		}
	}

	// align 'offset' up to 'alignment'
	inline size_t Std140Align(size_t offset, size_t alignment) {
		return (offset + alignment - 1) & ~(alignment - 1);
	}

	class STULU_API MaterialProperty {
	public:
		MaterialProperty(const std::string& name, size_t offset)
			: m_name(name), m_offset(offset) {};
		virtual ~MaterialProperty() = default;

		virtual const std::string& GetName() const { return m_name; };
		virtual size_t GetOffset() const { return m_offset; }

		virtual void ApplyValue(TestMaterial* material) const = 0;

		virtual size_t GetSize() const = 0;
		virtual MaterialPropertyType GetType() const = 0;

		virtual void Serializ(YAML::Emitter& out) const = 0;
		virtual void Deserializer(YAML::Node& node) = 0;

		static Ref<MaterialProperty> Construct(MaterialPropertyType type, const std::string& args, size_t expectedOffset);
		static MaterialPropertyType TypeFromString(std::string type);
		static Texture* GetDefaultTexture(DefaultMaterialTexture defaultTexture);
		static size_t GetComponentCount(MaterialPropertyType);

		static inline bool IsTextureType(MaterialPropertyType type){
			return type == MaterialPropertyType::Sampler2D || type == MaterialPropertyType::SkyBox;
		}
	protected:
		std::string m_name;
		size_t m_offset;
	};

	template<class T, MaterialPropertyType Type>
	class InternalPrimitivMaterialProperty : public MaterialProperty {
	public:
		static constexpr MaterialPropertyType PropertyType = Type;

		InternalPrimitivMaterialProperty(const T& val, const std::string& name, size_t offset)
			: MaterialProperty(name, offset), m_value(val) {}

		virtual size_t GetSize() const override { return sizeof(T); }
		virtual MaterialPropertyType GetType() const { return PropertyType; }

		virtual void ApplyValue(TestMaterial* material) const override {
			material->SetData(GetOffset(), GetSize(), &m_value);
		}

		virtual void Serializ(YAML::Emitter& out) const {
			out << YAML::Key << m_name << YAML::Value << m_value;
		};
		virtual void Deserializer(YAML::Node& node) {
			if (node[m_name])
				m_value = node[m_name].as<T>();
		};

		const T& GetValue() const { return m_value; }
		void SetValue(const T& val) { m_value = val; }
	private:
		T m_value;
	};

	template<class T, MaterialPropertyType Type>
	class InternalSamplerMaterialProperty : public MaterialProperty {
	public:
		using TextureType = TextureAsset<T>;
		static constexpr MaterialPropertyType PropertyType = Type;

		InternalSamplerMaterialProperty(const TextureType& texture, const std::string& name, size_t slot, DefaultMaterialTexture default = DefaultMaterialTexture::Black)
			: MaterialProperty(name, slot), m_value(texture), m_defaultTexture(default) {}

		virtual size_t GetSlot() const { return GetOffset(); }
		virtual size_t GetSize() const override { return 0; }
		virtual MaterialPropertyType GetType() const { return PropertyType; }

		virtual void ApplyValue(TestMaterial* material) const override {
			if (m_value.IsValid()) {
				material->SetSampler((uint32_t)GetOffset(), static_cast<Texture*>(*GetValue()));
			}
			else {
				material->SetSampler((uint32_t)GetOffset(), GetDefaultTexture(m_defaultTexture));
			}
		}

		virtual void Serializ(YAML::Emitter& out) const {
			out << YAML::Key << m_name << YAML::Value << YAML::BeginMap;
			if(m_value.IsValid())
				out << YAML::Key << "UUID" << YAML::Value << m_value.GetUUID();
			out << YAML::Key << "Default" << YAML::Value << (uint32_t)m_defaultTexture;
			out << YAML::EndMap;
		};
		virtual void Deserializer(YAML::Node& node) {
			UUID uuid;
			if (node[m_name]) {
				auto textureNode = node[m_name];
				if(node["Default"])
					m_defaultTexture = (DefaultMaterialTexture)node["Default"].as<uint32_t>();
				if (node["UUID"]) {
					m_value = AssetsManager::GlobalInstance().GetAsset<TextureType>(node["UUID"].as<UUID>());
				}

			}
		};

		const TextureType& GetValue() const { return m_value; }
		void SetValue(const TextureType& val) { m_value = val; }
	private:
		TextureType m_value;
		DefaultMaterialTexture m_defaultTexture;
	};

	using MaterialSampler2DProperty = InternalSamplerMaterialProperty<Texture2D, MaterialPropertyType::Sampler2D>;
	using MaterialSamplerSkyBoxProperty = InternalSamplerMaterialProperty<SkyBox, MaterialPropertyType::SkyBox>;

	using SimpleMaterialFloatProperty = InternalPrimitivMaterialProperty<float_t, MaterialPropertyType::Float>;
	using SimpleMaterialColorProperty = InternalPrimitivMaterialProperty<glm::vec4, MaterialPropertyType::Color>;

	using MaterialFloat2Property = InternalPrimitivMaterialProperty<glm::vec2, MaterialPropertyType::Float2>;
	using MaterialFloat3Property = InternalPrimitivMaterialProperty<glm::vec3, MaterialPropertyType::Float3>;
	using MaterialFloat4Property = InternalPrimitivMaterialProperty<glm::vec4, MaterialPropertyType::Float4>;
	using MaterialUIntProperty = InternalPrimitivMaterialProperty<uint32_t, MaterialPropertyType::UInt>;
	using MaterialIntProperty = InternalPrimitivMaterialProperty<int32_t, MaterialPropertyType::Int>;
	using MaterialInt2Property = InternalPrimitivMaterialProperty<glm::i32vec2, MaterialPropertyType::Int2>;
	using MaterialInt3Property = InternalPrimitivMaterialProperty<glm::i32vec3, MaterialPropertyType::Int3>;
	using MaterialInt4Property = InternalPrimitivMaterialProperty<glm::i32vec4, MaterialPropertyType::Int4>;

	class MaterialFloatProperty : public SimpleMaterialFloatProperty {
	public:
		MaterialFloatProperty(float val, const std::string& name, size_t offset, float min = 0.0f, float max = 0.0f)
			: SimpleMaterialFloatProperty(val, name, offset), m_min(min), m_max(max) {}

		bool HasLimits() const { return m_min != m_max; }
		float GetMin() const { return m_min; }
		float GetMax() const { return m_max; }
	private:
		float m_min, m_max;
	};

	class MaterialColorProperty : public SimpleMaterialColorProperty {
	public:
		MaterialColorProperty(const glm::vec4& val, const std::string& name, size_t offset, bool isHDR = false)
			: SimpleMaterialColorProperty(val, name, offset), m_hdr(isHDR){}

		bool IsHDR() const { return m_hdr; }
	private:
		bool m_hdr;
	};
	template<class T, class V>
	inline bool TestMaterial::SetAndApplyPropertiy(const std::string& name, const V& value) {
		if(!HasProperity(name)) return false;
		
		Ref<MaterialProperty> property = GetProperity(name);
		if (property->GetType() != T::PropertyType) return false;

		std::dynamic_pointer_cast<T>(property)->SetValue(value);
		property->ApplyValue(this);
		return true;
	}
}