#include "st_pch.h"
#include "Property.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Scripting/Managed/AssemblyManager.h"
#include "Stulu/Scene/GameObject.h"

#include "Stulu/Scripting/Managed/Bindings/Bindings.h"

namespace Stulu {
	Ref<Property> Property::Create(Mono::Object object, Mono::ClassField field) {
		if (!object || !field)
			return nullptr;

		const Mono::Type type = field.GetType();
		const std::string typeName = type.GetNameFull(Mono::TypeNameFormat::REFLECTION);

		const auto& propRegister = Application::get().getAssemblyManager()->GetPropertieRegister();

		for (const auto& [name, createFn] : propRegister) {
			if (typeName == name) {
				return createFn(object, field);
			}
		}
		return createRef<MiscProperty>(object, field);
	}
	BaseProperty::BaseProperty(Mono::Object object, Mono::ClassField field) {
		this->m_parentObjectPtr = object;
		this->m_fieldPtr = field;
		this->m_typePtr = field.GetType();
		this->m_name = field.GetName();
		this->m_typeName = this->m_typePtr.GetNameFull(Mono::TypeNameFormat::REFLECTION);
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
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void Int32Property::SetValue(int32_t value) {
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &value);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void UInt32Property::SetValue(uint32_t value) {
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &value);
	}
	
	void UInt64Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void UInt64Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<uint64_t>());
	}
	void UInt64Property::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<UInt64Property>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* UInt64Property::CopyValueToBuffer() const {
		uint64_t* data = (uint64_t*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void UInt64Property::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((uint64_t*)source));
	}
	uint64_t UInt64Property::GetValue() const {
		uint32_t outValue = 0;
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void UInt64Property::SetValue(uint64_t value) {
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &value);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void FloatProperty::SetValue(float value) {
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &value);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void BoolProperty::SetValue(bool value) {
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &value);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void Vector2Property::SetValue(const glm::vec2& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &temp);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void Vector3Property::SetValue(const glm::vec3& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &temp);
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
			m_fieldPtr.GetValue(m_parentObjectPtr, &outValue);
		return outValue;

	}
	void Vector4Property::SetValue(const glm::vec4& value) {
		auto temp = value;
		if (m_parentObjectPtr && m_fieldPtr)
			m_fieldPtr.SetValue(m_parentObjectPtr, &temp);
	}
#pragma endregion
#pragma region Assets
	AssetProperty::AssetProperty(Mono::Object object, Mono::ClassField field)
		: BaseProperty(object, field) {
		const auto& assembly = Application::get().getAssemblyManager()->getAppAssembly();
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Class assetClass = getDataType().GetClass();

		if (assetClass) {
			uint64_t id = 0;
			m_asset = createRef<MonoObjectInstance>(assetClass, assembly.get());
			m_idProperty = m_asset->FindFieldAs<UInt64Property>("_id");

			// get value if already asigned
			Mono::Object asignedAsset = m_fieldPtr.GetValueObject(domain, m_parentObjectPtr);
			if (asignedAsset) {
				UInt64Property other = UInt64Property(asignedAsset, m_idProperty->getField());
				uint64_t id = other.GetValue();
				SetValue(id);
			}
		}
		else {
			CORE_ERROR("Cant create AssetProperty, assetClass == 0")
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
		auto otherProp = std::dynamic_pointer_cast<AssetProperty>(other);
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
		if (m_idProperty)
			outValue = m_idProperty->GetValue();
		else
			CORE_ERROR("Cant get value of asset, m_idProperty == 0");
		return outValue;

	}
	void AssetProperty::SetValue(const UUID& value) {
		if (m_idProperty) {
			m_idProperty->SetValue((uint64_t)value);
		}
		else
			CORE_ERROR("Cant set value of asset, m_idProperty == 0");
	}
#pragma endregion
	GameObjectProperty::GameObjectProperty(Mono::Object object, Mono::ClassField field)
		: BaseProperty(object, field) {
		const auto& assembly = Application::get().getAssemblyManager()->getAppAssembly();
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Class goClass = getDataType().GetClass();

		if (goClass) {
			m_gameObject = createRef<MonoObjectInstance>(goClass, assembly.get());
			m_idProperty = m_gameObject->FindFieldAs<UInt64Property>("ID");

			// get value if already asigned
			Mono::Object assignedGo = m_fieldPtr.GetValueObject(domain, m_parentObjectPtr);
			if (assignedGo) {
				UInt32Property other = UInt32Property(assignedGo, m_idProperty->getField());
				uint32_t id = other.GetValue();
				SetValueRaw(id);
			}
			else {
				SetValueRaw(entt::null);
			}
		}
		else {
			CORE_ERROR("Cant create GameObjectProperty, goClass == 0")
		}
	}
	void GameObjectProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << (uint64_t)GetValue();
	}
	void GameObjectProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue((entt::entity)node["Value"].as<uint64_t>());
	}
	void GameObjectProperty::CopyValueTo(Ref<Property> other) const {
		if (other->getType() != this->getType())
			return;
		auto otherProp = std::dynamic_pointer_cast<GameObjectProperty>(other);
		otherProp->SetValue(this->GetValue());
	}
	void* GameObjectProperty::CopyValueToBuffer() const {
		entt::entity* data = (entt::entity*)malloc(this->getSize());
		*data = GetValue();
		return data;
	}
	void GameObjectProperty::SetValueFromBuffer(void* source) {
		if (!source)
			return;
		SetValue(*((entt::entity*)source));
	}
	entt::entity GameObjectProperty::GetValue() const {
		GameObject obj = GameObject((entt::entity)GetValueRaw(), StuluBindings::GetCurrentScene());
		if(obj.IsValid())
			return obj.GetID();
		return entt::null;
	}
	void GameObjectProperty::SetValue(entt::entity value) {
		SetValueRaw(GameObject::GetById(value, StuluBindings::GetCurrentScene()));
	}
	uint64_t GameObjectProperty::GetValueRaw() const {
		uint64_t outValue = entt::null;
		if (m_idProperty)
			outValue = m_idProperty->GetValue();
		else
			CORE_ERROR("Cant get value of GameObject, m_idProperty == 0");

		return (uint64_t)outValue;
	}
	void GameObjectProperty::SetValueRaw(uint64_t value) {
		if (m_idProperty) {
			m_idProperty->SetValue(value);
		}
		else
			CORE_ERROR("Cant set value of GameObject, m_idProperty == 0");
	}
}