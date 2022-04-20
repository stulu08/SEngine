#include "st_pch.h"
#include "MonoObjectInstance.h"

#include "AssemblyManager.h"
#include <Stulu.h>

namespace Stulu {
	MonoObjectInstance::MonoObjectInstance(const std::string& nameSpace, const std::string& className, ScriptAssembly* assembly)
		:m_nameSpace(nameSpace),m_className(className), m_assembly(assembly),m_constructed(false) {
		m_classPtr = m_assembly->createClass(m_nameSpace, m_className);
		if (m_classPtr) {
			m_objectPtr = mono_object_new(m_assembly->getDomain(), m_classPtr);
			m_gCHandle = mono_gchandle_new(m_objectPtr, false);
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance from {0}.{1}", m_nameSpace, m_className);
	}

	MonoObjectInstance::~MonoObjectInstance() {
		for (auto i : m_fields) {
			if (i.second.value) {
				delete(i.second.value);
			}
			i.second.m_fieldPtr = nullptr;
			i.second.m_typePtr = nullptr;
		}
		if (m_gCHandle) {
			mono_gchandle_free(m_gCHandle);
			m_gCHandle = 0;
		}
	}

	void MonoObjectInstance::addFunction(const std::string& fnName, const MonoFunction& mfn) {
		m_functions[fnName] = mfn;
	}

	void MonoObjectInstance::loadAll() {
		loadAllClassFunctions();
		loadAllVirtualParentFunctions();
		callDefaultConstructor();
		loadAllClassFields();
		setAllClassFields();
	}

	void MonoObjectInstance::loadFunction(const std::string& fnName) {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return;
		}
		m_functions[fnName] = m_assembly->createFunction(m_classPtr, fnName);
		m_functions[fnName].name = m_nameSpace + "." + m_className + ":" + fnName;
	}

	void MonoObjectInstance::loadVirtualFunction(const std::string& fnName, MonoClass* functionClass) {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return;
		}
		MonoFunction function;
		function.classPtr = m_classPtr;
		if(m_nameSpace.empty())
			function.name = m_className + ":" + fnName;
		else
			function.name = m_nameSpace + "." + m_className + ":" + fnName;

		std::string IclassName = mono_class_get_name(functionClass);

