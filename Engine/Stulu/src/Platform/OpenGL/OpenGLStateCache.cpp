#include "st_pch.h"
#include "OpenGLStateCache.h"
#include <glad/glad.h>

namespace Stulu {
#include "OpenGLStateCache.h"

	OpenGLStateCache::StateCache OpenGLStateCache::s_cache;

	void OpenGLStateCache::Reset() {
		s_cache = {};
		BindProgram(0);
		BindVertexArray(0);
		for (uint32_t i = 0; i < 32; ++i) {
			BindTextureUnit(i, 0);
		}
		SetCullFace(GL_BACK);
		StencilMask(0x00);
	}

	void OpenGLStateCache::BindProgram(uint32_t id) {
		if (s_cache.program != id) {
			s_cache.program = id;
			glUseProgram(id);
		}
	}

	void OpenGLStateCache::BindTextureUnit(uint32_t slot, uint32_t id) {
		if (s_cache.textures[slot] != id) {
			s_cache.textures[slot] = id;
			glBindTextureUnit(slot, id);
		}
	}

	void OpenGLStateCache::BindVertexArray(uint32_t id) {
		if (s_cache.vertexArray != id) {
			s_cache.vertexArray = id;
			glBindVertexArray(id);
		}
	}

	void OpenGLStateCache::BindUniformBuffer(uint32_t slot, uint32_t id) {
		if (slot >= s_cache.uniformBuffers.size()) return;
		if (s_cache.uniformBuffers[slot] != id) {
			s_cache.uniformBuffers[slot] = id;
			glBindBufferBase(GL_UNIFORM_BUFFER, slot, id);
		}
	}

	void OpenGLStateCache::BindStorageBuffer(uint32_t slot, uint32_t id) {
		if (slot >= s_cache.storageBuffers.size()) return;
		if (s_cache.storageBuffers[slot] != id) {
			s_cache.storageBuffers[slot] = id;
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, id);
		}
	}

	void OpenGLStateCache::StencilMask(uint8_t mask) {
		if (s_cache.stencilMask != mask) {
			s_cache.stencilMask = mask;
			glStencilMask(mask);
		}
	}


	void OpenGLStateCache::SetCullFace(uint32_t face) {
		if (s_cache.cullFace != face) {
			s_cache.cullFace = face;

			if (face == GL_FRONT_AND_BACK) {
				glDisable(GL_CULL_FACE);
			}
			else {
				glEnable(GL_CULL_FACE);
				glCullFace(face);
			}
		}
	}

}