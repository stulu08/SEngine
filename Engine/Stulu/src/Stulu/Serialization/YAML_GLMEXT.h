#pragma once
#include <yaml-cpp/yaml.h>
#include "Stulu/Math/Math.h"

namespace YAML {
	template<typename T, glm::length_t L, glm::qualifier Q>
	struct convert<glm::vec<L, T, Q>> {
		inline static Node encode(const glm::vec<L, T, Q>& rhs) {
			Node node;
			for (glm::length_t i = 0; i < L; ++i)
				node.push_back(rhs[i]);

			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::vec<L, T, Q>& rhs) {
			if (!node.IsSequence() || node.size() != L)
				return false;

			for (glm::length_t i = 0; i < L; ++i)
				rhs[i] = node[i].as<T>();

			return true;
		}
	};
	template<>
	struct convert<glm::quat> {
		inline static Node encode(const glm::quat& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		inline static bool decode(const Node& node, glm::quat& rhs) {
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

}
namespace Stulu {
	template<typename T, glm::length_t L, glm::qualifier Q>
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec<L, T, Q>& v) {
		out << YAML::Flow << YAML::BeginSeq;
		for (glm::length_t i = 0; i < L; ++i)
			out << v[i];
		out << YAML::EndSeq;
		return out;
	}
	inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}
}