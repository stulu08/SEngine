#pragma once

#include "Stulu/Scripting/Managed/Mono.h"
#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/YAML.h"

#include <entt/entt.hpp>

namespace Stulu {
	class STULU_API MonoObjectInstance;
	enum class PropertyType : uint32_t {
		Other, 
		Vector4_t, Vector3_t, Vector2_t, 
		Float_t, Int_t, UInt_t, Bool_t, 
		AssetHandle, GameObject_t
	};
	class STULU_API Property {
	public:
		virtual ~Property() = default;

		virtual void Serialize(YAML::Emitter& out) const = 0;
		virtual void Deserialize(YAML::detail::iterator_value& node) = 0;

		// copy value to other
		virtual void CopyValueTo(Ref<Property> other) const = 0;

		// allocates memory using malloc, the user is responsible for deleting the buffer using free
		virtual void* CopyValueToBuffer() const = 0;
		virtual void SetValueFromBuffer(void* source) = 0;

		virtual size_t getSize() const = 0;
		virtual PropertyType getType() const = 0;
		virtual std::string getName() const = 0;
		virtual std::string getTypeName() const = 0;
		virtual Mono::Type getDataType() const = 0;
		virtual Mono::ClassField getField() const = 0;
		virtual Mono::Object getParentObject() const = 0;

		static Ref<Property> Create(Mono::Object object, Mono::ClassField field);
	};
	class STULU_API BaseProperty : public Property {
	public:
		BaseProperty(Mono::Object object, Mono::ClassField field);
		virtual ~BaseProperty() = default;

		virtual std::string getName() const override { return m_name; };
		virtual std::string getTypeName() const override { return m_typeName; };
		virtual Mono::Type getDataType() const override { return m_typePtr; };
		virtual Mono::ClassField getField() const override { return m_fieldPtr; };
		virtual Mono::Object getParentObject() const override { return m_parentObjectPtr; };
	protected:
		std::string m_name = "", m_typeName = "";
		Mono::Type m_typePtr = nullptr;
		Mono::ClassField m_fieldPtr = nullptr;
		Mono::Object m_parentObjectPtr = nullptr;
	};
	class STULU_API MiscProperty : public BaseProperty {
	public:
		MiscProperty(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};
		virtual ~MiscProperty() = default;

		virtual void Serialize(YAML::Emitter& out) const override { };
		virtual void Deserialize(YAML::detail::iterator_value& node) override { };

		// copy value to other
		virtual void CopyValueTo(Ref<Property> other) const override { };
		virtual void* CopyValueToBuffer() const override { return nullptr; };
		virtual void SetValueFromBuffer(void* source) override { };

		virtual size_t getSize() const override { return 0; };
		virtual PropertyType getType() const override { return PropertyType::Other; };
	};
#pragma region Primitives
	class STULU_API Int32Property : public BaseProperty {
	public:
		Int32Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual int32_t GetValue() const;
		virtual void SetValue(int32_t value);

		virtual size_t getSize() const override { return sizeof(int32_t); };
		virtual PropertyType getType() const override { return PropertyType::Int_t; };
	};
	class STULU_API UInt32Property : public BaseProperty {
	public:
		UInt32Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual uint32_t GetValue() const;
		virtual void SetValue(uint32_t value);

		virtual size_t getSize() const override { return sizeof(uint32_t); };
		virtual PropertyType getType() const override { return PropertyType::UInt_t; };
	};
	class STULU_API UInt64Property : public BaseProperty {
	public:
		UInt64Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual uint64_t GetValue() const;
		virtual void SetValue(uint64_t value);

		virtual size_t getSize() const override { return sizeof(uint64_t); };
		virtual PropertyType getType() const override { return PropertyType::UInt_t; };
	};
	class STULU_API FloatProperty : public BaseProperty {
	public:
		FloatProperty(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual float GetValue() const;
		virtual void SetValue(float value);

		virtual size_t getSize() const override { return sizeof(float); };
		virtual PropertyType getType() const override { return PropertyType::Float_t; };
	};
	class STULU_API BoolProperty : public BaseProperty {
	public:
		BoolProperty(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual bool GetValue() const;
		virtual void SetValue(bool value);

		virtual size_t getSize() const override { return sizeof(bool); };
		virtual PropertyType getType() const override { return PropertyType::Bool_t; };
	};
#pragma endregion
#pragma region Vectors
	class STULU_API Vector2Property : public BaseProperty {
	public:
		Vector2Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec2 GetValue() const;
		virtual void SetValue(const glm::vec2& value);

		virtual size_t getSize() const override { return sizeof(glm::vec2); };
		virtual PropertyType getType() const override { return PropertyType::Vector2_t; };
	};
	class STULU_API Vector3Property : public BaseProperty {
	public:
		Vector3Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec3 GetValue() const;
		virtual void SetValue(const glm::vec3& value);

		virtual size_t getSize() const override { return sizeof(glm::vec3); };
		virtual PropertyType getType() const override { return PropertyType::Vector3_t; };
	};
	class STULU_API Vector4Property : public BaseProperty {
	public:
		Vector4Property(Mono::Object object, Mono::ClassField field)
			: BaseProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec4 GetValue() const;
		virtual void SetValue(const glm::vec4& value);

		virtual size_t getSize() const override { return sizeof(glm::vec4); };
		virtual PropertyType getType() const override { return PropertyType::Vector4_t; };
	};
#pragma endregion
#pragma region Assets
	class STULU_API AssetProperty : public BaseProperty {
	public:
		AssetProperty(Mono::Object object, Mono::ClassField field);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual UUID GetValue() const;
		virtual void SetValue(const UUID& value);

		virtual size_t getSize() const override { return sizeof(UUID); };
		virtual PropertyType getType() const override { return PropertyType::AssetHandle; };
		virtual AssetType getAssetType() const = 0;
	protected:
		Ref<UInt64Property> m_idProperty = nullptr;;
		Ref<MonoObjectInstance> m_asset = nullptr;
	};
	class STULU_API Texture2DProperty : public AssetProperty {
	public:
		Texture2DProperty(Mono::Object object, Mono::ClassField field)
			: AssetProperty(object, field){}

		virtual AssetType getAssetType() const override { return AssetType::Texture2D; };

	};
#pragma endregion
	class STULU_API GameObjectProperty : public BaseProperty {
	public:
		GameObjectProperty(Mono::Object object, Mono::ClassField field);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual entt::entity GetValue() const;
		virtual void SetValue(entt::entity value);
		virtual uint64_t GetValueRaw() const;
		virtual void SetValueRaw(uint64_t value);

		virtual size_t getSize() const override { return sizeof(UUID); };
		virtual PropertyType getType() const override { return PropertyType::GameObject_t; };
	protected:
		Ref<UInt64Property> m_idProperty = nullptr;;
		Ref<MonoObjectInstance> m_gameObject = nullptr;
	};
}