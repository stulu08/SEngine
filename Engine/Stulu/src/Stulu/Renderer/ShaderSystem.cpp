#include "st_pch.h"
#include "ShaderSystem.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Resources.h"

namespace Stulu {
	static ShaderType ShaderTypeFromString(const std::string& type) {
		if (type == "VERTEX" || type == "VERT")
			return ShaderType::Vertex;
		else if (type == "FRAGMENT" || type == "FRAG")
			return ShaderType::Fragment;
		else if (type == "COMPUTE" || type == "COMP")
			return ShaderType::Compute;
		CORE_ERROR("Unknown Shadertype: {0}", type);
		return ShaderType::None;
	}

	ShaderSystem::ShaderSystem() {
		AddIncludePath(Resources::EngineDataDir + "/Stulu/Shader");
		AddInternalIncludeFile("Stulu/Internals.glsl",

			std::string("#ifndef _STULU_INTERNALS_GLSL_\n") +
			std::string("#define _STULU_INTERNALS_GLSL_\n") +
			std::string("#define ST_USER_TEXTURE_0  ") + std::to_string(ST_USER_TEXTURE_START + 0) + "\n" +
			std::string("#define ST_USER_TEXTURE_1  ") + std::to_string(ST_USER_TEXTURE_START + 1) + "\n" +
			std::string("#define ST_USER_TEXTURE_2  ") + std::to_string(ST_USER_TEXTURE_START + 2) + "\n" +
			std::string("#define ST_USER_TEXTURE_3  ") + std::to_string(ST_USER_TEXTURE_START + 3) + "\n" +
			std::string("#define ST_USER_TEXTURE_4  ") + std::to_string(ST_USER_TEXTURE_START + 4) + "\n" +
			std::string("#define ST_USER_TEXTURE_5  ") + std::to_string(ST_USER_TEXTURE_START + 5) + "\n" +
			std::string("#define ST_USER_TEXTURE_6  ") + std::to_string(ST_USER_TEXTURE_START + 6) + "\n" +
			std::string("#define ST_USER_TEXTURE_7  ") + std::to_string(ST_USER_TEXTURE_START + 7) + "\n" +
			std::string("#define ST_USER_TEXTURE_8  ") + std::to_string(ST_USER_TEXTURE_START + 8) + "\n" +
			std::string("#define ST_USER_TEXTURE_9  ") + std::to_string(ST_USER_TEXTURE_START + 9) + "\n" +
			std::string("#define ST_USER_TEXTURE_10 ") + std::to_string(ST_USER_TEXTURE_START + 10) + "\n" +
			std::string("#define ST_USER_TEXTURE_COUNT ") + std::to_string(ST_USER_TEXTURE_COUNT) + "\n" +
			std::string("#define ST_USER_TEXTURE_START ") + std::to_string(ST_USER_TEXTURE_START) + "\n" +
			std::string("#define ST_USER_TEXTURE_END ") + std::to_string(ST_USER_TEXTURE_END) + "\n" +
			std::string("const int st_maxLights = ") + std::to_string(ST_MAXLIGHTS) + ";\n" +
			std::string("#define MAX_REFLECTION_LOD ") + std::to_string(ST_MAX_REFLECTION_LOD) + "\n" +
			std::string("#endif\n")
		);
	}
	ShaderSystem::~ShaderSystem() {

	}

	Ref<Shader> ShaderSystem::AddShader(const std::string& path) {
		if (!FileExists(path)) {
			CORE_ERROR("Cant find shader file: {0}", path);
			return nullptr;
		}
		std::string source = ReadFile(path);
		
		std::string name = GetShaderName(source);
		if (name.empty())
			return nullptr; // no #SShader 

		if (m_shaders.find(name) != m_shaders.end()) {
			CORE_INFO("Shader \"{0}\" already exists, reloading shader", name);
			ReloadShader(name);
			return m_shaders[name]->GetShader();
		}

		ProcessShader(source);
		auto sources = ProcessRegions(source);
		auto props = ProcessProperties(source);

		auto shader = Shader::create(name, sources);

		m_shaders.insert({ name, createRef<ShaderEntry>(shader, props, path) });
		return shader;
	}
	
	void ShaderSystem::LoadAllShaders(const std::string& path) {
		for (auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (!entry.is_regular_file())
				continue;
			auto& path = entry.path();
			if (path.extension() == ".glsl" || path.extension() == ".comp")
				AddShader(path.string());
		}
	}
	
