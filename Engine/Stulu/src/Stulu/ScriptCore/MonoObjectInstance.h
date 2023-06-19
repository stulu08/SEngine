#pragma once
#include "Stulu/Math/Math.h"

#include "Property/Property.h"
#include "ScriptAssembly.h"

namespace Stulu {
	class STULU_API MonoObjectInstance {
	public:
		MonoObjectInstance(const std::string& m_nameSpace, const std::string& m_className, ScriptAssembly* assembly);
		MonoObjectInstance(const MonoObjectInstance&);
		MonoObjectInstance(MonoObjectInstance&&) = default;
		~MonoObjectInstance();

		void addFunction(const std::string& fnName, const MonoFunction& mfn);

		void loadAll();

		void loadFunction(const std::string& fnName);
		void loadVirtualFunction(const std::string& fnName, MonoClass* functionClass);
		MonoMethod* getVirtualFunction(const std::string& fnName, MonoClass* functionClass) const;

		void loadAllClassFunctions();
		void loadAllVirtualParentFunctions();
		void loadAllClassFields();

		MonoObject* getObjectPtr() const { return m_objectPtr; }
		MonoClass* getClassPtr() const { return m_classPtr; }
		const std::string& getClassName() const { return m_className; }
		const std::string& getNameSpace() const { return m_nameSpace; }
		const std::unordered_map<std::string, MonoFunction>& getFunctions() const { return m_functions; }
		const std::vector<Ref<Property>>& getFields() const { return m_fields; }
		std::vector<Ref<Property>>& getFields() { return m_fields; }
		std::vector<std::string>& getFieldOrder() { return m_fieldOrder; }

		void callDefaultConstructor() const;
		void callConstructor(const std::string& params = "()", void** args = nullptr) const;
		MonoObject* call(const std::string& func, void** args = NULL, bool isStatic = false) const;

		static bool FieldHasAttribute(Ref<Property> field, MonoClass* attribute);

		bool isContructed() { return m_constructed; }
		void backup_fields();
		void load_fields_backup();
	private:
		void reload();
		std::unordered_map<std::string, void*> m_reloadFieldsChache;

		mutable bool m_constructed;

		UUID m_objectID = UUID();
		uint32_t m_gCHandle = 0;
		MonoClass* m_classPtr = nullptr;
		MonoObject* m_objectPtr = nullptr;
		ScriptAssembly* m_assembly;

		std::string m_nameSpace = "", m_className = "";

		//function name: print(int)
		std::unordered_map<std::string, MonoFunction> m_functions;
		std::vector<Ref<Property>> m_fields;
		//stores the field names in correct order
		std::vector<std::string> m_fieldOrder;

		friend class ScriptAssembly;
	};
}