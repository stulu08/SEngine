#include "st_pch.h"
#include "ScriptAssembly.h"

#include "MonoObjectInstance.h"
#include <mono/metadata/image.h>
#include <mono/metadata/metadata.h>

namespace Stulu {

	void ScriptAssembly::LoadAssembly(const std::string& assembly) {
		//m_assembly = m_domain.OpenAssembly(assembly);
		std::ifstream stream(assembly, std::ios::in | std::ios::binary | std::ios::ate);
		if (!stream.is_open()) {
			CORE_ERROR("File not found: {0}", assembly);
		}

		size_t size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		char* buffer = new char[size];
		stream.read(buffer, size);
		stream.close();

		Mono::Image::Status status;
		Mono::Image tempImage = Mono::Image::OpenFromDataFull(buffer, (uint32_t)size, true, &status, false);

		if (status != Mono::Image::Status::OK) {
			CORE_ERROR("Mono error: {0}", Mono::Image::GetStatusError(status));
		}

		m_assembly = Mono::Assembly::LoadFromFull(tempImage, assembly, &status, false);

		if (status != Mono::Image::Status::OK) {
			CORE_ERROR("Mono error: {0}", Mono::Image::GetStatusError(status));
		}

		tempImage.Close();

		delete[] buffer;
	}

	void ScriptAssembly::Load(const std::string& assembly) {
		CORE_INFO("Loading Assembly: {0}", assembly);

		LoadAssembly(assembly);

		if (!m_assembly) {
			CORE_ERROR("Mono Assembly creation failed for {0}", m_assembly);
			return;
		}
		m_image = m_assembly.GetImage();
		if (!m_image) {
			CORE_ERROR("Mono Image creation failed for {0}", m_assembly);
			return;
		}

		for (auto [id, object] : m_objects) {
			if (object) {
				object->m_objectID = id;
				object->m_assembly = this;
				object->Reload();
			}
		}
	}

	void ScriptAssembly::Unload() {
		CORE_INFO("Unloading Assembly");
		for (auto [id, object] : m_objects) {
			object->BackupFields();
			object->m_assembly = nullptr;
		}
	}

	Mono::Class ScriptAssembly::CreateClass(const std::string& m_nameSpace, const std::string& m_className) const {
		Mono::Class classPtr = Mono::Class::FromName(m_image, m_nameSpace.c_str(), m_className.c_str());
		if (classPtr) {
			return classPtr;
		}
		CORE_ERROR("Could not create Mono::Class from {0}.{1}", m_nameSpace, m_className);
		return classPtr;
	}

	Mono::Method ScriptAssembly::CreateMethod(Mono::Class classPtr, const std::string& methodName) {
		if (classPtr) {
			const std::string nameSpace = classPtr.GetNamespace();
			const std::string className = classPtr.GetName();
			const std::string classStr = nameSpace + "." + className + ":" + methodName;

			Mono::MethodDesc desc = Mono::MethodDesc::New(classStr, true);
			if (desc) {
				Mono::Method method = desc.SearchInClass(classPtr);
				desc.Free();
				if (method) {
					return method;
				}
			}
			CORE_ERROR("Could not create Mono::Method from {0}", classStr);
		}
		else {
			CORE_ERROR("Invalid Class");
		}

		return nullptr;
	}

	Mono::Object ScriptAssembly::InvokeMethod(Mono::Method method, void* obj, void** args) {
		if (!method)
			return nullptr;

		MonoObject* ex = nullptr;
		Mono::Object re = nullptr;

		re = Mono::RuntimeInvoke(method, obj, args, &ex);

		if (ex) {
			Mono::Object exception = ex;
			std::string name = exception.GetClass().GetName();
			Mono::String message = exception.ToString();
			CORE_ERROR("{0} in Managed Code: {1}", name, Mono::String(message).ToUtf8());
		}
		return re;
	}

	std::vector<Mono::Class> ScriptAssembly::LoadAllClasses(Mono::Class parentClass) {
		std::vector<Mono::Class> classes;
		if (!parentClass)
			return classes;

		const MonoTableInfo* table_info = mono_image_get_table_info(m_image, MONO_TABLE_TYPEDEF);

		int rows = mono_table_info_get_rows(table_info);
		for (int i = 0; i < rows; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
			const char* name = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAME]);
			const char* name_space = mono_metadata_string_heap(m_image, cols[MONO_TYPEDEF_NAMESPACE]);

			Mono::Class _class = Mono::Class::FromName(m_image, name_space, name);

			if (!_class || _class.IsEnum())
				continue;

			Mono::Class parent = _class.GetParent();
			if (!parent)
				continue;

			if (parent == parentClass)
				classes.push_back(_class);
		}
		return classes;
	}
}