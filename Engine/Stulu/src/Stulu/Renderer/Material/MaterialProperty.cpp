#include "st_pch.h"
#include "MaterialProperty.h"

namespace Stulu {
	MaterialPropertyType MaterialProperty::TypeFromString(std::string type) {
		std::transform(type.begin(), type.end(), type.begin(), [](char c) { return std::tolower(c); });

		if (type == "float")
			return MaterialPropertyType::Float;
		if (type == "float2")
			return MaterialPropertyType::Float2;
		if (type == "float3")
			return MaterialPropertyType::Float3;
		if (type == "float4")
			return MaterialPropertyType::Float4;
		if (type == "int")
			return MaterialPropertyType::Int;
		if (type == "int2")
			return MaterialPropertyType::Int2;
		if (type == "int3")
			return MaterialPropertyType::Int3;
		if (type == "int4")
			return MaterialPropertyType::Int4;
		if (type == "uint")
			return MaterialPropertyType::UInt;
		if (type == "sampler2d")
			return MaterialPropertyType::Sampler2D;
		if (type == "cubemap")
			return MaterialPropertyType::SkyBox;
		if (type == "color")
			return MaterialPropertyType::Color;

		return MaterialPropertyType::None;
	}

	Texture* MaterialProperty::GetDefaultTexture(DefaultMaterialTexture m_defaultTexture) {
		switch (m_defaultTexture)
		{
		case Stulu::DefaultMaterialTexture::Black:
			return Resources::BlackTexture();
		case Stulu::DefaultMaterialTexture::White:
			return Resources::WhiteTexture();
		}
		return Resources::BlackTexture();
	}

	size_t MaterialProperty::GetComponentCount(MaterialPropertyType type)
	{
		switch (type)
		{
		case Stulu::MaterialPropertyType::Float:
		case Stulu::MaterialPropertyType::Int:
		case Stulu::MaterialPropertyType::UInt:
		case Stulu::MaterialPropertyType::Sampler2D:
		case Stulu::MaterialPropertyType::SkyBox:
			return 1;

		case Stulu::MaterialPropertyType::Float2:
		case Stulu::MaterialPropertyType::Int2:
			return 2;

		case Stulu::MaterialPropertyType::Float3:
		case Stulu::MaterialPropertyType::Int3:
			return 3;

		case Stulu::MaterialPropertyType::Float4:
		case Stulu::MaterialPropertyType::Int4:
		case Stulu::MaterialPropertyType::Color:
			return 4;

		case Stulu::MaterialPropertyType::None:
			return 0;
		}
		return 0;
	}

	static size_t ParseSizeT(std::string& str) {
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

		if (str == "auto_align")
			return 0;
		else {
			try {
				return std::stoull(str);
			}
			catch (std::exception ex) {
				CORE_ERROR("Error parsing offset: {0}", ex.what());
			}
		}
		return 0;
	}
	static float ParseFloat(std::string& str) {
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		try {
			return std::stof(str);
		}
		catch (std::exception ex) {
			CORE_ERROR("Error parsing offset: {0}", ex.what());
		}
		return 0.0f;
	}
	static uint32_t ParseUInt(std::string& str) {
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		try {
			return std::stoul(str);
		}
		catch (std::exception ex) {
			CORE_ERROR("Error parsing offset: {0}", ex.what());
		}
		return 0;
	}
	static bool ParseBool(std::string& str) {
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		std::transform(str.begin(), str.end(), str.begin(), [](char c) { return std::tolower(c); });
		return str == "true";
	}

