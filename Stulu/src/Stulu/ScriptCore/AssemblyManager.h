#pragma once
#include "MonoObjectInstance.h"

namespace Stulu {
	class STULU_API AssemblyManager {
	public:
		AssemblyManager(const std::string& assemblyPath, const std::string& monoAssemblyDir = ".", const std::string& monoConfigDir = ".");
		~AssemblyManager();

		const Ref<ScriptAssembly>& getAssembly() { return m_assembly; }
		const Ref<ScriptAssembly>& getScriptCoreAssembly() { return m_scriptCoreAssembly; }
		MonoDomain* getDomain() const { return m_monoDomain; }

	private:
		void loadScriptCore(const std::string& assemblyPath);

		Ref<ScriptAssembly> m_assembly = nullptr;
		Ref<ScriptAssembly> m_scriptCoreAssembly = nullptr;
		MonoDomain* m_monoDomain = nullptr;
	};
}