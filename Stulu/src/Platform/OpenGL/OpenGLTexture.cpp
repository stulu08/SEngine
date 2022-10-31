#include "st_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace Stulu {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t openglID, uint32_t width, uint32_t height, const TextureSettings& settings) {
		updateInternal(openglID, width, height, settings);
	}
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings) {
		ST_PROFILING_FUNCTION();
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_hasMips = m_settings.levels > 1 || m_settings.forceGenMips;

		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(m_settings.format);
		m_dataFormat = format.second;
		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, format.first, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 0));
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 1));
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);
		if (m_settings.wrap == TextureWrap::ClampToBorder) {
			float borderColor[] = { m_settings.border.x,  m_settings.border.y,  m_settings.border.z,  m_settings.border.w };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		if (m_hasMips)
			glGenerateMipmap(GL_TEXTURE_2D);
		
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSettings& settings)
		:m_path(path), m_settings(settings) {
		ST_PROFILING_FUNCTION();
		update();
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		ST_PROFILING_FUNCTION();
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_rendererID);
	}
	void OpenGLTexture2D::setData(const void* data, uint32_t size, uint32_t mipLevel) {
		ST_PROFILING_FUNCTION();
		
		//make available for other formats
		//uint32_t pixelSize = m_dataFormat == GL_RGBA ? 4 : 3;
		//CORE_ASSERT(size == m_width * m_height * pixelSize, "Data is not entire Texture");

		glTextureSubImage2D(m_rendererID, mipLevel, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel) {
		//GL_UNSIGNED_INT_8_8_8_8 for a hex value like 0xffff00ff
		glTextureSubImage2D(m_rendererID, mipLevel, posX, posY, 1, 1, m_dataFormat, GL_UNSIGNED_INT_8_8_8_8, &hexData);
	}
	void OpenGLTexture2D::getData(void* data, uint32_t size, uint32_t mipLevel) const {
		glGetTextureImage(m_rendererID, mipLevel, m_dataFormat, GL_UNSIGNED_BYTE, size, data);
	}
	uint32_t OpenGLTexture2D::getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel) const {
		uint32_t data;
		glGetTextureSubImage(m_rendererID, mipLevel, posX, posY, 0, 1, 1, 1, m_dataFormat, GL_UNSIGNED_BYTE, sizeof(uint32_t), &data);
		return data;
	}
	void OpenGLTexture2D::update() {
		ST_PROFILING_FUNCTION();
		if (m_rendererID)
			glDeleteTextures(1, &m_rendererID);
		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		void* textureData = nullptr;
		bool float_ = false;
		{
			ST_PROFILING_SCOPE("reading file - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			if (isGLTextureFormatFloat((TextureFormat)m_settings.format)) {
				textureData = stbi_loadf(m_path.c_str(), &width, &height, &channels, 0);
				float_ = true;
			}
			else {
				textureData = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
			}
			CORE_ASSERT(textureData, "Texture failed to load: {0}", m_path);
		}
		m_width = width;
		m_height = height;


		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(m_settings.format);
		if (!isCorrectFormat(m_settings.format, channels)) {
			CORE_ERROR("Texture format not correct in texture {0}", m_path);
			CORE_WARN("Correcting format to a format that has {0} channels", channels);
			m_settings.format = TextureFormat::Auto;
			format = TextureFormatToGLenum(m_settings.format, channels);
		}

		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);
		GLenum internalFormat = format.first;
		m_dataFormat = format.second;
		m_hasMips = m_settings.levels > 1 || m_settings.forceGenMips;
		


		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, internalFormat, m_width, m_height);

		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 0));
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 1));
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);
		if (m_settings.wrap == TextureWrap::ClampToBorder) {
			float borderColor[] = { m_settings.border.x,  m_settings.border.y,  m_settings.border.z,  m_settings.border.w };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

		if (textureData) {
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_dataFormat, float_ ? GL_FLOAT : GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
		}

		if (m_hasMips)
			glGenerateMipmap(GL_TEXTURE_2D);


	}
	void OpenGLTexture2D::updateParameters() {
		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);
		m_hasMips = m_settings.levels > 1 || m_settings.forceGenMips;
		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 0));
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenum(m_settings.filtering, m_hasMips, 1));
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, wrap);
		if (m_settings.wrap == TextureWrap::ClampToBorder) {
			float borderColor[] = { m_settings.border.x,  m_settings.border.y,  m_settings.border.z,  m_settings.border.w };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
	}
	void OpenGLTexture2D::updateInternal(uint32_t openglID, uint32_t width, uint32_t height, const TextureSettings& settings) {
		m_rendererID = openglID;
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_hasMips = m_settings.levels > 1;
		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(m_settings.format, 3);
		m_dataFormat = format.second;
	}
	bool OpenGLTexture2D::operator==(const Texture& other) const {
		return m_rendererID == *static_cast<uint32_t*>(other.getNativeRendererObject());
	}


	std::pair<uint32_t, uint32_t> TextureFormatToGLenum(TextureFormat& format, int channels) {
		GLenum internalFormat, m_dataFormat;
		switch (format)
		{
		case Stulu::TextureFormat::RGBA:
			internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RGB:
			internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::RG:
			internalFormat = GL_RG8;
			m_dataFormat = GL_RG;
			break;
		case Stulu::TextureFormat::R:
			internalFormat = GL_R8;
			m_dataFormat = GL_RED;
			break;
		case Stulu::TextureFormat::SRGB:
			CORE_WARN("SRGBA Texture are currently not supported");
			internalFormat = GL_RGB8;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::SRGBA:
			CORE_WARN("SRGBA Texture are currently not supported");
			internalFormat = GL_RGBA8;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RGBA16F:
			internalFormat = GL_RGBA16F;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RGB16F:
			internalFormat = GL_RGB16F;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::RGBA32F:
			internalFormat = GL_RGBA32F;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RG32F:
			internalFormat = GL_RG32F;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::Auto:
			switch (channels) {
			case 4:
				format = TextureFormat::RGBA;
				break;
			case 3:
				format = TextureFormat::RGB;
				break;
			case 2:
				format = TextureFormat::RG;
				break;
			case 1:
				format = TextureFormat::R;
				break;
			}
			return TextureFormatToGLenum(format, channels);
		//depth_stencil
		case Stulu::TextureFormat::Depth16:
			internalFormat = GL_DEPTH_COMPONENT16;
			m_dataFormat = GL_DEPTH_COMPONENT;
			break;
		case Stulu::TextureFormat::Depth24:
			internalFormat = GL_DEPTH_COMPONENT24;
			m_dataFormat = GL_DEPTH_COMPONENT;
			break;
		case Stulu::TextureFormat::Depth32:
			internalFormat = GL_DEPTH_COMPONENT32;
			m_dataFormat = GL_DEPTH_COMPONENT;
			break;
		case Stulu::TextureFormat::Depth32F:
			internalFormat = GL_DEPTH_COMPONENT32F;
			m_dataFormat = GL_DEPTH_COMPONENT;
			break;
		case Stulu::TextureFormat::Depth24_Stencil8:
			internalFormat = GL_DEPTH24_STENCIL8;
			m_dataFormat = GL_DEPTH_STENCIL;
			break;
		case Stulu::TextureFormat::Depth32F_Stencil8:
			internalFormat = GL_DEPTH32F_STENCIL8;
			m_dataFormat = GL_DEPTH_STENCIL;
			break;
		case Stulu::TextureFormat::Stencil8:
			internalFormat = GL_STENCIL_INDEX8;
			m_dataFormat = GL_STENCIL_INDEX;
			break;
		}
		return { internalFormat,m_dataFormat };
	}
	uint32_t TextureWrapToGLenum(TextureWrap wrap) {
		switch (wrap)
		{
		case TextureWrap::ClampToEdge:
			return GL_CLAMP;
		case TextureWrap::Repeat:
			return GL_REPEAT;
		case TextureWrap::ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		}
		return GL_REPEAT;
	}
	uint32_t TextureFilteringToGLenum(TextureFiltering filtering, bool hasMips, uint32_t min_mag) {
		if (min_mag == 0) {
			switch (filtering)
			{
			case TextureFiltering::Linear:
				return hasMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
			case TextureFiltering::Nearest:
				return GL_NEAREST;
			}
		}
		else {
			switch (filtering)
			{
			case TextureFiltering::Linear:
				return GL_LINEAR;
			case TextureFiltering::Nearest:
				return GL_NEAREST;
			}
		}
		
		return GL_LINEAR;
	}
	bool isCorrectFormat(TextureFormat format, int channels) {
		switch (format)
		{
		case TextureFormat::R:
			return channels == 1;
			break;
		case TextureFormat::RG:
			return channels == 2;
			break;
		case TextureFormat::RGB:
			return channels == 3;
			break;
		case TextureFormat::RGBA:
			return channels == 4;
			break;
		case TextureFormat::SRGB:
			return channels == 3;
			break;
		case TextureFormat::SRGBA:
			return channels == 4;
			break;
		case TextureFormat::RGB16F:
			return channels == 3;
			break;
		case TextureFormat::RGBA16F:
			return channels == 4;
			break;
		case TextureFormat::RGBA32F:
			return channels == 4;
			break;
		}
		return true;
	}
	bool isGLTextureFormatFloat(const TextureFormat& format) {
		return format == TextureFormat::RGB16F || format == TextureFormat::RGBA16F || format == TextureFormat::RGBA32F || 
			format == TextureFormat::RG32F || format == TextureFormat::Depth32F || format == TextureFormat::Depth32F_Stencil8;
	}
}