	Ref<MaterialProperty> MaterialProperty::Construct(MaterialPropertyType type, const std::string& args, size_t expectedOffset) {
		// split into arguments
		std::vector<std::string> segments;
		{
			std::stringstream stream(args);
			std::string segment;
			while (std::getline(stream, segment, ',')) {
				segments.push_back(segment);
			}
		}

		// needs (name, offset, ...) at least
		if (segments.size() < 2) {
			CORE_ERROR("To less arguments for ShaderProperity");
			return nullptr;
		}


		// find name, "Name" ,
		const std::string& nameSeg = segments[0];
		const size_t nameFirstPos = nameSeg.find_first_of('"');
		const size_t nameLastPos = nameSeg.find_last_of('"');
		if (nameFirstPos == nameLastPos || nameLastPos == nameSeg.npos) {
			CORE_ERROR("Could not parse name for shader property");
			return nullptr;
		}
		std::string name = nameSeg.substr(nameFirstPos + 1, nameLastPos - nameFirstPos - 1);

		// read the offset/binding
		size_t offset = ParseSizeT(segments[1]);
		if (offset == 0)
			offset = expectedOffset;
		else if (offset != expectedOffset)
			CORE_WARN("Offset of Material will be overwritten!");

		switch (type)
		{
		case Stulu::MaterialPropertyType::Float:
			{
				float min = 0.0f, max = 0.0f;
				if (segments.size() >= 3)
					min = ParseFloat(segments[2]);
				if (segments.size() >= 4)
					max = ParseFloat(segments[3]);
				return createRef<MaterialFloatProperty>(0.0f, name, offset, min, max);
			}

		case Stulu::MaterialPropertyType::Color:
			{
				bool hdr = false;
				if (segments.size() >= 3)
					hdr = ParseBool(segments[2]);
				return createRef<MaterialColorProperty>(glm::vec4(1.0f), name, offset, hdr);
			}
		case Stulu::MaterialPropertyType::Float2:
			return createRef<MaterialFloat2Property>(glm::vec2(0.0f), name, offset);
		case Stulu::MaterialPropertyType::Float3:
			return createRef<MaterialFloat3Property>(glm::vec3(0.0f), name, offset);
		case Stulu::MaterialPropertyType::Float4:
			return createRef<MaterialFloat4Property>(glm::vec4(0.0f), name, offset);
		case Stulu::MaterialPropertyType::Int:
			return createRef<MaterialIntProperty>(0, name, offset);
		case Stulu::MaterialPropertyType::Int2:
			return createRef<MaterialInt2Property>(glm::i32vec2(0), name, offset);
		case Stulu::MaterialPropertyType::Int3:
			return createRef<MaterialInt2Property>(glm::i32vec3(0), name, offset);
		case Stulu::MaterialPropertyType::Int4:
			return createRef<MaterialInt2Property>(glm::i32vec4(0), name, offset);
		case Stulu::MaterialPropertyType::UInt:
			return createRef<MaterialUIntProperty>(0u, name, offset);

		case Stulu::MaterialPropertyType::Sampler2D:
			{
				uint32_t binding = 0;
				if (segments.size() >= 3) binding = ParseUInt(segments[2]);
				else CORE_ERROR("Missing binding slot for Texture Property inside shader, assuming 0");

				DefaultMaterialTexture texture = DefaultMaterialTexture::Black;
				if (segments.size() >= 4) {
					std::string& str = segments[3];
					str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
					std::transform(str.begin(), str.end(), str.begin(), [](char c) { return std::tolower(c); });
					texture = str == "white" ? DefaultMaterialTexture::White : DefaultMaterialTexture::Black;
				}
				return createRef<MaterialSampler2DProperty>(nullptr, name, binding, offset, texture);
			}
		case Stulu::MaterialPropertyType::SkyBox:
			{
				uint32_t binding = 0;
				if (segments.size() >= 3) binding = ParseUInt(segments[2]);
				else CORE_ERROR("Missing binding slot for Texture Property inside shader, assuming 0");

				DefaultMaterialTexture texture = DefaultMaterialTexture::Black;
				if (segments.size() >= 4) {
					std::string& str = segments[3];
					str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
					std::transform(str.begin(), str.end(), str.begin(), [](char c) { return std::tolower(c); });
					texture = str == "white" ? DefaultMaterialTexture::White : DefaultMaterialTexture::Black;
				}
				return createRef<MaterialSamplerSkyBoxProperty>(nullptr, name, binding, offset, texture);
			}
		}
		return nullptr;
	}
}