		if (function.classPtr) {
			std::string m = m_nameSpace + "." + IclassName + ":" + fnName;
			MonoMethodDesc* descPtr = mono_method_desc_new(m.c_str(), true);
			if (descPtr) {
				MonoMethod* virtualMethod = mono_method_desc_search_in_class(descPtr, functionClass);
				if (virtualMethod) {
					function.methodPtr = mono_object_get_virtual_method(m_objectPtr, virtualMethod);
					m_functions[function.name] = function;
					mono_method_desc_free(descPtr);
					return;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}", function.name);
	}
	MonoMethod* MonoObjectInstance::getVirtualFunction(const std::string& fnName, MonoClass* functionClass) {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return nullptr;
		}

		std::string IclassName = mono_class_get_name(functionClass);

		if (m_classPtr) {
			std::string m = m_nameSpace + "." + IclassName + ":" + fnName;
			MonoMethodDesc* descPtr = mono_method_desc_new(m.c_str(), true);
			if (descPtr) {
				MonoMethod* virtualMethod = mono_method_desc_search_in_class(descPtr, functionClass);
				if (virtualMethod) {
					MonoMethod* meth = mono_object_get_virtual_method(m_objectPtr, virtualMethod);
					mono_method_desc_free(descPtr);
					return meth;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}.{1}", fnName, m_nameSpace);
		return nullptr;
	}
	void MonoObjectInstance::loadAllClassFunctions() {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return;
		}
		void* iter = NULL;
		MonoMethod* method;
		while (method = mono_class_get_methods(m_classPtr, &iter)) {
			MonoFunction func;
			func.name = mono_method_full_name(method, 1);
			if (func.name.find_first_of(" (") != func.name.npos)
				func.name.replace(func.name.find_first_of(" ("), 1, "");//remove space befor the parameters "exampleFunc (uint,string)" -> "exampleFunc(uint,string)"

			MonoMethodDesc* desc = mono_method_desc_new(func.name.c_str(), true);
			if (desc) {
				func.methodPtr = mono_method_desc_search_in_class(desc, m_classPtr);
				mono_method_desc_free(desc);
			}
			func.classPtr = m_classPtr;

			m_functions[func.name] = func;
		}
	}
	void MonoObjectInstance::loadAllVirtualParentFunctions() {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return;
		}
		void* iter = NULL;
		MonoClass* interf;
		interf = mono_class_get_parent(m_classPtr);
		if (interf) {
			MonoMethod* virtualMethod;
			while (virtualMethod = mono_class_get_methods(interf, &iter)) {
				std::string fnName = mono_method_full_name(virtualMethod, true);//Stulu.Component:onStart () -> onStart()
				if (fnName.find_first_of(" (") != fnName.npos)
					fnName.replace(fnName.find_first_of(" ("), 1, "");//remove space befor the parameters "Stulu.Component:onStart ()" -> "Stulu.Component:onStart()"
				size_t ld = fnName.find_last_of(':');
				loadVirtualFunction(fnName.substr(ld+1,fnName.size()), interf);
			}
			
		}
		
	}
	template<typename T>
	T* assignFieldValue(MonoClassField* src, MonoObject* object) {
		T value;
		mono_field_get_value(object, src, &value);

		//T* dst = (T*)malloc(sizeof(T));
		T* dst = new T();
		*dst = value;
		return dst;
	}
	template<typename T>
	T* updateFieldValue(void* dest, MonoClassField* src, MonoObject* object) {
		T value;
		mono_field_get_value(object, src, &value);

		T* dst = (T*)dest;
		*dst = value;
		return dst;
	}
	void MonoObjectInstance::loadAllClassFields() {
		void* iter = NULL;
		MonoClassField* field;
		while (field = mono_class_get_fields(m_classPtr, &iter)) {
			std::string name = mono_field_get_name(field);
			if (m_fields.find(name) != m_fields.end()) {
				if(m_fields.at(name).value)
					delete(m_fields.at(name).value);
				m_fields.at(name).atrributeList.clear();
				m_fields.erase(name);
			}
			MonoClassMember mem;
			mem.m_fieldPtr = field;
			mem.m_typePtr = mono_field_get_type(field);
			mem.name = name;
			mem.typeName = mono_type_get_name_full(mem.m_typePtr,MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_REFLECTION);

			if (mem.typeName.empty())
				mem.type = MonoClassMember::Type::Other;
			else if (mem.typeName == "Stulu.Vector4") {
				mem.type = MonoClassMember::Type::Vector4_t;
				mem.value = assignFieldValue<glm::vec4>(field, m_objectPtr);
			}
			else if (mem.typeName == "Stulu.Vector3") {
				mem.type = MonoClassMember::Type::Vector3_t;
				mem.value = assignFieldValue<glm::vec3>(field, m_objectPtr);
			}
			else if (mem.typeName == "Stulu.Vector2") {
				mem.type = MonoClassMember::Type::Vector2_t;
				mem.value = assignFieldValue<glm::vec2>(field, m_objectPtr);
			}
			else if (mem.typeName == "System.Single") {
				mem.type = MonoClassMember::Type::float_t;
				mem.value = assignFieldValue<float>(field, m_objectPtr);
			}
			else if (mem.typeName == "System.Int32") {
				mem.type = MonoClassMember::Type::int_t;
				mem.value = assignFieldValue<int32_t>(field, m_objectPtr);
			}
			else if (mem.typeName == "System.UInt32") {
				mem.type = MonoClassMember::Type::uint_t;
				mem.value = assignFieldValue<uint32_t>(field, m_objectPtr);
			}
			else
				mem.type = MonoClassMember::Type::Other;
			m_fields[name] = mem;
		}
	}

	void MonoObjectInstance::reloadClassFieldValue(const std::string& field) {
		if (m_fields.find(field) == m_fields.end())
			return;
		auto& mem = m_fields.at(field);

		switch (mem.type) {
		case MonoClassMember::Type::Vector4_t:
			mem.value = updateFieldValue<glm::vec4>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		case MonoClassMember::Type::Vector3_t:
			mem.value = updateFieldValue<glm::vec3>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		case MonoClassMember::Type::Vector2_t:
			mem.value = updateFieldValue<glm::vec2>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		case MonoClassMember::Type::float_t:
			mem.value = updateFieldValue<float>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		case MonoClassMember::Type::int_t:
			mem.value = updateFieldValue<int32_t>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		case MonoClassMember::Type::uint_t:
			mem.value = updateFieldValue<uint32_t>(mem.value, mem.m_fieldPtr, m_objectPtr);
			break;
		}
	}

