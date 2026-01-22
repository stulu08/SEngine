#include "st_pch.h"
#include "OpenGLTextureCubeMap.h"

#include <glad/glad.h>
#include <stb_image.h>
#include <Stulu/Resources/Resources.h>
#include <Stulu/Renderer/RenderCommand.h>
#include <Stulu/Renderer/Renderer.h>
#include "OpenGlTexture.h"
#include "OpenGLStateCache.h"

namespace Stulu {
	OpenGLCubeMap::OpenGLCubeMap(uint32_t resolution, TextureSettings settings)
		: m_resolution(resolution), m_settings(settings) {
		glGenTextures(1, &m_map);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_map);

		auto [internalformat, dataformat] = TextureFormatToGLenum(m_settings.format);

		for (uint32_t i = 0; i < 6; i++)
		{
			if (isGLTextureFormatFloat(m_settings.format))
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, m_resolution, m_resolution, 0, dataformat, GL_FLOAT, nullptr);
			else
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, m_resolution, m_resolution, 0, dataformat, GL_UNSIGNED_BYTE, nullptr);
		}
		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenumMinification(m_settings.filtering));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenumMagnification(m_settings.filtering));

		if (m_settings.levels > 1)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	OpenGLCubeMap::OpenGLCubeMap(uint32_t resolution, const std::vector<std::string>& faces, TextureSettings settings)
		: m_resolution(resolution), m_settings(settings) {

		glGenTextures(1, &m_map);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_map);


		stbi_set_flip_vertically_on_load(false);
		for (uint32_t i = 0; i < faces.size(); i++) {
			auto& path = faces[i];
			void* textureData = nullptr;
			bool isFloatData = false;
			int32_t width, height, channels;
			if (isTextureFileFloat(path.c_str())) {
				textureData = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
				isFloatData = true;
			}
			else {
				textureData = stbi_load(path.c_str(), &width, &height, &channels, 0);
			}

			CORE_ASSERT(textureData, std::string("Texture failed to load: ") + faces[i]);

			if (!isCorrectFormat(m_settings.format, channels, faces[i].c_str())) {
				CORE_ERROR("Texture format not correct for texture {0}", faces[i]);
			}
			auto [internalformat, dataformat] = TextureFormatToGLenum(m_settings.format);

			if (isFloatData)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, dataformat, GL_FLOAT, textureData);
			else
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalformat, width, height, 0, dataformat, GL_UNSIGNED_BYTE, textureData);

			stbi_image_free(textureData);
		}

		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenumMinification(m_settings.filtering));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenumMagnification(m_settings.filtering));

		if (m_settings.levels > 1)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	
	}

	OpenGLCubeMap::~OpenGLCubeMap() {
		glDeleteTextures(1, &m_map);
	}

	void OpenGLCubeMap::bind(uint32_t slot) const {
		OpenGLStateCache::BindTextureUnit(slot, m_map);
	}

	void OpenGLCubeMap::GenerateMips() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_map);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	bool OpenGLCubeMap::operator==(const Texture& other) const {
		return m_map == *static_cast<uint32_t*>(other.getNativeRendererObject());
	}
}