#pragma once
#include "Stulu/Renderer/Texture.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
	class STULU_API VulkanImage2D : public Texture2D {
	public:
		VulkanImage2D(uint32_t width, uint32_t height, const TextureSettings& settings);
		VulkanImage2D(const std::string& path, const TextureSettings& settings = TextureSettings());

		// internal for swap chain
		inline VulkanImage2D(VkImageView view, uint32_t width, uint32_t height) :  m_imageView(view), m_image(nullptr), m_width(width), m_height(height) {}

		virtual ~VulkanImage2D();

		virtual void bind(uint32_t slot) const override;

		virtual void* getNativeRendererObject() const override { return (void*)(m_imageView); }

		virtual uint32_t getWidth() const override { return m_width; }
		virtual uint32_t getHeight() const override { return m_height; }
		virtual uint32_t getMipWidth(uint32_t level) const override { return uint32_t((float)m_width / (glm::pow(2, level))); }
		virtual uint32_t getMipHeight(uint32_t level) const override { return uint32_t((float)m_height / (glm::pow(2, level))); }

		virtual TextureSettings& getSettings() override { return m_settings; }

		virtual void setData(const void* data, uint32_t size, uint32_t mipLevel = 0) override;
		virtual void setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) override;

		virtual void getData(void* data, uint32_t size, uint32_t mipLevel = 0) const override;
		virtual uint32_t getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel = 0) const override;

		virtual void update() override;
		virtual void updateParameters() override;

		virtual bool operator == (const Texture& other) const override;
		virtual operator int() override { return 0; }

		//temp
		virtual std::string getPath() const override { return m_path; }

		// internal
		void setSettings(const TextureSettings& settings) { m_settings = settings; }
	private:
		inline bool HasMips() const { return m_settings.levels > 1 || m_settings.filtering == TextureFiltering::Trilinear; }

		std::string m_path = "";
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		VkImage m_image;
		VkImageView m_imageView;
		TextureSettings m_settings;
	};
}

