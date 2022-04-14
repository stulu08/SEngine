#pragma once
#include "Stulu/Math/Math.h"

#include "ScriptAssembly.h"
#include <variant>


namespace Stulu {
	class STULU_API MonoObjectInstance {
	public:
		//variable
		struct MonoClassMember {
			std::string name;
			std::string typeName;
			enum class Type {
				Other,Vector4_t,Vector3_t,Vector2_t,float_t,int_t,uint_t
			}type;
			void* value;
			MonoClassField* m_fieldPtr;
			MonoType* m_typePtr;
		};

		MonoObjectInstance(const std::string& m_nameSpace, const std::string& m_className, ScriptAssembly* assembly);
		~MonoObjectInstance();

		void addFunction(const std::string& fnName, const MonoFunction& mfn);

		void loadAll();

		void loadFunction(const std::string& fnName);
		void loadVirtualFunction(const std::string& fnName, MonoClass* functionClass);

		void loadAllClassFunctions();
		void loadAllVirtualParentFunctions();
		void loadAllClassFields();

		void reloadClassFieldValue(const std::string& field);

		void setAllClassFields() const;
		void setClassField(const std::string& field) const;

		MonoObject* getObjectPtr() const { return m_objectPtr; }
		const std::string& getClassName() const { return m_className; }
		const std::string& getNameSpace() const { return m_nameSpace; }
		const std::unordered_map<std::string, MonoFunction>& getFunctions() const { return m_functions; }
		const std::unordered_map<std::string, MonoClassMember>& getFields() const { return m_fields; }
		std::unordered_map<std::string, MonoClassMember>& getFields() { return m_fields; }

		void callDefaultConstructor() const;
		void callConstructor(const std::string& params = "()", void** args = nullptr) const;
		MonoObject* call(const std::string& func, void** args = NULL, bool isStatic = false) const;

		bool isContructed() { return m_constructed; }
	private:
		void reload();


		mutable bool m_constructed;

		uint32_t m_gCHandle = 0;
		MonoClass* m_classPtr = nullptr;
		MonoObject* m_objectPtr = nullptr;
		ScriptAssembly* m_assembly;

		std::string m_nameSpace = "", m_className = "";

		//function name: print(int)
		std::unordered_map<std::string, MonoFunction> m_functions;
		std::unordered_map<std::string, MonoClassMember> m_fields;

		friend class ScriptAssembly;
	};
}