	void MonoObjectInstance::setClassField(const std::string& field) const {
		if (m_fields.find(field) == m_fields.end())
			return;
		auto& i = m_fields.at(field);
		if (i.value != nullptr && i.type != MonoClassMember::Type::Other) {
			mono_field_set_value(m_objectPtr, i.m_fieldPtr, i.value);
		}
	}

	void MonoObjectInstance::setAllClassFields() const {
		if (m_objectPtr && m_classPtr) {
			for (auto& i : m_fields) {
				if (i.second.value != nullptr && i.second.type != MonoClassMember::Type::Other) {
					mono_field_set_value(m_objectPtr, i.second.m_fieldPtr, i.second.value);
				}
			}
		}
	}

	void MonoObjectInstance::callDefaultConstructor() const {
		if (m_objectPtr) {
			mono_runtime_object_init(m_objectPtr);
			m_constructed = true;
		}
	}

	void MonoObjectInstance::callConstructor(const std::string& params, void** args) const {
		call((std::string(".ctor") + params), args, false);
		m_constructed = true;
	}

	MonoObject* MonoObjectInstance::call(const std::string& func, void** args, bool isStatic) const {
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return nullptr;
		}
		const std::string name = (m_nameSpace.empty() ? "" : m_nameSpace + ".") + m_className + ":" + func;
		if (m_functions.find(name) != m_functions.end()) {
			return m_assembly->invokeFunction(m_functions.at(name), isStatic ? NULL : m_objectPtr, args);
		}
		CORE_ERROR("Function not found: {0}", name);
		return nullptr;
	}
	void MonoObjectInstance::reload() {
		for (auto [name, field] : m_fields) {
			field.atrributeList.clear();
			field.m_fieldPtr = nullptr;
			field.m_typePtr = nullptr;
		}
		std::unordered_map<std::string, MonoClassMember> fieldsChache = m_fields;
		m_fields.clear();
		if (m_gCHandle) {
			mono_gchandle_free(m_gCHandle);
			m_gCHandle = 0;
		}
		m_constructed = false;


		m_classPtr = m_assembly->createClass(m_nameSpace, m_className);
		if (m_classPtr) {
			m_objectPtr = mono_object_new(m_assembly->getDomain(), m_classPtr);
			m_gCHandle = mono_gchandle_new(m_objectPtr, false);

			loadAll();
			for (auto [name, field] : fieldsChache) {
				if (m_fields.find(name) == m_fields.end())
					continue;
				if (field.type == m_fields[name].type)
					m_fields[name].value = field.value;
			}
			setAllClassFields();
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance from {0}.{1}", m_nameSpace, m_className);
	}
	bool MonoObjectInstance::fieldHasAttribute(MonoClassMember& field, MonoClass* attribute) {
		if (attribute) {
			if (field.atrributeList.find(attribute) != field.atrributeList.end())
				return field.atrributeList.at(attribute);

			field.atrributeList[attribute] = false;
			if (field.m_typePtr) {
				MonoClass* parent = mono_field_get_parent(field.m_fieldPtr);
				MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parent, field.m_fieldPtr);
				
				if (attrInfo) {
					//field.atrributeList[attribute] = mono_custom_attrs_has_attr(attrInfo, attribute) != 0; idk why, but wont work so i make my own way because mono bad and me sad
					bool value = false;
					for (int i = 0; i < attrInfo->num_attrs; ++i) {
						MonoCustomAttrEntry* centry = &attrInfo->attrs[i];
						if (centry->ctor == NULL)
							continue;
						MonoClass* klass = mono_method_get_class(centry->ctor);
						std::string cAttrClassName = std::string(mono_class_get_namespace(klass)) + "." + std::string(mono_class_get_name(klass));
						std::string sAttrClassName = std::string(mono_class_get_namespace(klass)) + "." + std::string(mono_class_get_name(klass));
						value = cAttrClassName == sAttrClassName;
					}

					field.atrributeList[attribute] = value;
					mono_custom_attrs_free(attrInfo);
				}
			}
			return field.atrributeList[attribute];
		}

		return false;
	}
}