#include "st_pch.h"
#include "ManagedProperty.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Scripting/Managed/AssemblyManager.h"
#include "Stulu/Scene/GameObject.h"

#include "Stulu/Scripting/Managed/Bindings/Bindings.h"

namespace Stulu {
	Ref<ManagedProperty> ManagedProperty::Create(Mono::Object object, Mono::ClassField field) {
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
		return createRef<ManagedMiscProperty>(object, field);
	}
	ManagedProperty::ManagedProperty(Mono::Object object, Mono::ClassField field) {
		this->m_parentObjectPtr = object;
		this->m_fieldPtr = field;
	}
	std::string ManagedProperty::getName() const {
		return m_fieldPtr.GetName();
	}
	std::string ManagedProperty::getTypeName() const {
		return getDataType().GetNameFull(Mono::TypeNameFormat::REFLECTION);
	}
	Mono::Type ManagedProperty::getDataType() const {
		return m_fieldPtr.GetType();
	}
#pragma region Primitives
	void Int32Property::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void Int32Property::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<int32_t>());
	}
	void Int32Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void UInt32Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void UInt64Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void FloatProperty::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void BoolProperty::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void Vector2Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void Vector3Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
	void Vector4Property::CopyValueTo(Ref<ManagedProperty> other) const {
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
		: ManagedProperty(object, field) {
		const auto& assembly = Application::get().getAssemblyManager()->getAppAssembly();
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Class assetClass = getDataType().GetClass();

		if (assetClass) {
			m_idField = assetClass.GetFieldFromName("assetID");
			m_gcField = assetClass.GetFieldFromName("gcHandle");
			m_initMethod = assetClass.GetMethodFromNameInHierarchy("Initilize", 1);
		}
		else {
			CORE_ERROR("AssetProperty: Missing 'assetClass' field.");
		}
		CORE_ASSERT(m_idField, "AssetProperty: Missing 'assetID' field.");
		CORE_ASSERT(m_gcField, "AssetProperty: Missing 'gcHandle' field.");
		CORE_ASSERT(m_initMethod, "AssetProperty: Missing 'Initialize' method.");
	}
	void AssetProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << GetValue();
	}
	void AssetProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<UUID>());
	}
	void AssetProperty::CopyValueTo(Ref<ManagedProperty> other) const {
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
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Object assetHandle = m_fieldPtr.GetValueObject(domain, m_parentObjectPtr);

		uint64_t id = UUID::null;
		if (assetHandle) {
			m_idField.GetValue(assetHandle, &id);
		}
		return id;
	}
	void AssetProperty::SetValue(const UUID& value) {
		if (value == UUID::null) {
			m_fieldPtr.SetValue(m_parentObjectPtr, nullptr);
			return;
		}

		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		const auto& assembly = Application::get().getAssemblyManager()->getScriptCoreAssembly();

		Mono::Class assetClass = m_fieldPtr.GetType().GetClass();
		if (!assetClass) {
			CORE_ERROR("AssetProperty::SetValue: Cannot resolve class.");
			return;
		}
		Mono::Object newAssetHandle = Mono::Object::New(domain, assetClass);

		// reset garbage collector handle
		m_gcField.SetValue(newAssetHandle, (void*)&GC_NULL);

		uint64_t assetID = value;
		void* arguments[1] = { &assetID };
		assembly->InvokeMethod(m_initMethod, newAssetHandle, arguments);
		m_fieldPtr.SetValue(m_parentObjectPtr, newAssetHandle);
	}
#pragma endregion
	GameObjectProperty::GameObjectProperty(Mono::Object object, Mono::ClassField field)
		: ManagedProperty(object, field) {
		const auto& assembly = Application::get().getAssemblyManager()->getAppAssembly();
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Class gameObjectClass = getDataType().GetClass();

		if (gameObjectClass) {
			m_idField = gameObjectClass.GetFieldFromName("ID");
			m_initMethod = gameObjectClass.GetMethodFromName("Init", 1);
		}
		else {
			CORE_ERROR("GameObjectProperty: Missing 'gameObjectClass' field.");
		}
		CORE_ASSERT(m_idField, "GameObjectProperty: Missing 'ID' field.");
		CORE_ASSERT(m_initMethod, "GameObjectProperty: Missing 'Init' method.");
	}
	void GameObjectProperty::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Value" << (uint64_t)GetValue();
	}
	void GameObjectProperty::Deserialize(YAML::detail::iterator_value& node) {
		SetValue(node["Value"].as<entt::entity>());
	}
	void GameObjectProperty::CopyValueTo(Ref<ManagedProperty> other) const {
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
		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		Mono::Object objectHandle = m_fieldPtr.GetValueObject(domain, m_parentObjectPtr);

		entt::entity id = entt::null;
		if (objectHandle) {
			m_idField.GetValue(objectHandle, &id);
		}
		return id;
	}
	void GameObjectProperty::SetValue(entt::entity value) {
		if (value == entt::null) {
			m_fieldPtr.SetValue(m_parentObjectPtr, nullptr);
			return;
		}

		const auto& domain = Application::get().getAssemblyManager()->getCoreDomain();
		const auto& assembly = Application::get().getAssemblyManager()->getScriptCoreAssembly();

		Mono::Class gameObjectClass = m_fieldPtr.GetType().GetClass();
		if (!gameObjectClass) {
			CORE_ERROR("GameObjectProperty::SetValue: Cannot resolve class.");
			return;
		}
		Mono::Object newGameObjectHandle = Mono::Object::New(domain, gameObjectClass);

		entt::entity objectID = value;
		void* arguments[1] = { &objectID };
		assembly->InvokeMethod(m_initMethod, newGameObjectHandle, arguments);
		m_fieldPtr.SetValue(m_parentObjectPtr, newGameObjectHandle);
	}
}