#include "st_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Stulu {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_path(path) {

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* textureData = stbi_load(path.c_str(), &width, &height, &channels,0);
		CORE_ASSERT(textureData, "Texture failed to load: {0}",path);
		m_width = width;
		m_height = height;
		GLenum internalFormat = 0, dataFormat = 0;
		switch (channels) {
		case 4:
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		case 3:
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			break;
		}
		CORE_ASSERT(internalFormat, "Texture channel not supported: {0}", path);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, textureData);

		stbi_image_free(textureData);
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_rendererID);
	}
	////////////////////////////////////////////



	OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>& faces) {
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		for (uint32_t i = 0; i < faces.size(); i++) {
			stbi_uc* textureData = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
			CORE_ASSERT(textureData, "Texture failed to load: {0}", faces[i].c_str());
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
		}
		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	OpenGLCubeMap::~OpenGLCubeMap() {
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLCubeMap::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_rendererID);
	}
}