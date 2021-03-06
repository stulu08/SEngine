#pragma once
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include <Stulu/Renderer/Texture.h>
#include "Stulu/Scene/Material.h"
#include "Stulu/Core/UUID.h"
namespace Stulu {
	class AssetsManager;
}
namespace YAML {

	template<>
	struct convert<glm::vec2> {
		inline static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec3> {
		inline static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::vec4> {
		inline static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
	template<>
	struct convert<glm::mat4> {
		inline static Node encode(const glm::mat4& rhs) {
			Node node;
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					node.push_back(rhs[x][y]);
				}
			}
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::mat4& rhs) {
			if (!node.IsSequence() || node.size() != 16)
				return false;
			int i = 0;
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					rhs[x][y] = node[i].as<float>();
					i++;
				}
			}
			return true;
		}
	};
	template<>
	struct convert<Stulu::TextureSettings> {
		inline static Node encode(const Stulu::TextureSettings& rhs) {
			Node node;
			node.push_back(rhs.format);
			node.push_back(rhs.wrap);
			node.push_back(rhs.tiling);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Stulu::TextureSettings& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.format = node[0].as<int>();
			rhs.wrap = node[1].as<int>();
			rhs.tiling = node[2].as<glm::vec2>();
			return true;
		}
	};
	template<>
	struct convert<Stulu::MaterialTexture> {
		inline static Node encode(const Stulu::MaterialTexture& rhs) {
			Node node;
			node.push_back(rhs.binding);
			node.push_back((uint64_t)rhs.uuid);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Stulu::MaterialTexture& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;
			rhs.binding = node[0].as<int>();
			rhs.uuid = node[1].as<uint64_t>();
			return true;
		}
	};
	template<>
	struct convert<Stulu::MaterialDataType> {
		inline static Node encode(const Stulu::MaterialDataType& rhs) {
			Node node;
			node.push_back((int)rhs.type);
			switch (rhs.type) {
				case Stulu::ShaderDataType::Float:			node.push_back(std::any_cast<float>(rhs.data));					break;
				case Stulu::ShaderDataType::Float2:			node.push_back(std::any_cast<glm::vec2>(rhs.data));				break;
				case Stulu::ShaderDataType::Float3:			node.push_back(std::any_cast<glm::vec3>(rhs.data));				break;
				case Stulu::ShaderDataType::Float4:			node.push_back(std::any_cast<glm::vec4>(rhs.data));				break;

				case  Stulu::ShaderDataType::Int:			node.push_back(std::any_cast<int>(rhs.data));					break;
				//case  Stulu::ShaderDataType::Int2:		node.push_back()												break;
				//case  Stulu::ShaderDataType::Int3:		node.push_back()												break;
				//case  Stulu::ShaderDataType::Int4:		node.push_back()												break;

				//case  Stulu::ShaderDataType::Mat3:		node.push_back()												break;
				case  Stulu::ShaderDataType::Mat4:			node.push_back(std::any_cast<glm::mat4>(rhs.data));				break;

				//case  Stulu::ShaderDataType::Bool:		node.push_back()												break;

				case  Stulu::ShaderDataType::Sampler:		node.push_back(std::any_cast<Stulu::MaterialTexture>(rhs.data));	break;
				default:
					CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
			}
			node.push_back(rhs.name);
			node.push_back(rhs.order);

			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Stulu::MaterialDataType& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;
			Stulu::ShaderDataType type = (Stulu::ShaderDataType)node[0].as<int>();
			if (type == Stulu::ShaderDataType::Float) {
				float v = node[1].as<float>();
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Float2) {
				glm::vec2 v = node[1].as<glm::vec2>();
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Float3) {
				glm::vec3 v = node[1].as<glm::vec3>();
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Float4) {
				glm::vec4 v = node[1].as<glm::vec4>();
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Mat4) {
				glm::mat4 v = node[1].as<glm::mat4>();
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Int) {
				int v = node[1].as<int>();				
				rhs.data = v;
			}
			else if (type == Stulu::ShaderDataType::Sampler) {
				Stulu::MaterialTexture v = node[1].as<Stulu::MaterialTexture>();
				rhs.data = v;
			}
			else {
				CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
				return false;
			}


			rhs.type = type;
			rhs.name = node[2].as<std::string>();;
			rhs.order = node[3].as<uint32_t>();
			return true;
		}
	};

}
namespace Stulu {
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (int x = 0; x < 4; x++) {
			for (int y = 0; y < 4; y++) {
				out << v[x][y];
			}
		}
		out << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::TextureSettings& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << v.format << v.wrap << v.tiling;
		out << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::MaterialTexture& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << v.binding << (uint64_t)v.uuid;
		out << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::MaterialDataType& rhs) {
		out << YAML::Flow;
		out << YAML::BeginSeq << (int)rhs.type;
		switch (rhs.type) {
			case Stulu::ShaderDataType::Float:			out << std::any_cast<float>(rhs.data);			break;
			case Stulu::ShaderDataType::Float2:			out << std::any_cast<glm::vec2>(rhs.data);		break;
			case Stulu::ShaderDataType::Float3:			out << std::any_cast<glm::vec3>(rhs.data);		break;
			case Stulu::ShaderDataType::Float4:			out << std::any_cast<glm::vec4>(rhs.data);		break;

			case  Stulu::ShaderDataType::Int:			out << std::any_cast<int>(rhs.data);			break;
			//case  Stulu::ShaderDataType::Int2:		out << 											break;
			//case  Stulu::ShaderDataType::Int3:		out << 											break;
			//case  Stulu::ShaderDataType::Int4:		out << 											break;

			//case  Stulu::ShaderDataType::Mat3:		node.push_back()								break;
			case  Stulu::ShaderDataType::Mat4:			out << std::any_cast<glm::mat4>(rhs.data);		break;

			//case  Stulu::ShaderDataType::Bool:		out << 											break;

			case  Stulu::ShaderDataType::Sampler:		out << std::any_cast<Stulu::MaterialTexture>(rhs.data);	break;
			default:
				CORE_ASSERT(false, "Uknown ShaderDataType or not supported!");
		}
		out << rhs.name << rhs.order << YAML::EndSeq;
		return out;
	}
}