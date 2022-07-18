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
			std::unordered_map<void*, bool> atrributeList;
			enum class Type {
				Other,Vector4_t,Vector3_t,Vector2_t,float_t,int_t,uint_t
			}type;
			void* value;
			MonoClassField* m_fieldPtr;
			MonoType* m_typePtr;
			size_t getSize() { return GetTypeSize(type); }

			static inline size_t GetTypeSize(Type type) {
				switch (type)
				{
				case Stulu::MonoObjectInstance::MonoClassMember::Type::Vector4_t:
					return sizeof(glm::vec4);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::Vector3_t:
					return sizeof(glm::vec3);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::Vector2_t:
					return sizeof(glm::vec2);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::float_t:
					return sizeof(float);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::int_t:
					return sizeof(int32_t);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::uint_t:
					return sizeof(uint32_t);
				case Stulu::MonoObjectInstance::MonoClassMember::Type::Other:
					return 0;
				}
				return 0;
			}

			bool operator==(const MonoClassMember l) const {
				return this->m_fieldPtr == l.m_fieldPtr;
			}
		};

		MonoObjectInstance(const std::string& m_nameSpace, const std::string& m_className, ScriptAssembly* assembly);
		MonoObjectInstance(const MonoObjectInstance&);
		~MonoObjectInstance();

		void addFunction(const std::string& fnName, const MonoFunction& mfn);

		void loadAll();

		void loadFunction(const std::string& fnName);
		void loadVirtualFunction(const std::string& fnName, MonoClass* functionClass);
		MonoMethod* getVirtualFunction(const std::string& fnName, MonoClass* functionClass);

		void loadAllClassFunctions();
		void loadAllVirtualParentFunctions();
		void loadAllClassFields();

		void reloadClassFieldValue(const std::string& field);

		void setAllClassFields() const;
		void setClassField(const std::string& field) const;

		MonoObject* getObjectPtr() const { return m_objectPtr; }
		MonoClass* getClassPtr() const { return m_classPtr; }
		const std::string& getClassName() const { return m_className; }
		const std::string& getNameSpace() const { return m_nameSpace; }
		const std::unordered_map<std::string, MonoFunction>& getFunctions() const { return m_functions; }
		const std::unordered_map<std::string, MonoClassMember>& getFields() const { return m_fields; }
		std::unordered_map<std::string, MonoClassMember>& getFields() { return m_fields; }
		std::vector<std::string>& getFieldOrder() { return m_fieldOrder; }

		void callDefaultConstructor() const;
		void callConstructor(const std::string& params = "()", void** args = nullptr) const;
		MonoObject* call(const std::string& func, void** args = NULL, bool isStatic = false) const;

		static bool fieldHasAttribute(MonoClassMember& field, MonoClass* attribute);

		bool isContructed() { return m_constructed; }
	private:
		void reload();

		mutable bool m_constructed;

		UUID m_objectID = UUID();
		uint32_t m_gCHandle = 0;
		MonoClass* m_classPtr = nullptr;
		MonoObject* m_objectPtr = nullptr;
		ScriptAssembly* m_assembly;

		std::string m_nameSpace = "", m_className = "";

		//function name: print(int)
		std::unordered_map<std::string, MonoFunction> m_functions;
		std::unordered_map<std::string, MonoClassMember> m_fields;
		//stores the field names in correct order
		std::vector<std::string> m_fieldOrder;

		friend class ScriptAssembly;
	};
}

namespace std {
	template<>
	struct hash<Stulu::MonoObjectInstance::MonoClassMember> {
		std::size_t operator()(const Stulu::MonoObjectInstance::MonoClassMember& field) const {
			return hash<void*>()((void*)field.m_fieldPtr);
		}
	};
}