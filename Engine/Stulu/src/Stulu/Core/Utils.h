#pragma once
#include <string>
#include <fstream>

namespace Stulu {
	inline std::string ReadFile(const std::string& path) {
		std::string result;
		std::ifstream inStream(path, std::ios::in | std::ios::binary);

		if (inStream) {
			inStream.seekg(0, std::ios::end);
			result.resize(inStream.tellg());
			inStream.seekg(0, std::ios::beg);
			inStream.read(&result[0], result.size());
			inStream.close();
		}
		else
			CORE_ERROR("Could not open File: {0}", path);

		return result;
	}
	inline bool FileExists(const char* path) {
		if (FILE* file = fopen(path, "r")) {
			fclose(file);
			return true;
		}
		else {
			return false;
		}
	}
	inline bool FileExists(const std::string& path) {
		return FileExists(path.c_str());
	}

	template<class T>
	inline T& NativeRenderObjectCast(void* obj) {
		return *(T*)obj;
	}
	template<class T>
	inline const T& NativeRenderObjectCast(const void* obj) {
		return *(T*)obj;
	}
}