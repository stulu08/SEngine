#include "st_pch.h"
#include "MonoObjectInstance.h"

#include "AssemblyManager.h"
#include <Stulu.h>

namespace Stulu {
	MonoObjectInstance::MonoObjectInstance(const std::string& nameSpace, const std::string& className, ScriptAssembly* assembly)
		:m_nameSpace(nameSpace),m_className(className), m_assembly(assembly),m_constructed(false) {
		ST_PROFILING_FUNCTION();
		m_classPtr = m_assembly->createClass(m_nameSpace, m_className);
		if (m_classPtr) {
			m_objectPtr = mono_object_new(m_assembly->getDomain(), m_classPtr);
			m_gCHandle = mono_gchandle_new(m_objectPtr, false);
			m_assembly->RegisterObject(m_objectID, this);
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance from {0}.{1}", m_nameSpace, m_className);
	}
	MonoObjectInstance::MonoObjectInstance(const MonoObjectInstance& other) {
		ST_PROFILING_FUNCTION();
		this->m_nameSpace = other.m_nameSpace;
		this->m_className = other.m_className;
		this->m_assembly = other.m_assembly;
		this->m_constructed = false;

		m_classPtr = m_assembly->createClass(m_nameSpace, m_className);
		if (m_classPtr) {
			m_objectPtr = mono_object_new(m_assembly->getDomain(), m_classPtr);
			m_gCHandle = mono_gchandle_new(m_objectPtr, false);
			m_assembly->RegisterObject(m_objectID, this);
		}else
			CORE_ERROR("Could not create MonoObjectInstance from {0}.{1}", m_nameSpace, m_className);
		m_fieldOrder = other.m_fieldOrder;
		m_functions = other.m_functions;
		m_reloadFieldsChache = other.m_reloadFieldsChache;

		loadAllClassFields();
		for (auto otherField : other.m_fields) {
			for (auto thisField : m_fields) {
				if (thisField->getFieldPtr() == otherField->getFieldPtr()) {
					otherField->CopyValueTo(thisField);
				}
			}
		}
	}
	MonoObjectInstance::~MonoObjectInstance() {
		ST_PROFILING_FUNCTION();

		// in case there are still cache field buffers allocated
		for (auto [name, buffer] : m_reloadFieldsChache) {
			if (buffer)
				free(buffer);
		}

		if (!m_assembly->getRootDomain())
			return;
		if (m_gCHandle) {
			mono_gchandle_free(m_gCHandle);
			m_gCHandle = 0;
		}
		m_assembly->DeRegisterObject(m_objectID);
	}

	void MonoObjectInstance::addFunction(const std::string& fnName, const MonoFunction& mfn) {
		m_functions[fnName] = mfn;
	}
	void MonoObjectInstance::loadAll(bool construct) {
		ST_PROFILING_FUNCTION();
		loadAllClassFunctions();
		loadAllVirtualParentFunctions();
		if(construct)
			callDefaultConstructor();
		loadAllClassFields();
	}

	void MonoObjectInstance::loadFunction(const std::string& fnName) {
		ST_PROFILING_FUNCTION();
		if (!m_classPtr) {
			CORE_ERROR("Invalid class: {0}.{1}", m_nameSpace, m_className);
			return;
		}
		m_functions[fnName] = m_assembly->createFunction(m_classPtr, fnName);
		m_functions[fnName].name = m_nameSpace + "." + m_className + ":" + fnName;
	}
	void MonoObjectInstance::loadVirtualFunction(const std::string& fnName, MonoClass* functionClass) {
		ST_PROFILING_FUNCTION();
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
				mono_method_desc_free(descPtr);
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}", function.name);
	}
	MonoMethod* MonoObjectInstance::getVirtualFunction(const std::string& fnName, MonoClass* functionClass) const {
		ST_PROFILING_FUNCTION();
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
				mono_method_desc_free(descPtr);
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}.{1}", fnName, m_nameSpace);
		return nullptr;
	}
	void MonoObjectInstance::loadAllClassFunctions() {
		ST_PROFILING_FUNCTION();
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
		ST_PROFILING_FUNCTION();
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
	void MonoObjectInstance::loadAllClassFields() {
		ST_PROFILING_FUNCTION();
		bool createOrder = m_fieldOrder.size() == 0;
		m_fields.clear();
		void* iter = NULL;
		MonoClassField* field;
		
		while (field = mono_class_get_fields(m_classPtr, &iter)) {
			std::string name = mono_field_get_name(field);
			for (auto field : m_fields) {
				if (field->getName() == name)
					m_fields.erase(std::find(m_fields.begin(), m_fields.end(), field));
			}
			Ref<Property> property = Property::Create(getObjectPtr(), field);
			if (property) {
				m_fields.push_back(property);
				if(createOrder)
					m_fieldOrder.push_back(name);
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
			try {
				return m_assembly->invokeFunction(m_functions.at(name), isStatic ? NULL : m_objectPtr, args);
			}
			catch (char* e) {
				CORE_ERROR("Uncaught Exception in {1}\n{0}", e, mono_method_full_name(m_functions.at(name).methodPtr, 1));
				return nullptr;
			}
		}
		CORE_ERROR("Function not found: {0}", name);
		return nullptr;
	}

	void MonoObjectInstance::reload() {
		m_fields.clear();
		m_fieldOrder.clear();
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
			load_fields_backup();
		}
		else {
			CORE_ERROR("Could not create MonoObjectInstance from {0}.{1}", m_nameSpace, m_className);
		}
		
	}
	void MonoObjectInstance::backup_fields() {
		ST_PROFILING_FUNCTION();
		m_reloadFieldsChache.clear();
		m_reloadFieldsChache = create_field_backup();
	}
	void MonoObjectInstance::load_fields_backup() {
		load_fields_backup_from(m_reloadFieldsChache);
		m_reloadFieldsChache.clear();
	}
	std::unordered_map<std::string, void*> MonoObjectInstance::create_field_backup() {
		ST_PROFILING_FUNCTION();
		std::unordered_map<std::string, void*> map;
		for (auto field : m_fields) {
			std::string id = field->getName() + "@" + std::to_string((uint32_t)field->getType());
			map.insert({ id, field->CopyValueToBuffer() });
		}
		return map;
	}

	void MonoObjectInstance::load_fields_backup_from(std::unordered_map<std::string, void*>& map) {
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
	bool MonoObjectInstance::FieldHasAttribute(Ref<Property> field, MonoClass* attribute) {
		if (attribute) {
			if (field->getTypePtr()) {
				MonoClass* parent = mono_field_get_parent(field->getFieldPtr());
				MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parent, field->getFieldPtr());
				
				if (attrInfo) {
					std::string AttrClassName = std::string(mono_class_get_namespace(attribute)) + "." + std::string(mono_class_get_name(attribute));
					bool has = false;
					for (int i = 0; i < attrInfo->num_attrs; ++i) {
						MonoCustomAttrEntry* centry = &attrInfo->attrs[i];
						if (centry->ctor == NULL)
							continue;
						MonoClass* klass = mono_method_get_class(centry->ctor);
						std::string cAttrClassName = std::string(mono_class_get_namespace(klass)) + "." + std::string(mono_class_get_name(klass));
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