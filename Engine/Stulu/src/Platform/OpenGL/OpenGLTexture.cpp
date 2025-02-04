#include "st_pch.h"
#include "OpenGLTexture.h"

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stulu {
	OpenGLTexture2D::OpenGLTexture2D(uint32_t internalID, uint32_t width, uint32_t height, const TextureSettings& settings) {
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_rendererID = internalID;
	}
	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const TextureSettings& settings) {
		m_width = width;
		m_height = height;
		m_settings = settings;
		m_rendererID = 0;

		std::pair<GLenum, GLenum> format = TextureFormatToGLenum(m_settings.format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, format.first, m_width, m_height);

		updateParameters();

		if (HasMips())
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, const TextureSettings& settings)
		:m_path(path), m_settings(settings) {
		update();
	}
	OpenGLTexture2D::~OpenGLTexture2D() {
		glDeleteTextures(1, &m_rendererID);
	}
	void OpenGLTexture2D::bind(uint32_t slot) const {
		glBindTextureUnit(slot, m_rendererID);
	}
	void OpenGLTexture2D::setData(const void* data, uint32_t size, uint32_t mipLevel) {
		auto& [internalFormat, dataFormat] = TextureFormatToGLenum(m_settings.format);
		glTextureSubImage2D(m_rendererID, mipLevel, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel) {
		auto& [internalFormat, dataFormat] = TextureFormatToGLenum(m_settings.format);
		glTextureSubImage2D(m_rendererID, mipLevel, posX, posY, 1, 1, dataFormat, GL_UNSIGNED_BYTE, &hexData);
	}
	void OpenGLTexture2D::getData(void* data, uint32_t size, uint32_t mipLevel) const {
		TextureFormat format = m_settings.format;
		auto& [internalFormat, dataFormat] = TextureFormatToGLenum(format);
		glGetTextureImage(m_rendererID, mipLevel, dataFormat, GL_UNSIGNED_BYTE, size, data);
	}
	uint32_t OpenGLTexture2D::getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel) const {
		uint32_t data;

		TextureFormat format = m_settings.format;
		auto& [internalFormat, dataFormat] = TextureFormatToGLenum(format);
		glGetTextureSubImage(m_rendererID, mipLevel, posX, posY, 0, 1, 1, 1, dataFormat, GL_UNSIGNED_BYTE, sizeof(uint32_t), &data);

		return data;
	}
	void OpenGLTexture2D::update() {
		if (m_rendererID)
			glDeleteTextures(1, &m_rendererID);

		int32_t width, height, channels;
		stbi_set_flip_vertically_on_load(1);

		void* textureData = nullptr;
		bool isFloatData = false;

		if (isTextureFileFloat(m_path.c_str())) {
			textureData = stbi_loadf(m_path.c_str(), &width, &height, &channels, 0);
			isFloatData = true;
		}
		else {
			textureData = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
		}

		CORE_ASSERT(textureData, "Texture failed to load: {0}", m_path);
		m_width = width;
		m_height = height;
		
		auto& format = TextureFormatToGLenum(m_settings.format);
		if (!isCorrectFormat(m_settings.format, channels, m_path.c_str())) {
			CORE_ERROR("Texture format not correct for texture {0}", m_path);
			format = TextureFormatToGLenum(m_settings.format, channels);
		}
		auto& [internalFormat, dataFormat] = format;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		glTextureStorage2D(m_rendererID, m_settings.levels, internalFormat, m_width, m_height);

		updateParameters();

		if (textureData) {
			glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, isFloatData ? GL_FLOAT : GL_UNSIGNED_BYTE, textureData);
			stbi_image_free(textureData);
		}

		if (HasMips())
			glGenerateMipmap(GL_TEXTURE_2D);


	}
	void OpenGLTexture2D::updateParameters() {
		GLenum wrap = TextureWrapToGLenum(m_settings.wrap);
		
		glBindTexture(GL_TEXTURE_2D, m_rendererID);
		glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, TextureFilteringToGLenumMinification(m_settings.filtering));
		glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, TextureFilteringToGLenumMagnification(m_settings.filtering));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);
		if (m_settings.wrap == TextureWrap::ClampToBorder) {
			float borderColor[] = { m_settings.border.x,  m_settings.border.y,  m_settings.border.z,  m_settings.border.w };
			glTextureParameterfv(m_rendererID, GL_TEXTURE_BORDER_COLOR, borderColor);
		}
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
			internalFormat = GL_SRGB8;
			m_dataFormat = GL_SRGB;
			break;
		case Stulu::TextureFormat::SRGBA:
			CORE_WARN("SRGBA Texture are currently not supported");
			internalFormat = GL_SRGB8_ALPHA8;
			m_dataFormat = GL_SRGB_ALPHA;
			break;
		case Stulu::TextureFormat::RGBA16F:
			internalFormat = GL_RGBA16F;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RGB16F:
			internalFormat = GL_RGB16F;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::RG16F:
			internalFormat = GL_RG16F;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::R16F:
			internalFormat = GL_R16F;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::RGBA32F:
			internalFormat = GL_RGBA32F;
			m_dataFormat = GL_RGBA;
			break;
		case Stulu::TextureFormat::RGB32F:
			internalFormat = GL_RGB32F;
			m_dataFormat = GL_RGB;
			break;
		case Stulu::TextureFormat::RG32F:
			internalFormat = GL_RG32F;
			m_dataFormat = GL_RG;
			break;
		case Stulu::TextureFormat::R32F:
			internalFormat = GL_R32F;
			m_dataFormat = GL_RED;
			break;
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
		}
		return { internalFormat,m_dataFormat };
	}
	uint32_t TextureWrapToGLenum(TextureWrap wrap) {
		switch (wrap)
		{
		case TextureWrap::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case TextureWrap::Repeat:
			return GL_REPEAT;
		case TextureWrap::ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		}
		return GL_CLAMP_TO_EDGE;
	}
	uint32_t TextureFilteringToGLenumMinification(TextureFiltering filter) {
		switch (filter)
		{
		case TextureFiltering::Linear:
			return GL_LINEAR;
		case TextureFiltering::Trilinear:
			return GL_LINEAR_MIPMAP_LINEAR;
		case TextureFiltering::Nearest:
			return GL_NEAREST;
		}
		return GL_NEAREST;
	}
	uint32_t TextureFilteringToGLenumMagnification(TextureFiltering filter) {
		switch (filter)
		{
		case TextureFiltering::Bilinear:
			return GL_LINEAR;
		case TextureFiltering::Trilinear:
			return GL_LINEAR;
		case TextureFiltering::Nearest:
			return GL_NEAREST;
		}
		return GL_NEAREST;
	}
	bool isCorrectFormat(TextureFormat& format, int channels, const char* flName) {
		bool is32 = stbi_is_hdr(flName);
		bool is16 = stbi_is_16_bit(flName);
		if (is32) {
			if (channels == 1) {
				if (format == TextureFormat::R32F)
					return true;
				format = TextureFormat::R32F;
			}
			if (channels == 2) {
				if (format == TextureFormat::RG32F)
					return true;
				format = TextureFormat::RG32F;
			}
			if (channels == 3) {
				if (format == TextureFormat::RGB32F)
					return true;
				format = TextureFormat::RGB32F;
			}
			if (channels == 4) {
				if (format == TextureFormat::RGBA32F)
					return true;
				format = TextureFormat::RGBA32F;
			}
		}
		else if (is16) {
			if (channels == 1) {
				if (format == TextureFormat::R16F)
					return true;
				format = TextureFormat::R16F;
			}
			if (channels == 2) {
				if (format == TextureFormat::RG16F)
					return true;
				format = TextureFormat::RG16F;
			}
			if (channels == 3) {
				if (format == TextureFormat::RGB16F)
					return true;
				format = TextureFormat::RGB16F;
			}
			if (channels == 4) {
				if (format == TextureFormat::RGBA16F)
					return true;
				format = TextureFormat::RGBA16F;
			}
		}
		else {
			if (channels == 1) {
				if (format == TextureFormat::R)
					return true;
				format = TextureFormat::R;
			}
			if (channels == 2) {
				if (format == TextureFormat::RG)
					return true;
				format = TextureFormat::RG;
			}
			if (channels == 3) {
				if (format == TextureFormat::RGB || format == TextureFormat::SRGB)
					return true;
				format = TextureFormat::RGB;
			}
			if (channels == 4) {
				if (format == TextureFormat::RGBA || format == TextureFormat::SRGBA)
					return true;
				format = TextureFormat::RGBA;
			}
		}
		return false;
	}
#include <magic_enum/magic_enum.hpp>
	bool isGLTextureFormatFloat(const TextureFormat& format) {
		//the easy way
		const auto name = magic_enum::enum_name(format);
		return name.find("F") != name.npos;
	}
	bool isGLTextureFormat32BIT(const TextureFormat& format) {
		//the easy way
		const auto name = magic_enum::enum_name(format);
		return name.find("16") != name.npos;
	}
	bool isGLTextureFormat16BIT(const TextureFormat& format) {
		//the easy way
		const auto name = magic_enum::enum_name(format);
		return name.find("32") != name.npos;
	}
	bool isTextureFileFloat(const char* flName) {
		return stbi_is_16_bit(flName) || stbi_is_hdr(flName);
	}
}