#pragma once
#include "Stulu/Core/Core.h"
#include <cstdint>
#include <array>

namespace Stulu {
	class STULU_API OpenGLStateCache {
	public:
		static void Reset();

		static void BindProgram(uint32_t id);
		static void BindTextureUnit(uint32_t slot, uint32_t id);
		static void BindVertexArray(uint32_t id);
		static void BindUniformBuffer(uint32_t slot, uint32_t id);
		static void BindStorageBuffer(uint32_t slot, uint32_t id);
		static void StencilMask(uint8_t mask);
		static void SetCullFace(uint32_t face);

	private:
		struct StateCache {
			std::array<uint32_t, 32> textures{};
			std::array<uint32_t, 32> uniformBuffers{};
			std::array<uint32_t, 32> storageBuffers{};

			uint32_t program = 0;
			uint32_t framebuffer = 0;
			uint32_t vertexArray = 0;
			uint32_t vertexBuffer = 0;
			uint32_t indexBuffer = 0;
			uint8_t stencilMask = 0;
			uint32_t cullFace = 0;
		};

		static StateCache s_cache;
	};
}
