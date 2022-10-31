#pragma once
#include "Stulu/Math/Math.h"

#include "ScriptAssembly.h"
#include <variant>

#ifndef DLL_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define DLL_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

// The following ifdef block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with the
// yaml_cpp_EXPORTS symbol defined on the command line. This symbol should not
// be defined on any project that uses this DLL. This way any other project
// whose source files include this file see YAML_CPP_API functions as being
// imported from a DLL, whereas this DLL sees symbols defined with this macro as
// being exported.
#undef YAML_CPP_API

#ifdef YAML_CPP_DLL      // Using or Building YAML-CPP DLL (definition defined
						 // manually)
#ifdef yaml_cpp_EXPORTS  // Building YAML-CPP DLL (definition created by CMake
						 // or defined manually)
//	#pragma message( "Defining YAML_CPP_API for DLL export" )
#define YAML_CPP_API __declspec(dllexport)
#else  // yaml_cpp_EXPORTS
//	#pragma message( "Defining YAML_CPP_API for DLL import" )
#define YAML_CPP_API __declspec(dllimport)
#endif  // yaml_cpp_EXPORTS
#else   // YAML_CPP_DLL
#define YAML_CPP_API
#endif  // YAML_CPP_DLL

#endif  // DLL_H_62B23520_7C8E_11DE_8A39_0800200C9A66

namespace YAML {
	class YAML_CPP_API Emitter;
	namespace detail {
		struct iterator_value;
	}
}

namespace Stulu {
	template<typename T>
	inline void* assignFieldValue(MonoClassField* src, MonoObject* object) {
		T value;
		mono_field_get_value(object, src, &value);

		//T* dst = (T*)malloc(sizeof(T));
		T* dst = new T();
		*dst = value;
		return dst;
	}
	template<typename T>
	inline void* updateFieldValue(void* dest, MonoClassField* src, MonoObject* object) {
		T value;
		mono_field_get_value(object, src, &value);
		T* dst = (T*)dest;
		*dst = value;
		return dst;
	}
	class STULU_API MonoObjectInstance {
	public:
		//variable
		struct MonoClassMember {
			std::string name;
			std::string typeName;
			std::unordered_map<void*, bool> atrributeList;
			enum class Type {
				Other,Vector4_t,Vector3_t,Vector2_t,float_t,int_t,uint_t,bool_t
			}type;
			void* value;
			MonoClassField* m_fieldPtr;
			MonoType* m_typePtr;
			size_t getSize() { return GetTypeSize(type); }

			static size_t GetTypeSize(Type type);

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
	struct MonoClassMemberTypeFnInfo {
		MonoObjectInstance::MonoClassMember::Type type;
		std::function<size_t()> getSize;
		std::function<void* (MonoClassField*, MonoObject*)> assignFieldValue;
		std::function<void* (MonoObjectInstance::MonoClassMember&, MonoObject*)> updateFieldValue;
		std::function<void(YAML::Emitter&, MonoObjectInstance::MonoClassMember&)> serialize;
		std::function<void(std::unordered_map<std::string, MonoObjectInstance::MonoClassMember>&, std::string, YAML::detail::iterator_value&)> deserialize;
		std::function<void(Stulu::Ref<Stulu::MonoObjectInstance>& script, MonoObjectInstance::MonoClassMember& field, const std::string& name)> uiFunc;
		static inline std::unordered_map<std::string, MonoClassMemberTypeFnInfo> infos;
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