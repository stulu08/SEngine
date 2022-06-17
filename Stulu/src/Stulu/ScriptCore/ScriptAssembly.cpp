#include "st_pch.h"
#include "ScriptAssembly.h"

#include "MonoObjectInstance.h"


namespace Stulu {
	char* constStringToCharPtr(const std::string& src) {
		return strcpy(new char[src.length() + 1], src.c_str());
	}


	ScriptAssembly::ScriptAssembly(MonoDomain* rootDomain, const std::string& assembly) 
		: m_monoRootDomain(rootDomain),m_assembly(assembly) {
		CORE_INFO("Loading Assembly: {0}", m_assembly);

		m_monoScriptDomain = mono_domain_create_appdomain(constStringToCharPtr(m_assembly), NULL);
		if (!m_monoScriptDomain) {
			CORE_ERROR("Mono Scripting Domain creation failed for {0}", m_assembly);
			return;
		}

		mono_domain_set(m_monoScriptDomain, 0);

		m_monoAssembly = mono_domain_assembly_open(mono_domain_get(), m_assembly.c_str());
		if (!m_monoAssembly) {
			CORE_ERROR("Mono Assembly creation failed for {0}", m_assembly);
			return;
		}
		m_monoImage = mono_assembly_get_image(m_monoAssembly);
		if (!m_monoImage) {
			CORE_ERROR("Mono Image creation failed for {0}", m_assembly);
			return;
		}
		m_errorCallBack = [=](const std::string& msg, MonoMethod* func) {
			CORE_ERROR("C# Runtime Error:\n{0}\nCaused in function: {1}", msg, mono_method_full_name(func, 1));//default error callback
		};
		CORE_INFO("Loading Assembly finished")
	}

	ScriptAssembly::~ScriptAssembly() {
		if (!m_monoRootDomain)
			return;
		for (auto object : m_objects) {
			object->m_assembly = nullptr;
		}
		mono_domain_set(m_monoRootDomain, 0);
		mono_domain_unload(m_monoScriptDomain);
	}

	MonoClass* ScriptAssembly::createClass(const std::string& m_nameSpace, const std::string& m_className) const {
		MonoClass* classPtr = mono_class_from_name(m_monoImage, m_nameSpace.c_str(), m_className.c_str());
		if (classPtr) {
			return classPtr;
		}
		CORE_ERROR("Could not create MonoClass* from {0}.{1}", m_nameSpace, m_className);
		return classPtr;
	}

