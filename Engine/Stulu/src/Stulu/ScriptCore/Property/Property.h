#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-debug.h>

#include "Stulu/Math/Math.h"
#include "Stulu/Scene/AssetsManager.h"
#pragma region YAML
#ifndef DLL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define DLL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#undef YAML_CPP_API
#ifdef YAML_CPP_DLL				 
#ifdef yaml_cpp_EXPORTS  					 
#define YAML_CPP_API __declspec(dllexport)
#else
#define YAML_CPP_API __declspec(dllimport)
#endif
#else
#define YAML_CPP_API
#endif
#endif
namespace YAML {
	class YAML_CPP_API Emitter;
	namespace detail {
		struct iterator_value;
	}
}
#pragma endregion

namespace Stulu {
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
		virtual MonoType* getTypePtr() const = 0;
		virtual MonoClassField* getFieldPtr() const = 0;
		virtual MonoObject* getObjectPtr() const = 0;

		static Ref<Property> Create(MonoObject* object, MonoClassField* field);
		static inline std::unordered_map<std::string, std::function<Ref<Property>(MonoObject* object, MonoClassField* field)>> TypeRegister;
	};
	class STULU_API BaseProperty : public Property {
	public:
		BaseProperty(MonoObject* object, MonoClassField* field);
		virtual ~BaseProperty() = default;

		virtual std::string getName() const override { return m_name; };
		virtual std::string getTypeName() const override { return m_typeName; };
		virtual MonoType* getTypePtr() const override { return m_typePtr; };
		virtual MonoClassField* getFieldPtr() const override { return m_fieldPtr; };
		virtual MonoObject* getObjectPtr() const override { return m_parentObjectPtr; };
	protected:
		std::string m_name = "", m_typeName = "";
		MonoType* m_typePtr = nullptr;
		MonoClassField* m_fieldPtr = nullptr;
		MonoObject* m_parentObjectPtr = nullptr;
	};
	class STULU_API MiscProperty : public BaseProperty {
	public:
		MiscProperty(MonoObject* object, MonoClassField* field)
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
		Int32Property(MonoObject* object, MonoClassField* field)
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
		UInt32Property(MonoObject* object, MonoClassField* field)
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
	class STULU_API FloatProperty : public BaseProperty {
	public:
		FloatProperty(MonoObject* object, MonoClassField* field)
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
		BoolProperty(MonoObject* object, MonoClassField* field)
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
		Vector2Property(MonoObject* object, MonoClassField* field)
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
		Vector3Property(MonoObject* object, MonoClassField* field)
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
		Vector4Property(MonoObject* object, MonoClassField* field)
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
		AssetProperty(MonoObject* object, MonoClassField* field);

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
		MonoClassField* m_uuidFieldPtr = nullptr;
		MonoObject* m_assetObjectPtr = nullptr;
	};
	class STULU_API Texture2DProperty : public AssetProperty {
	public:
		Texture2DProperty(MonoObject* object, MonoClassField* field)
			: AssetProperty(object, field){}

		virtual AssetType getAssetType() const override { return AssetType::Texture2D; };

	};
#pragma endregion
	class STULU_API GameObjectProperty : public BaseProperty {
	public:
		GameObjectProperty(MonoObject* object, MonoClassField* field);

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::detail::iterator_value& node) override;

		virtual void CopyValueTo(Ref<Property> other) const override;
		virtual void* CopyValueToBuffer() const override;
		virtual void SetValueFromBuffer(void* source) override;
		virtual UUID GetValue() const;
		virtual void SetValue(const UUID& value);
		virtual uint32_t GetValueRaw() const;
		virtual void SetValueRaw(uint32_t value);

		virtual size_t getSize() const override { return sizeof(UUID); };
		virtual PropertyType getType() const override { return PropertyType::GameObject_t; };
	protected:
		MonoClassField* m_idFieldPtr = nullptr;
		MonoObject* m_assetObjectPtr = nullptr;
	};
}