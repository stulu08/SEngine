#pragma once

#include "Stulu/Scripting/Managed/Mono.h"
#include "Stulu/Resources/AssetsManager.h"
#include "Stulu/Serialization/YAML.h"

#include <entt/entt.hpp>

namespace Stulu {
	class STULU_API MonoObjectInstance;

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
	protected:
		Mono::Method m_assetCreateMethod = nullptr;
		Mono::ClassField m_idField = nullptr;
		Mono::ClassField m_gcField = nullptr;

		static inline constexpr uint32_t GC_NULL = 0;
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

		virtual size_t getSize() const override { return sizeof(UUID); };
	protected:
		Mono::Method m_initMethod = nullptr;
		Mono::ClassField m_idField = nullptr;
	};
}