	MonoFunction ScriptAssembly::createFunction(const std::string& m_nameSpace, const std::string& m_className, const std::string& functionName) const {
		MonoFunction function;
		function.name = functionName;

		std::string classStr = (std::string(".") + m_className + ":" + functionName);

		function.classPtr = mono_class_from_name(m_monoImage, m_nameSpace.c_str(), m_className.c_str());
		if (function.classPtr) {
			MonoMethodDesc* descPtr = mono_method_desc_new(classStr.c_str(), false);
			if (descPtr) {
				function.methodPtr = mono_method_desc_search_in_class(descPtr, function.classPtr);
				if (function.methodPtr) {
					mono_method_desc_free(descPtr);
					return function;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}{1}", m_nameSpace, classStr);
		return function;
	}

	MonoFunction ScriptAssembly::createFunction(MonoClass* classPtr, const std::string& functionName) {
		MonoFunction function;
		function.classPtr = classPtr;

		const char* m_nameSpace = mono_class_get_namespace(classPtr);
		const char* m_className = mono_class_get_name(classPtr);

		function.name = functionName;

		std::string classStr = (std::string(".") + m_className + ":" + functionName);

		if (function.classPtr) {
			MonoMethodDesc* descPtr = mono_method_desc_new(classStr.c_str(), false);
			if (descPtr) {
				function.methodPtr = mono_method_desc_search_in_class(descPtr, function.classPtr);
				if (function.methodPtr) {
					mono_method_desc_free(descPtr);
					return function;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}{1}", m_nameSpace, classStr);
		return function;
	}
	struct MonoMethodExecuterArgs {
		MonoMethod* method;
		void* object;
		void** args;
		MonoObject* ex = nullptr;
	};
	//object unwinding or something like this exists
	void MonoexecuteMethodSafe(MonoMethodExecuterArgs args) {
#ifdef ST_PLATFORM_WINDOWS
		__try {
			mono_runtime_invoke(args.method, args.object, args.args, &args.ex);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			if (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) {
				CORE_ERROR("Exception: Acces violation")
			}
			else {
				CORE_ERROR("Exception: {0}", GetExceptionCode());
			}
		}
#else
		mono_runtime_invoke(args.method, args.object, args.args, &args.ex);
#endif
		
	}
	MonoObject* ScriptAssembly::invokeFunction(const MonoFunction& function, void* obj, void** args) const {
		if (!function.methodPtr)
			return nullptr;
		MonoObject* ex = nullptr;
		MonoObject* re = nullptr;
		//ex = mono_runtime_invoke(function.methodPtr, obj, args, &ex);

		MonoMethodExecuterArgs execArgs = MonoMethodExecuterArgs();
		execArgs.method = function.methodPtr;
		execArgs.object = obj;
		execArgs.args = args;
		execArgs.ex = ex;
		
		MonoexecuteMethodSafe(execArgs);

		if (ex) {
			MonoString* excM = mono_object_to_string(ex, nullptr);
			m_errorCallBack(mono_string_to_utf8(excM), function.methodPtr);
		}
		return re;
	}
	MonoObject* ScriptAssembly::invokeFunction(MonoMethod* function, void* obj, void** args) const {
		if (!function)
			return nullptr;

		MonoObject* ex = nullptr;
		MonoObject* re = nullptr;
		re = mono_runtime_invoke(function, obj, args, &ex);

		if (ex) {
			MonoString* excM = mono_object_to_string(ex, nullptr);
			m_errorCallBack(mono_string_to_utf8(excM), function);
		}
		return re;
	}

	void ScriptAssembly::loadAllClasses(MonoClass* _parentClass) {
		const MonoTableInfo* table_info = mono_image_get_table_info(m_monoImage, MONO_TABLE_TYPEDEF);

		std::string parent = mono_class_get_namespace(_parentClass) + std::string(".") + mono_class_get_name(_parentClass);
		

		int rows = mono_table_info_get_rows(table_info);
		/* For each row, get some of its values */
		for (int i = 0; i < rows; i++)
		{
			MonoClass* _class = nullptr;
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(m_monoImage, cols[MONO_TYPEDEF_NAME]);
			const char* name_space = mono_metadata_string_heap(m_monoImage, cols[MONO_TYPEDEF_NAMESPACE]);
			_class = mono_class_from_name(m_monoImage, name_space, name);

			std::string n = mono_class_get_name(_class);

			

			if (_parentClass) {
				MonoClass* parentCl = mono_class_get_parent(_class);
				if (!parentCl)
					continue;
				std::string classParent = parent = mono_class_get_namespace(parentCl) + std::string(".") + mono_class_get_name(parentCl);
				if(classParent == parent)
					m_classes.push_back({ mono_class_get_name(_class), mono_class_get_namespace(_class)});
			}
			else {
				m_classes.push_back({ mono_class_get_name(_class), mono_class_get_namespace(_class) });
			}
		}
	}

	void ScriptAssembly::reload(std::function<bool(const std::string&)>& recompileFinished, std::function<bool(const std::string&)>& recompile) {
		CORE_INFO("Reloading Assembly: {0}", m_assembly);

		for (auto object : m_objects) {
			object->m_assembly = nullptr;
		}

		mono_domain_set(m_monoRootDomain, 0);
		mono_domain_unload(m_monoScriptDomain);



		//delete m_assembly but safe a copy if build fails
		if(std::filesystem::exists(m_assembly + ".temp"))
			std::filesystem::remove(m_assembly + ".temp");

		std::filesystem::copy_file(m_assembly,m_assembly+".temp");
		std::filesystem::remove(m_assembly);
		
		//recompile c# code with postbuildcommands to move files
		CORE_INFO("Recompiling {0}", m_assembly);
		if (recompile(m_assembly)) {
			CORE_ERROR("Recompiling failed: {0}", m_assembly);
			CORE_WARN("Using old Assembly");
			std::filesystem::copy_file(m_assembly + ".temp", m_assembly);
			std::filesystem::remove(m_assembly + ".temp");
		}
		while (true) {
			if (recompileFinished(m_assembly)) {
				if (std::filesystem::exists(m_assembly)) {
					std::filesystem::remove(m_assembly + ".temp");
					CORE_INFO("Recompiling finished");
					break;
				}
				else {
					CORE_ERROR("File not found: {0}", m_assembly);
					CORE_WARN("Using old Assembly");
					std::filesystem::copy_file(m_assembly + ".temp", m_assembly);
					std::filesystem::remove(m_assembly + ".temp");
					break;
				}
				
			}
		}
		//load assembly


		CORE_INFO("Reloading Assembly: {0}", m_assembly);
		m_monoScriptDomain = mono_domain_create_appdomain(constStringToCharPtr(m_assembly), NULL);
		if (!m_monoScriptDomain) {
			CORE_ERROR("Mono Scripting Domain creation failed for {0}", m_assembly);
			return;
		}

		mono_domain_set(m_monoScriptDomain, 0);

		m_monoAssembly = mono_domain_assembly_open(mono_domain_get(), m_assembly.c_str());
		if (!m_monoAssembly) {
			CORE_ERROR("Mono Assembly creation failed for {0}", m_assembly);
			return;
		}
		m_monoImage = mono_assembly_get_image(m_monoAssembly);
		if (!m_monoImage) {
			CORE_ERROR("Mono Image creation failed for {0}", m_assembly);
			return;
		}
		CORE_INFO("Reloading Assembly finished")


		for (auto object : m_objects) {
			object->m_assembly = this;
			object->reload();
		}
	}

}