#pragma once
#include "MonoObjectInstance.h"

#define SCRIPT_CORE_NAME "Stulu-ScriptCore"
#define SCRIPT_CORE_DLL_PATH "data/Stulu-ScriptCore.dll"
namespace Stulu {
	class STULU_API ScriptCore {
	public:
		ScriptCore(){}
		ScriptCore(const std::string& monoAssemblyPath, const std::string& monoConfigPath);
		~ScriptCore();

		/// <summary>
		/// An Example for StuluEditor.Engine
		/// </summary>
		/// <param name="nameSpace">StuluEditor</param>
		/// <param name="className">Engine</param>
		/// <returns></returns>
		MonoClass* createClass(const std::string& nameSpace, const std::string& className);
		/// <summary>
		/// An Example for StuluEditor.Engine::Test(string,int)
		/// </summary>
		/// <param name="nameSpace">StuluEditor</param>
		/// <param name="className">Engine</param>
		/// <param name="functionName">:Test (string,int)</param>
		MonoFunction createFunction(const std::string& nameSpace, const std::string& className, const std::string& functionName);
		/// <summary>
		/// An Example for .Engine::Test(string,int)
		/// </summary>
		/// <param name="classPtr">MonoClass Pointer</param>
		/// <param name="className">Engine</param>
		/// <param name="functionName">:Test (string,int)</param>
		MonoFunction createFunction(MonoClass* classPtr, const std::string& functionName); 

		MonoObject* invokeFunction(const MonoFunction& function, void* obj, void** args);

		Ref<MonoObjectInstance>& getEngineObject() { return m_engineObject; }

		static inline ScriptCore* get() { return s_instance; }
	private:
		void load();
		void addInternalCalls();

		Ref<MonoObjectInstance> m_engineObject;

		MonoDomain* m_monoScriptCoreDomain = nullptr;
		MonoAssembly* m_monoScriptCoreAssembly = nullptr;
		MonoImage* m_monoScriptCoreImage = nullptr;

		static inline ScriptCore* s_instance = nullptr;

		friend class MonoObjectInstance;
	};
}