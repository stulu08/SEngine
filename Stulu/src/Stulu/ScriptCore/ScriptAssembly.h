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
	class STULU_API ScriptAssembly {
	public:
		ScriptAssembly(const std::string& name, const std::string& assembly);
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
		/// <param name="functionName">:Test(string,int)</param>
		MonoFunction createFunction(const std::string& m_nameSpace, const std::string& m_className, const std::string& functionName) const;
		/// <summary>
		/// An Example for .Engine::Test(string,int)
		/// </summary>
		/// <param name="classPtr">MonoClass Pointer</param>
		/// <param name="m_className">Engine</param>
		/// <param name="functionName">:Test(string,int)</param>
		static MonoFunction createFunction(MonoClass* classPtr, const std::string& functionName);

		MonoObject* invokeFunction(const MonoFunction& function, void* obj, void** args) const;

		MonoDomain* getDomain() const { return m_monoDomain; }
		MonoAssembly* getAssembly() const { return m_monoAssembly; }
		MonoImage* getImage() const { return m_monoImage; }

		void setErrorCallback(std::function<void(const std::string&, const MonoFunction&)>& ca) { m_errorCallBack = ca; };

	private:
		MonoDomain* m_monoDomain = nullptr;
		MonoAssembly* m_monoAssembly = nullptr;
		MonoImage* m_monoImage = nullptr;

		std::function<void(const std::string&, const MonoFunction&)> m_errorCallBack;
	};
}