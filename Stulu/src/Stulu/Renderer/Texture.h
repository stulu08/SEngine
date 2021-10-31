#pragma once
#include "Stulu/Core/Core.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Stulu {
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual void bind(uint32_t slot = 0) const = 0;
		virtual uint32_t getRendererID() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual bool operator == (const Texture & other) const = 0;
		virtual operator int() = 0;
	};
	class Texture2D : public Texture {
	public:
		static Ref<Texture2D> create(const std::string& path);
		static Ref<Texture2D> create(uint32_t width, uint32_t height);
		virtual void setData(void* data, uint32_t size) const = 0;
		glm::vec2 tiling = { 1.0f,1.0f };

		//temp
		virtual std::string getPath() const = 0;
	};
	class CubeMap : public Texture {
	public:
		static Ref<CubeMap> create(uint32_t width, uint32_t height);
		static Ref<CubeMap> create(const std::vector<std::string>& faces);
		static Ref<CubeMap> create(const std::string& cubeMapYamlPath);
	};
}