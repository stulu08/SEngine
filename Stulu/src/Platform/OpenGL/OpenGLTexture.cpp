#include "st_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Stulu {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;

		GLenum internalFormat = GL_RGBA8;
		m_dataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_path(path) {

		ST_PROFILING_FUNCTION();
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* textureData;
		{
			ST_PROFILING_SCOPE("reading file - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			textureData = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		CORE_ASSERT(textureData, "Texture failed to load: {0}",path);
		m_width = width;
		m_height = height;
		m_dataFormat = 0;
		GLenum internalFormat = 0;
		switch (channels) {
		case 4:
			internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
			break;
		case 3:
			internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			break;
		}
		CORE_ASSERT(internalFormat, "Texture channel not supported: {0}", path);
		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, textureData);

		stbi_image_free(textureData);
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		ST_PROFILING_FUNCTION();
		int32_t active_id = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_id);
		if (active_id != m_rendererID) {
			glBindTextureUnit(slot, m_rendererID);
		}
	}
	void OpenGLTexture2D::setData(void* data, uint32_t size) const{
		ST_PROFILING_FUNCTION();

		uint32_t pixelSize = m_dataFormat == GL_RGBA ? 4 : 3;
		CORE_ASSERT(size == m_width * m_height * pixelSize, "Data is not entire Texture");
		glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}

	uint32_t OpenGLTexture2D::getRendererID() const {
		return m_rendererID;
	}
	bool OpenGLTexture2D::operator==(const Texture& other) const {
		return m_rendererID == other.getRendererID();
	}
	////////////////////////////////////////////



	OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>& faces) {
		ST_PROFILING_FUNCTION();
		glGenTextures(1, &m_rendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		for (uint32_t i = 0; i < faces.size(); i++) {
			GLenum type = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
			stbi_uc* textureData;
			{
				ST_PROFILING_SCOPE("reading file - OpenGLCubeMap::OpenGLCubeMap(const std::vector<std::string>&)");
				textureData = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
			}
			CORE_ASSERT(textureData, "Texture failed to load: {0}", faces[i].c_str());
			glTexImage2D(type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	OpenGLCubeMap::~OpenGLCubeMap() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLCubeMap::bind(uint32_t slot) const {
		ST_PROFILING_FUNCTION();
		int32_t active_id = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &active_id);
		if (active_id != m_rendererID) {
			glBindTextureUnit(slot, m_rendererID);
		}
	}
	uint32_t OpenGLCubeMap::getRendererID() const {
		return m_rendererID;
	}
	bool OpenGLCubeMap::operator==(const Texture& other) const {
		return m_rendererID == other.getRendererID();
	}
}