	void ShaderSystem::ReloadShaders() {
		for (auto& [name, entry] : m_shaders) {
			ReloadShader(name);
		}
	}
	void ShaderSystem::ReloadShader(const std::string& name) {
		if (m_shaders.find(name) == m_shaders.end()) {
			CORE_ERROR("Shader \"{0}\" does not exist", name);
			return;
		}
		auto& entry = GetEntry(name);

		if (!FileExists(entry->m_path)) {
			CORE_ERROR("File not found: \"{0}\"", entry->m_path);
			return;
		}

		std::string source = ReadFile(entry->m_path);
		std::string newName = GetShaderName(source);

		if (newName != name) {
			CORE_WARN("Shader name changed, removing \"{0}\", adding \"{1}\"", name, newName);
			RemoveShader(name);
			AddShader(newName);
			return;
		}

		if (newName.empty())
			return; // no #SShader 



		ProcessShader(source);
		auto sources = ProcessRegions(source);
		auto props = ProcessProperties(source);

		m_shaders[name]->m_props = props;
		m_shaders[name]->m_shader->reload(sources);
	}
	std::string ShaderSystem::GetShaderName(const std::string& source) const{
		const std::string shaderToken = "#SShader ";
		if (source.rfind(shaderToken, 0) == 0) {
			const size_t begin = shaderToken.length();
			const size_t end = source.find("\n\r");

			const size_t first = source.find('"', begin);
			const size_t last = source.find('"', first + 1);

			if (first >= end || last >= end) {
				const size_t maxNum = glm::min((size_t)15, end - begin);
				CORE_ERROR("Error while parsing shader name at: {0}", source.substr(begin, maxNum));
				CORE_ASSERT(false, "Error parsing shader name!");
				return "";
			}

			return source.substr(first + 1, last - first - 1);
		}
		else {
			return "";
		}
	}
	void ShaderSystem::ProcessShader(std::string& source) const{
		const char* token = "#include ";
		for (size_t pos = source.find(token); pos != source.npos; pos = source.find(token, pos + 1)) {
			if (source[pos - 1] != '\n' && source[pos - 1] != '\r')
				continue;

			const size_t maxEnd = source.find("\n\r", pos);
			const size_t begin = source.find('\"', pos);
			const size_t end = source.find("\"", begin + 1);

			if (begin >= maxEnd || end >= maxEnd) {
				const size_t maxNum = glm::min((size_t) 15, maxEnd - pos);
				CORE_ERROR("Error while parsing shader at: {0}", source.substr(pos, maxNum));
				CORE_ASSERT(false, "Syntax error");
				return;
			}

			const std::string fileStr = source.substr(begin + 1, end - begin - 1);
			std::string content = "/* Include -> \"" + fileStr + "\" */\n";

			if (m_internalFiles.find(fileStr) != m_internalFiles.end()) {
				content += m_internalFiles.at(fileStr);
			}
			else {
				std::string file = "";
				for (const std::string& path : m_includeDirs) {
					const std::string fPath = path + "/" + fileStr;
					if (FileExists(fPath)) {
						file = fPath;
						break;
					}
				}
				if (file.empty()) {
					CORE_ERROR("File not found: {0}", fileStr);
					CORE_ASSERT(false, "Include error");
					continue;
				}
				content += ReadFile(file);
			}

			source.replace(pos, end - pos + 1, content);
		}
	}

