#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>

namespace Stulu {
	struct MonoFunction {
		MonoClass* classPtr;
		MonoMethod* methodPtr;

		std::string name = "";
	};
	class STULU_API MonoObjectInstance;
	class STULU_API ScriptAssembly {
	public:
		ScriptAssembly(MonoDomain* rootDomain, const std::string& assembly);
		~ScriptAssembly();

		/// <summary>
		/// An Example for StuluEditor.Engine
		/// </summary>
		/// <param name="m_nameSpace">StuluEditor</param>
		/// <param name="m_className">Engine</param>
		/// <returns></returns>
		MonoClass* createClass(const std::string& m_nameSpace, const std::string& m_className) const;
		/// <summary>
		/// An Example for StuluEditor.Engine::Test(string,int)
		/// </summary>
		/// <param name="m_nameSpace">StuluEditor</param>
		/// <param name="m_className">Engine</param>
		/// <param name="functionName">Test(string,int)</param>
		MonoFunction createFunction(const std::string& m_nameSpace, const std::string& m_className, const std::string& functionName) const;
		/// <summary>
		/// An Example for Test(string,int)
		/// </summary>
		/// <param name="classPtr">MonoClass Pointer</param>
		/// <param name="functionName">Test(string,int)</param>
		static MonoFunction createFunction(MonoClass* classPtr, const std::string& functionName);

		MonoObject* invokeFunction(const MonoFunction& function, void* obj, void** args) const;
		MonoObject* invokeFunction(MonoMethod* function, void* obj, void** args) const;

		MonoDomain* getDomain() const { return m_monoScriptDomain; }
		MonoDomain* getRootDomain() const { return m_monoRootDomain; }
		MonoAssembly* getAssembly() const { return m_monoAssembly; }
		MonoImage* getImage() const { return m_monoImage; }

		void setErrorCallback(std::function<void(const std::string&, MonoMethod*)>& ca) { m_errorCallBack = ca; };

		//for reloading the assembly
		void registerObject(Ref<MonoObjectInstance> object) { m_objects.push_back(object); }
		
		struct MonoClassEntry {
			std::string name;
			std::string nameSpace;
		};
		void loadAllClasses(MonoClass* parentClass);
		std::vector<MonoClassEntry>& getClasses() { return m_classes; }


		void reload(std::function<bool(const std::string&)>& recompileFinished, std::function<bool(const std::string&)>& recompile);
	private:
		const std::string m_assembly;
		MonoDomain* m_monoRootDomain = nullptr;
		MonoDomain* m_monoScriptDomain = nullptr;
		MonoAssembly* m_monoAssembly = nullptr;
		MonoImage* m_monoImage = nullptr;
		std::vector<MonoClassEntry> m_classes;
		std::vector<Ref<MonoObjectInstance>> m_objects;


		std::function<void(const std::string&, MonoMethod* func)> m_errorCallBack;
	};
}