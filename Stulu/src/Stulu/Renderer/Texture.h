#pragma once
#include "Stulu/Core/Core.h"
#include <string>
#include <glm/glm.hpp>

namespace Stulu {
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void bind(uint32_t slot = 0) const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
	};
	class Texture2D : public Texture {
	public:
		static Ref<Texture2D> create(const std::string& path);

		glm::vec2 tiling = { 1.0f,1.0f };
	};
	class CubeMap : public Texture {
	public:
		static Ref<CubeMap> create(const std::vector<std::string>& faces);
	};
}