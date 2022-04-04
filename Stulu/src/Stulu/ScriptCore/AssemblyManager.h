#pragma once
#include "MonoObjectInstance.h"

namespace Stulu {
	class STULU_API AssemblyManager {
	public:
		AssemblyManager(const std::string& monoAssemblyDir, const std::string& monoConfigDir);
		~AssemblyManager();

		const Ref<ScriptAssembly>& getScriptCoreAssembly() { return m_scriptCore; }

	private:
		void loadScriptCore();

		Ref<ScriptAssembly> m_scriptCore = nullptr;
	};
}