	std::vector<Ref<ShaderProperity>> ShaderSystem::ProcessProperties(std::string& source) const {
		std::vector<Ref<ShaderProperity>> props;
		const char* token = "#properity ";
		for (size_t pos = source.find(token); pos != source.npos; pos = source.find(token, pos + 1)) {
			if (source[pos - 1] != '\n' && source[pos - 1] != '\r')
				continue;

			const size_t maxEnd = source.find("\n\r", pos);
			const size_t begin = pos + strlen(token);

			const size_t typeStrBegin = source.find_first_not_of(" ", begin);
			const size_t typeStrEnd = source.find_first_of(" (", typeStrBegin);

			const size_t firstBracket = source.find('(', begin);
			const size_t lastBracket = source.find(')', firstBracket);

			const size_t nameIndicator = source.find('|', lastBracket);
			const size_t nameBegin = source.find_first_not_of(' ', nameIndicator + 1);
			const size_t nameEnd = source.find_first_of(" \n\r", nameBegin);

			if (typeStrEnd >= maxEnd || firstBracket >= maxEnd || lastBracket >= maxEnd || nameEnd >= maxEnd) {
				const size_t maxNum = glm::min((size_t)15, maxEnd - pos);
				CORE_ERROR("Error while parsing shader at: {0}", source.substr(pos, maxNum));
				CORE_ASSERT(false, "Syntax error");
				continue;
			}

			const std::string typeStr = source.substr(typeStrBegin, typeStrEnd - typeStrBegin);
			const std::string constructValues = source.substr(firstBracket + 1, lastBracket - firstBracket - 1);
			const std::string name = source.substr(nameBegin, nameEnd - nameBegin);

			switch (ShaderProperity::TypeFromString(typeStr))
			{
			case Stulu::ShaderProperity::Type::Range:
				props.push_back(createRef<ShaderProperityRange>(name, constructValues));
				break;
			case Stulu::ShaderProperity::Type::Color:
				props.push_back(createRef<ShaderProperityColor>(name, constructValues));
				break;
			case Stulu::ShaderProperity::Type::Enum:
				props.push_back(createRef<ShaderProperityEnum>(name, constructValues));
				break;
			case Stulu::ShaderProperity::Type::Marker:
				props.push_back(createRef<ShaderProperityMarker>(name, constructValues));
				break;
			default:
				CORE_ERROR("Not able to create ShaderProperity: {0}", name);
				break;
			}
		}
		return props;
	}
	
	ShaderSource ShaderSystem::ProcessRegions(std::string& source) const {
		ShaderSource sources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			std::transform(type.begin(), type.end(), type.begin(), [](char c) -> char { return std::toupper(c); });

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			const std::string shaderSource = (pos == std::string::npos) ?
				source.substr(nextLinePos) :
				source.substr(nextLinePos, pos - nextLinePos);

			const ShaderType typeEnum = ShaderTypeFromString(type);
			sources.Add(typeEnum, shaderSource);
		}
		return sources;
	}
	
	ShaderProperity::Type ShaderProperity::TypeFromString(const std::string& str) {
		if (str == "Color" || str == "color") {
			return Type::Color;
		}
		if (str == "Range" || str == "range") {
			return Type::Range;
		}
		if (str == "Enum" || str == "enum" || str == "Combo" || str == "combo") {
			return Type::Enum;
		}
		if (str == "Marker" || str == "marker" || str == "Help" || str == "help") {
			return Type::Marker;
		}
		CORE_WARN("Uknown Shader Properity type: {0}", str);
		return Type::None;
	}
	
	ShaderProperityRange::ShaderProperityRange(const std::string& name, const std::string& values) {
		m_name = name;
		std::stringstream stream(values);
		std::string segment;
		std::vector<std::string> segments;
		while (std::getline(stream, segment, ','))
		{
			segments.push_back(segment);
		}
		if (segments.size() != 2) {
			CORE_ERROR("Wrong arguments for ShaderProperity Range");
			return;
		}
		m_min = std::stof(segments[0]);
		m_max = std::stof(segments[1]);
	}
	ShaderProperityEnum::ShaderProperityEnum(const std::string& name, const std::string& values) {
		m_name = name;
		std::stringstream stream(values);
		std::string segment;
		while (std::getline(stream, segment, ',')) {
			m_names.push_back(segment);
		}
	}
	ShaderProperityMarker::ShaderProperityMarker(const std::string& name, const std::string& text) {
		m_name = name;
		m_text = text;
		size_t index = 0;
		while (true) {
			index = m_text.find("\\n", index);
			if (index == std::string::npos) break;

			m_text.replace(index, 2, "\n");

			index += 2;
		}
	}
	ShaderProperityColor::ShaderProperityColor(const std::string& name, const std::string& values)
		:m_hdr(false), m_name(name) {
		std::stringstream stream(values);
		std::string arg;
		while (std::getline(stream, arg, ',')) {
			if (arg._Starts_with("hdr") || arg._Starts_with("HDR")) {
				if (arg.find("true"))
					m_hdr = true;
			}
		}
	}

	ShaderEntry::ShaderEntry(const Ref<Shader>& shader, const std::vector<Ref<ShaderProperity>>& props, const std::string& path)
		: m_shader(shader), m_props(props), m_path(path) {
	}
	Ref<ShaderProperity> ShaderEntry::GetProperity(std::string name) const {
		for (auto& prop : m_props) {
			if(prop->getName() == name){
				return prop;
			}
		}
		return nullptr;
	}
	bool ShaderEntry::HasProperity(std::string name) const {
		for (auto& prop : m_props) {
			if (prop->getName() == name) {
				return true;
			}
		}
		return false;
	}

}