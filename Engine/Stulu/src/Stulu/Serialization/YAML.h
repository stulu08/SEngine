#pragma once
#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

#include <Stulu/Renderer/Texture.h>
#include "Stulu/Physics/PhysicsMaterial.h"
#include "Stulu/Types/UUID.h"
#include "YAML_GLMEXT.h"
#include <entt.hpp>

namespace Stulu {
	class AssetsManager;
}
namespace YAML {
	template<>
	struct convert<Stulu::UUID> {
		inline static Node encode(const Stulu::UUID& rhs) {
			return convert<uint64_t>::encode((uint64_t)rhs);
		}
		inline static bool decode(const Node& node, Stulu::UUID& rhs) {
			uint64_t id = (uint64_t)rhs;
			bool ret = convert<uint64_t>::decode(node, id);
			rhs = (Stulu::UUID)id;
			return ret;
		}
	};
	template<>
	struct convert<entt::entity> {
		inline static Node encode(const entt::entity& rhs) {
			return convert<ENTT_ID_TYPE>::encode((ENTT_ID_TYPE)rhs);
		}
		inline static bool decode(const Node& node, entt::entity& rhs) {
			ENTT_ID_TYPE id = (ENTT_ID_TYPE)rhs;
			bool ret = convert<ENTT_ID_TYPE>::decode(node, id);
			rhs = (entt::entity)id;
			return ret;
		}
	};
	template<>
	struct convert<Stulu::PhysicsMaterial> {
		inline static Node encode(const Stulu::PhysicsMaterial& rhs) {
			Node node;
			node.push_back(rhs.DynamicFriction);
			node.push_back(rhs.StaticFriction);
			node.push_back(rhs.Restitution);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Stulu::PhysicsMaterial& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.DynamicFriction = node[0].as<float>();
			rhs.StaticFriction = node[1].as<float>();
			rhs.Restitution = node[2].as<float>();
			return true;
		}
	};
	template<>
	struct convert<Stulu::TextureSettings> {
		inline static Node encode(const Stulu::TextureSettings& rhs) {
			Node node;
			node.push_back((uint32_t)rhs.format);
			node.push_back((uint32_t)rhs.wrap);
			node.push_back(rhs.tiling);
			node.push_back((uint32_t)rhs.filtering);
			node.push_back(rhs.border);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, Stulu::TextureSettings& rhs) {
			if (!node.IsSequence())
				return false;
			if (node.size() == 3) {
				rhs.format = (Stulu::TextureFormat)node[0].as<uint32_t>();
				rhs.wrap = (Stulu::TextureWrap)node[1].as<uint32_t>();
				rhs.tiling = node[2].as<glm::vec2>();
			}
			else if (node.size() == 5) {
				rhs.format = (Stulu::TextureFormat)node[0].as<uint32_t>();
				rhs.wrap = (Stulu::TextureWrap)node[1].as<uint32_t>();
				rhs.tiling = node[2].as<glm::vec2>();
				rhs.filtering = (Stulu::TextureFiltering)node[3].as<glm::uint32_t>();
				rhs.border = node[4].as<glm::vec4>();
			}
			else {
				return false;
			}
			
			return true;
		}
	};
}
namespace Stulu {
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::UUID& v) {
		out << (uint64_t)v;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const entt::entity& v) {
		out << (ENTT_ID_TYPE)v;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::PhysicsMaterial& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.DynamicFriction << v.StaticFriction << v.Restitution << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const Stulu::TextureSettings& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << (uint32_t)v.format << (uint32_t)v.wrap << v.tiling << (uint32_t)v.filtering << v.border;
		out << YAML::EndSeq;
		return out;
	}
}