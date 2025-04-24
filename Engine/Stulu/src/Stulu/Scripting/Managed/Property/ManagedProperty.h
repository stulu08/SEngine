#pragma once

#include "Stulu/Scripting/Managed/Mono.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Serialization/YAML.h"

#include <entt/entt.hpp>

namespace Stulu {
	class STULU_API MonoObjectInstance;
	enum class ManagedPropertyType : uint32_t {
		Other, 
		Vector4_t, Vector3_t, Vector2_t, 
		Float_t, Int_t, UInt_t, Bool_t, 
		AssetHandle, GameObject_t
	};
	class STULU_API ManagedProperty {
	public:
		ManagedProperty(Mono::Object object, Mono::ClassField field);
		virtual ~ManagedProperty() = default;

		virtual std::string getName() const;
		virtual std::string getTypeName() const;
		virtual Mono::Type getDataType() const;

		virtual Mono::ClassField getField() const { return m_fieldPtr; };
		virtual Mono::Object getParentObject() const { return m_parentObjectPtr; };

		virtual size_t getSize() const = 0;
		virtual ManagedPropertyType getType() const = 0;

		// allocates memory using malloc, the user is responsible for deleting the buffer using free
		virtual void* CopyValueToBuffer() const = 0;
		virtual void SetValueFromBuffer(void* source) = 0;
		// copy value to other
		virtual void CopyValueTo(Ref<ManagedProperty> other) const = 0;

		virtual void Serialize(YAML::Emitter& out) const = 0;
		virtual void Deserialize(YAML::detail::iterator_value& node) = 0;

		static Ref<ManagedProperty> Create(Mono::Object object, Mono::ClassField field);
	protected:
		Mono::ClassField m_fieldPtr = nullptr;
		Mono::Object m_parentObjectPtr = nullptr;
	};
	class STULU_API ManagedMiscProperty : public ManagedProperty {
	public:
		ManagedMiscProperty(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};
		virtual ~ManagedMiscProperty() = default;

		virtual void Serialize(YAML::Emitter& out) const override { };
		virtual void Deserialize(YAML::detail::iterator_value& node) override { };

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override { };
		virtual void* CopyValueToBuffer() const override { return nullptr; };
		virtual void SetValueFromBuffer(void* source) override { };

		virtual size_t getSize() const override { return 0; };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Other; };
	};
#pragma region Primitives
	class STULU_API Int32Property : public ManagedProperty {
	public:
		Int32Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual int32_t GetValue() const;
		virtual void SetValue(int32_t value);

		virtual size_t getSize() const override { return sizeof(int32_t); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Int_t; };
	};
	class STULU_API UInt32Property : public ManagedProperty {
	public:
		UInt32Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual uint32_t GetValue() const;
		virtual void SetValue(uint32_t value);

		virtual size_t getSize() const override { return sizeof(uint32_t); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::UInt_t; };
	};
	class STULU_API UInt64Property : public ManagedProperty {
	public:
		UInt64Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual uint64_t GetValue() const;
		virtual void SetValue(uint64_t value);

		virtual size_t getSize() const override { return sizeof(uint64_t); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::UInt_t; };
	};
	class STULU_API FloatProperty : public ManagedProperty {
	public:
		FloatProperty(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual float GetValue() const;
		virtual void SetValue(float value);

		virtual size_t getSize() const override { return sizeof(float); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Float_t; };
	};
	class STULU_API BoolProperty : public ManagedProperty {
	public:
		BoolProperty(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual bool GetValue() const;
		virtual void SetValue(bool value);

		virtual size_t getSize() const override { return sizeof(bool); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Bool_t; };
	};
#pragma endregion
#pragma region Vectors
	class STULU_API Vector2Property : public ManagedProperty {
	public:
		Vector2Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec2 GetValue() const;
		virtual void SetValue(const glm::vec2& value);

		virtual size_t getSize() const override { return sizeof(glm::vec2); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Vector2_t; };
	};
	class STULU_API Vector3Property : public ManagedProperty {
	public:
		Vector3Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec3 GetValue() const;
		virtual void SetValue(const glm::vec3& value);

		virtual size_t getSize() const override { return sizeof(glm::vec3); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Vector3_t; };
	};
	class STULU_API Vector4Property : public ManagedProperty {
	public:
		Vector4Property(Mono::Object object, Mono::ClassField field)
			: ManagedProperty(object, field) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual glm::vec4 GetValue() const;
		virtual void SetValue(const glm::vec4& value);

		virtual size_t getSize() const override { return sizeof(glm::vec4); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::Vector4_t; };
	};
#pragma endregion
#pragma region Assets
	class STULU_API AssetProperty : public ManagedProperty {
	public:
		AssetProperty(Mono::Object object, Mono::ClassField field);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual UUID GetValue() const;
		virtual void SetValue(const UUID& value);

		virtual size_t getSize() const override { return sizeof(UUID); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::AssetHandle; };
	protected:
		Ref<UInt64Property> m_idProperty = nullptr;;
		Ref<MonoObjectInstance> m_asset = nullptr;
	};
	class STULU_API Texture2DProperty : public AssetProperty {
	public:
		Texture2DProperty(Mono::Object object, Mono::ClassField field)
			: AssetProperty(object, field){}
	};
#pragma endregion
	class STULU_API GameObjectProperty : public ManagedProperty {
	public:
		GameObjectProperty(Mono::Object object, Mono::ClassField field);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<ManagedProperty> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual entt::entity GetValue() const;
		virtual void SetValue(entt::entity value);
		virtual uint64_t GetValueRaw() const;
		virtual void SetValueRaw(uint64_t value);

		virtual size_t getSize() const override { return sizeof(UUID); };
		virtual ManagedPropertyType getType() const override { return ManagedPropertyType::GameObject_t; };
	protected:
		Ref<UInt64Property> m_idProperty = nullptr;;
		Ref<MonoObjectInstance> m_gameObject = nullptr;
	};
}