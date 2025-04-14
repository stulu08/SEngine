#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include "Stulu/Core/Log.h"

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
	inline std::string CleanPath(std::string& newString) {
		std::transform(newString.begin(), newString.end(), newString.begin(), [](char c) { return c == '\\' ? '/' : c; });
		return newString;
	}
	inline std::string CleanPath(const std::string& string) {
		std::string newString = string;
		std::transform(newString.begin(), newString.end(), newString.begin(), [](char c) { return c == '\\' ? '/' : c; });
		return newString;
	}
	inline std::vector<std::string> SplitString(const std::string& input, char delimiter = '@') {
		std::vector<std::string> result;
		std::stringstream ss(input);
		std::string item;

		while (std::getline(ss, item, delimiter)) {
			result.push_back(item);
		}

		return result;
	}
}