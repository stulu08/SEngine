#include "st_pch.h"
#include "MonoObjectInstance.h"

#include "AssemblyManager.h"
#include <Stulu.h>
#include <mono/metadata/reflection.h>

namespace Stulu {
	MonoObjectInstance::MonoObjectInstance(Mono::Class clasz, ScriptAssembly* assembly)
		: m_class(clasz), m_assembly(assembly), m_initilized(false) {
		ST_PROFILING_FUNCTION();
		if (m_class) {
			CreateObject();
			m_assembly->RegisterObject(m_objectID, this);
			m_classID = m_class.GetNamespace() + "@" + m_class.GetName();

			LoadAllClassFields();
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance");
	}
	MonoObjectInstance::MonoObjectInstance(const MonoObjectInstance& other) {
		ST_PROFILING_FUNCTION();
		this->m_assembly = other.m_assembly;
		this->m_initilized = false;

		m_class = other.m_class;
		if (m_class) {
			CreateObject();
			m_assembly->RegisterObject(m_objectID, this);
			m_classID = m_class.GetNamespace() + "@" + m_class.GetName();

			LoadAllClassFields();

			for (auto otherField : other.m_fields) {
				for (auto thisField : m_fields) {
					if (thisField->getField() == otherField->getField()) {
						otherField->CopyValueTo(thisField);
					}
				}
			}
		}
		else {
			CORE_ERROR("Could not create MonoObjectInstance");
		}
	}
	void MonoObjectInstance::CreateObject() {
		Mono::Object temp = Mono::Object::New(Stulu::Application::get().getAssemblyManager()->getCoreDomain(), m_class);
		Mono::RuntimeObjectInit(temp);
		m_garbageHandle = Mono::GCHandle::New(temp, false);
		m_object = m_garbageHandle.GetTarget();
	}

	MonoObjectInstance::~MonoObjectInstance() {
		ST_PROFILING_FUNCTION();

		if (m_reloadFieldsChache) {
			// in case there are still cache field buffers allocated
			const auto chache = *m_reloadFieldsChache;
			for (auto& [name, buffer] : chache) {
				if (buffer != NULL)
					free(buffer);
			}
			delete m_reloadFieldsChache;
		}

		if (!m_assembly)
			return;
		if (m_garbageHandle) {
			m_garbageHandle.Free();
		}
		m_assembly->DeRegisterObject(m_objectID);
	}
	void MonoObjectInstance::LoadAllClassFields() {
		m_fields.clear();
		Mono::ClassField field = nullptr;
		
		for (Mono::Class parent = m_class; parent != nullptr; parent = parent.GetParent()) {
			void* iter = NULL;
			while (field = parent.GetFields(&iter)) {
				Ref<Property> property = Property::Create(getObject(), field);
				if (property) {
					m_fields.push_back(property);
				}
			}
		}
	}

	void MonoObjectInstance::CallConstructor(uint32_t params, void** args) {
		Mono::Method ctor = m_class.GetMethodFromName(".ctor", params);
		CallMethod(ctor, args, false);
	}
	Mono::Object MonoObjectInstance::CallMethod(Mono::Method method, void** args, bool isStatic) {
		if (method) {
			return m_assembly->InvokeMethod(method, isStatic ? NULL : m_object, args);
		}
		return nullptr;
	}

	void MonoObjectInstance::Reload() {
		BackupFields();
		if (m_garbageHandle) {
			m_garbageHandle.Free();
		}
		m_initilized = false;

		size_t seperator = m_classID.find('@');
		if (seperator != m_classID.npos) {
			const std::string className = m_classID.substr(seperator + 1);
			const std::string classSpace = m_classID.substr(0, seperator);
			m_class = m_assembly->CreateClass(classSpace, className);
			if (m_class) {
				CreateObject();
				LoadAllClassFields();
				LoadFieldsBackup();
				return;
			}
		}
		CORE_ERROR("Could not create MonoObjectInstance with classID: ", m_classID);
	}
	void MonoObjectInstance::BackupFields(bool overwrite) {
		if (!m_reloadFieldsChache || overwrite)
			m_reloadFieldsChache = new std::unordered_map<std::string, void*>(std::move(CreateFieldBackup()));
	}
	void MonoObjectInstance::LoadFieldsBackup() {
		auto& cache = *m_reloadFieldsChache;
		LoadFieldsBackupFrom(cache);

		if (m_reloadFieldsChache) {
			delete m_reloadFieldsChache;
			m_reloadFieldsChache = nullptr;
		}
	}
	Ref<Property> MonoObjectInstance::FindField(const std::string& name) const {
		for (int i = 0; i < m_fields.size(); i++) {
			const auto& prop = m_fields.at(i);
			if (prop->getName() == name)
				return prop;
		}
		return nullptr;
	}
	std::unordered_map<std::string, void*> MonoObjectInstance::CreateFieldBackup() {
		std::unordered_map<std::string, void*> map;
		for (auto field : m_fields) {
			std::string id = field->getName() + "@" + std::to_string((uint32_t)field->getType());
			map.insert({ id, field->CopyValueToBuffer() });
		}
		return map;
	}

	void MonoObjectInstance::LoadFieldsBackupFrom(std::unordered_map<std::string, void*>& map) {
		for (auto field : m_fields) {
			std::string id = field->getName() + "@" + std::to_string((uint32_t)field->getType());
			if (map.find(id) != map.end()) {
				field->SetValueFromBuffer(map[id]);
			}
		}
		for (auto& [name, buffer] : map) {
			if (buffer) {
				free(buffer);
				buffer = nullptr;
			}
		}
	}
	bool MonoObjectInstance::FieldHasAttribute(Ref<Property> field, Mono::Class attribute) {
		if (attribute) {
			if (field->getDataType()) {
				Mono::Class parent = field->getField().GetParent();
				MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parent, field->getField());
				
				if (attrInfo) {
					std::string AttrClassName = attribute.GetNamespace() + "." + attribute.GetName();
					bool has = false;
					for (int i = 0; i < attrInfo->num_attrs; ++i) {
						MonoCustomAttrEntry* centry = &attrInfo->attrs[i];
						if (centry->ctor == NULL)
							continue;
						Mono::Class klass = mono_method_get_class(centry->ctor);
						std::string cAttrClassName = klass.GetNamespace() + "." + klass.GetName();
						if (AttrClassName == cAttrClassName) {
							has = true;
							break;
						}
					}
					mono_custom_attrs_free(attrInfo);
					return has;
				}
			}
		}
	
		return false;
	}
}