#include "st_pch.h"
#include "Property.h"
#include "Stulu/Scene/YAML.h"
#include "Stulu/Core/Application.h"
#include "Stulu/ScriptCore/AssemblyManager.h"

namespace Stulu {
	Ref<Property> Property::Create(MonoObject* object, MonoClassField* field) {
		if (!object || !field)
			return nullptr;

		MonoType* type = mono_field_get_type(field);
		std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_REFLECTION);
		
		for (auto [name, createFn]: TypeRegister) {
			if (typeName == name) {
				return createFn(object, field);
			}
		}
		return createRef<MiscProperty>(object, field);
	}
	BaseProperty::BaseProperty(MonoObject* object, MonoClassField* field) {
		this->m_parentObjectPtr = object;
		this->m_fieldPtr = field;
		this->m_typePtr = mono_field_get_type(field);
		this->m_name = mono_field_get_name(field);
		this->m_typeName = mono_type_get_name_full(this->m_typePtr, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_REFLECTION);
	}
#pragma region Primitives
	void Int32Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void Int32Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<int32_t>());
	}
	void Int32Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<Int32Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* Int32Property::CopyValueToBuffer() const {
		int32_t* data = (int32_t*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void Int32Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((uint32_t*)source));
	}
	int32_t Int32Property::GetValue() const {
		int32_t outValue = 0;
		if(m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void Int32Property::SetValue(int32_t value) {
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &value);
	}

	void UInt32Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void UInt32Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<uint32_t>());
	}
	void UInt32Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<UInt32Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* UInt32Property::CopyValueToBuffer() const {
		uint32_t* data = (uint32_t*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void UInt32Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((uint32_t*)source));
	}
	uint32_t UInt32Property::GetValue() const {
		uint32_t outValue = 0;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void UInt32Property::SetValue(uint32_t value) {
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &value);
	}

	void FloatProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void FloatProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<float>());
	}
	void FloatProperty::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<FloatProperty>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* FloatProperty::CopyValueToBuffer() const {
		float* data = (float*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void FloatProperty::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((float*)source));
	}
	float FloatProperty::GetValue() const {
		float outValue = 0;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void FloatProperty::SetValue(float value) {
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &value);
	}

	void BoolProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void BoolProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<bool>());
	}
	void BoolProperty::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<BoolProperty>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* BoolProperty::CopyValueToBuffer() const {
		bool* data = (bool*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void BoolProperty::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((bool*)source));
	}
	bool BoolProperty::GetValue() const {
		bool outValue = 0;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void BoolProperty::SetValue(bool value) {
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &value);
	}
#pragma endregion
#pragma region Vectors
	void Vector2Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void Vector2Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<glm::vec2>());
	}
	void Vector2Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<Vector2Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* Vector2Property::CopyValueToBuffer() const {
		glm::vec2* data = (glm::vec2*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void Vector2Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((glm::vec2*)source));
	}
	glm::vec2 Vector2Property::GetValue() const {
		glm::vec2 outValue = glm::vec2(0.0f);
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void Vector2Property::SetValue(const glm::vec2& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &temp);
	}

	void Vector3Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void Vector3Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<glm::vec3>());
	}
	void Vector3Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<Vector3Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* Vector3Property::CopyValueToBuffer() const {
		glm::vec3* data = (glm::vec3*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void Vector3Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((glm::vec3*)source));
	}
	glm::vec3 Vector3Property::GetValue() const {
		glm::vec3 outValue = glm::vec3(0.0f);
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void Vector3Property::SetValue(const glm::vec3& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &temp);
	}

	void Vector4Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void Vector4Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<glm::vec4>());
	}
	void Vector4Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<Vector4Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* Vector4Property::CopyValueToBuffer() const {
		glm::vec4* data = (glm::vec4*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void Vector4Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((glm::vec4*)source));
	}
	glm::vec4 Vector4Property::GetValue() const {
		glm::vec4 outValue = glm::vec4(0.0f);
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_get_value(m_parentObjectPtr, m_fieldPtr, &outValue);
		return outValue;

	}
	void Vector4Property::SetValue(const glm::vec4& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			mono_field_set_value(m_parentObjectPtr, m_fieldPtr, &temp);
	}
#pragma endregion
	AssetProperty::AssetProperty(MonoObject* object, MonoClassField* field)
		: BaseProperty(object, field) {
		MonoDomain* domain = Application::get().getAssemblyManager()->getDomain();
		MonoClass* fieldClass = mono_type_get_class(this->getTypePtr());
		if (fieldClass) {
			m_uuidFieldPtr = mono_class_get_field_from_name(fieldClass, "ID");
			m_assetObjectPtr = mono_field_get_value_object(domain, m_fieldPtr, m_parentObjectPtr);
			// initilized property if null
			if (m_assetObjectPtr == nullptr) {
				m_assetObjectPtr = mono_object_new(domain, fieldClass);
				mono_runtime_object_init(m_assetObjectPtr);
				mono_field_set_value(m_parentObjectPtr, m_fieldPtr, m_assetObjectPtr);
			}
		}
	}
	void AssetProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << (uint64_t)GetValue();
	}
	void AssetProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<uint64_t>());
	}
	void AssetProperty::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<Texture2DProperty>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* AssetProperty::CopyValueToBuffer() const {
		uint64_t* data = (uint64_t*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void AssetProperty::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((uint64_t*)source));
	}
	UUID AssetProperty::GetValue() const {
		UUID outValue = UUID::null;
		if (m_assetObjectPtr && m_uuidFieldPtr)
			mono_field_get_value(m_assetObjectPtr, m_uuidFieldPtr, &outValue);
		else
			CORE_ERROR("Null");
		return outValue;

	}
	void AssetProperty::SetValue(const UUID& value) {
		auto temp = value;
		if (m_assetObjectPtr && m_uuidFieldPtr)
			mono_field_set_value(m_assetObjectPtr, m_uuidFieldPtr, &temp);
		else
			CORE_ERROR("Null");
	}
}