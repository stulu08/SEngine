#include "st_pch.h"
#include "ShaderSystem.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Resources/Resources.h"

namespace Stulu {
	ShaderSystem::ShaderSystem() {
		m_spirvSupported = CheckSpirv();

		m_compiler = ShaderCompiler::Create();
		m_compiler->AddHeaderFront("#version 450");
		
		AddIncludePath(Resources::EngineDataDir + "/Stulu/Shader");
		AddIncludePath(std::filesystem::current_path().string());
		AddInternalIncludeFile("Stulu/Internals.glsl",
			std::string("#ifndef _STULU_INTERNALS_GLSL_\n") +
			std::string("#define _STULU_INTERNALS_GLSL_\n") + 

			std::string("#define ST_BUFFER_MODEL_BIND  ") + std::to_string((uint32_t)BufferBinding::Model) + "\n" +
			std::string("#define ST_BUFFER_CAMERA_BIND  ") + std::to_string((uint32_t)BufferBinding::Camera) + "\n" +
			std::string("#define ST_BUFFER_POSTPROCESS_BIND  ") + std::to_string((uint32_t)BufferBinding::PostProcessing) + "\n" +
			std::string("#define ST_BUFFER_LIGHT_BIND  ") + std::to_string((uint32_t)BufferBinding::Light) + "\n" +
			std::string("#define ST_BUFFER_SCENE_BIND  ") + std::to_string((uint32_t)BufferBinding::Scene) + "\n" +
			std::string("#define ST_BUFFER_MATERIAL_BIND  ") + std::to_string((uint32_t)BufferBinding::Material) + "\n" +
			std::string("#define ST_BUFFER_USER_MATERIAL_BIND  ") + std::to_string((uint32_t)BufferBinding::UserMaterial) + "\n" +
			std::string("#define ST_USER_MATERIAL_BINDING ") + std::to_string((int)BufferBinding::UserMaterial) + "\n" +

			std::string("#define ST_SKYBOX_TEXTURE_BIND_ENV  ") + std::to_string(ST_SKYBOX_TEXTURE_BIND_ENV) + "\n" +
			std::string("#define ST_SKYBOX_TEXTURE_BIND_IRR  ") + std::to_string(ST_SKYBOX_TEXTURE_BIND_IRR) + "\n" +
			std::string("#define ST_SKYBOX_TEXTURE_BIND_PRE  ") + std::to_string(ST_SKYBOX_TEXTURE_BIND_PRE) + "\n" +
			std::string("#define ST_SKYBOX_TEXTURE_BIND_BRD  ") + std::to_string(ST_SKYBOX_TEXTURE_BIND_BRD) + "\n" +

			std::string("#define ST_MAX_SHADOW_CASCADES ") + std::to_string(ST_MAX_SHADOW_CASCADES) + "\n" +
			std::string("#define ST_DEFAULT_SHADOW_TEXTURE_BIND_MAP  ") + std::to_string(ST_DEFAULT_SHADOW_TEXTURE_BIND_MAP) + "\n" +
			std::string("#define ST_POINT_SHADOW_TEXTURE_BIND_MAP  ") + std::to_string(ST_POINT_SHADOW_TEXTURE_BIND_MAP) + "\n" +
			std::string("#define ST_CASCADE_SHADOW_TEXTURE_BIND_MAP  ") + std::to_string(ST_CASCADE_SHADOW_TEXTURE_BIND_MAP) + "\n" +

			std::string("#define ST_USER_TEXTURE_0  ") + std::to_string(ST_USER_TEXTURE_START + 0) + "\n" +
			std::string("#define ST_USER_TEXTURE_1  ") + std::to_string(ST_USER_TEXTURE_START + 1) + "\n" +
			std::string("#define ST_USER_TEXTURE_2  ") + std::to_string(ST_USER_TEXTURE_START + 2) + "\n" +
			std::string("#define ST_USER_TEXTURE_3  ") + std::to_string(ST_USER_TEXTURE_START + 3) + "\n" +
			std::string("#define ST_USER_TEXTURE_4  ") + std::to_string(ST_USER_TEXTURE_START + 4) + "\n" +
			std::string("#define ST_USER_TEXTURE_5  ") + std::to_string(ST_USER_TEXTURE_START + 5) + "\n" +
			std::string("#define ST_USER_TEXTURE_6  ") + std::to_string(ST_USER_TEXTURE_START + 6) + "\n" +
			std::string("#define ST_USER_TEXTURE_7  ") + std::to_string(ST_USER_TEXTURE_START + 7) + "\n" +
			std::string("#define ST_USER_TEXTURE_8  ") + std::to_string(ST_USER_TEXTURE_START + 8) + "\n" +
			std::string("#define ST_USER_TEXTURE_COUNT ") + std::to_string(ST_USER_TEXTURE_COUNT) + "\n" +
			std::string("#define ST_USER_TEXTURE_START ") + std::to_string(ST_USER_TEXTURE_START) + "\n" +
			std::string("#define ST_USER_TEXTURE_END ") + std::to_string(ST_USER_TEXTURE_END) + "\n" +

			std::string("const int st_maxLights = ") + std::to_string(ST_MAXLIGHTS) + ";\n" +
			std::string("#define ENTITY_ID_NULL 0xffffffff\n") +
			std::string("#define MAX_REFLECTION_LOD ") + std::to_string(ST_MAX_REFLECTION_LOD) + "\n" +
			std::string("#define ST_MAX_INSTANCES ") + std::to_string(ST_MAX_INSTANCES) + "\n" +
			std::string("#define ST_MAX_BONES  ") + std::to_string(ST_MAX_BONES) + "\n" +
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
		std::string source = ProcessShader(path);
		std::string name = GetShaderName(source);
		if (name.empty())
			return nullptr; // no #SShader 

		if (m_shaders.find(name) != m_shaders.end()) {
			CORE_INFO("Shader \"{0}\" already exists, reloading shader", name);
			ReloadShader(name);
			return m_shaders[name]->GetShader();
		}

		auto sources = ProcessRegions(source);
		auto props = ProcessProperties(source);

		return CreateShader(name, sources, props, path);
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

	std::string ShaderSystem::LoadShaderSource(const std::string& fileStr) const {
		// Check internal
		if (m_internalFiles.find(fileStr) != m_internalFiles.end()) {
			return m_internalFiles.at(fileStr);
		}
		// Check includes
		else {
			for (const std::string& path : m_includeDirs) {
				std::string fPath = path + "/" + fileStr;
				if (FileExists(fPath)) {
					return ReadFile(fPath);
				}
			}
		}

		CORE_ERROR("Shader not found: {0}", fileStr);
		return "";
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

		// not loaded by path
		if (entry->m_path.empty())
			return;

		if (!FileExists(entry->m_path)) {
			CORE_ERROR("File not found: \"{0}\"", entry->m_path);
			return;
		}

		std::string source = ProcessShader(entry->m_path);
		std::string newName = GetShaderName(source);

		if (newName != name) {
			CORE_WARN("Shader name changed, removing \"{0}\", adding \"{1}\"", name, newName);
			RemoveShader(name);
			AddShader(newName);
			return;
		}

		if (newName.empty())
			return; // no #SShader 

		auto sources = ProcessRegions(source);
		auto props = ProcessProperties(source);

		ShaderCompileResult compileResult;
		if (!m_compiler->CompileToCache(sources, BuildCacheFile(name), compileResult)) {
			CORE_ERROR("Compilation for shader '{0}' failed!", name);
			CORE_ASSERT(false, "");
			return;
		}

		m_shaders[name]->m_props = props;
		m_shaders[name]->m_shader->reload(compileResult);
	}
	bool ShaderSystem::CheckSpirv() {
		if (Renderer::getRendererAPI() == Renderer::API::Vulkan) {
			return true;
		}

		if (Renderer::getRendererAPI() == Renderer::API::OpenGL) {
			auto api = Application::get().getWindow().getContext()->getApiInfos();
			// intel integrated gpu's have a problem with loading sprv, despite having support for the extension
			return !(api.vendor.rfind("Intel", 0) == 0);
		}
		return false;
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


	Ref<Shader> ShaderSystem::CreateShader(const std::string& name, const ShaderSource& sources, const std::vector<Ref<MaterialProperty>>& properties, const std::string& path) {
		ShaderCompileResult compileResult;
		if (!m_compiler->CompileToCache(sources, BuildCacheFile(name), compileResult)) {
			CORE_ERROR("Compilation for shader '{0}' failed!", name);
			CORE_ASSERT(false, "");
			// Delete and retype following to copy shader files after build: B====D
			return nullptr;
		}

		auto shader = Shader::create(name, compileResult);
		
		m_shaders.insert({ name, createRef<ShaderEntry>(shader, properties, path.empty() ? name : path)});
		return m_shaders[name]->GetShader();
	}

	std::vector<Ref<MaterialProperty>> ShaderSystem::ProcessProperties(std::string& source) const {
		std::vector<Ref<MaterialProperty>> props;
		const char* token = "#Expose ";

		size_t offset = 0;
		for (size_t pos = source.find(token); pos != source.npos; pos = source.find(token, pos + 1)) {
			if (source[pos - 1] != '\n' && source[pos - 1] != '\r')
				continue;

			const size_t maxEnd = source.find("\n\r", pos);
			const size_t begin = pos + strlen(token);

			const size_t typeStrBegin = source.find_first_not_of(" ", begin);
			const size_t typeStrEnd = source.find_first_of(" (", typeStrBegin);

			const size_t firstBracket = source.find('(', begin);
			const size_t lastBracket = source.find(')', firstBracket);

			if (typeStrEnd >= maxEnd || firstBracket >= maxEnd || lastBracket >= maxEnd) {
				const size_t maxNum = glm::min((size_t)15, maxEnd - pos);
				CORE_ERROR("Error while parsing shader at: {0}", source.substr(pos, maxNum));
				CORE_ASSERT(false, "Syntax error");
				continue;
			}

			const std::string typeStr = source.substr(typeStrBegin, typeStrEnd - typeStrBegin);
			const std::string constructValues = source.substr(firstBracket + 1, lastBracket - firstBracket - 1);
			
			MaterialPropertyType type = MaterialProperty::TypeFromString(typeStr);
			if (type != MaterialPropertyType::None) {

				size_t compCount = MaterialProperty::GetComponentCount(type);
				if (compCount != 0) {
					size_t baseAlign = Std140BaseAlignment(compCount);
					offset = Std140Align(offset, baseAlign);
				}
				

				auto prop = MaterialProperty::Construct(type, constructValues, offset);
				if (prop) {
					props.push_back(prop);
				}
				else {
					CORE_ERROR("Could not create '{0}' property for shader!", typeStr);
				}

				if (compCount != 0) {
					size_t size = Std140Size(compCount);
					offset += size;
				}
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

	ShaderEntry::ShaderEntry(const Ref<Shader>& shader, const std::string& path)
		: m_shader(shader), m_props(), m_path(path) {
	}

	ShaderEntry::ShaderEntry(const Ref<Shader>& shader, const std::vector<Ref<MaterialProperty>>& props, const std::string& path)
		: m_shader(shader), m_props(props), m_path(path) {
	}
	Ref<MaterialProperty> ShaderEntry::GetProperity(std::string name) const {
		for (auto& prop : m_props) {
			if(prop->GetName() == name){
				return prop;
			}
		}
		return nullptr;
	}
	bool ShaderEntry::HasProperity(std::string name) const {
		for (auto& prop : m_props) {
			if (prop->GetName() == name) {
				return true;
			}
		}
		return false;
	}

	std::string ShaderSystem::ProcessShader(const std::string& filename) const {
		ShaderSystem::ShaderIncludeNode node = ParseShaderIncludeTree(filename);
		
		std::stringstream stream;
		FlattenShaderInclude(node, stream);
		return stream.str();
	}

	ShaderSystem::ShaderIncludeNode ShaderSystem::ParseShaderIncludeTree(const std::string& filepath) const {
		ShaderIncludeNode node;
		node.filename = filepath;
		
		std::string source = LoadShaderSource(filepath);
		if (source.empty()) {
			node.filename = ":<file not found>";
			return node;
		}

		std::istringstream ss(source);
		std::string line;
		size_t lineIndex = 0;

		while (std::getline(ss, line)) {
			if (line.find("#include ") != std::string::npos) {
				size_t begin = line.find('\"');
				size_t end = line.find('\"', begin + 1);
				if (begin == std::string::npos || end == std::string::npos) {
					CORE_ERROR("Malformed #include in {0} at line {1}", filepath, lineIndex + 1);
					continue;
				}

				std::string includePath = line.substr(begin + 1, end - begin - 1);
				ShaderIncludeNode childNode = ParseShaderIncludeTree(includePath);

				// Mark this line as a placeholder for include
				node.includes.emplace_back(lineIndex, std::move(childNode));
				node.lines.push_back(""); // placeholder
			}
			else {
				node.lines.push_back(line);
			}
			++lineIndex;
		}
		return node;
	}
	void ShaderSystem::FlattenShaderInclude(const ShaderIncludeNode& node, std::ostream& out, const std::string& indent) const {
		size_t lineCount = 0;
		size_t includeIdx = 0;

		for (size_t i = 0; i < node.lines.size(); ++i) {
			// Is this line an include marker?
			if (includeIdx < node.includes.size() && node.includes[includeIdx].first == i) {
				const ShaderIncludeNode& child = node.includes[includeIdx].second;

				out << indent << "/* Begin Include \"" << child.filename << "\" */\n";
				out << indent << "#line 1 \"" << child.filename << "\"\n";
				FlattenShaderInclude(child, out, indent);
				out << indent << "#line " << (lineCount + 3) << " \"" << node.filename << "\"\n";
				out << indent << "/* End Include \"" << child.filename << "\" */\n";

				includeIdx++;
				continue;
			}

			out << indent << node.lines[i] << "\n";
			++lineCount;
		}
	}
}