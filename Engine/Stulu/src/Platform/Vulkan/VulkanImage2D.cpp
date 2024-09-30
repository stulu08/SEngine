#include "st_pch.h"
#include "VulkanImage2D.h"
#include "VulkanRenderAPI.h"

namespace Stulu {
	VulkanImage2D::VulkanImage2D(uint32_t width, uint32_t height, const TextureSettings& settings) {
		m_width = width;
		m_height = height;
		m_settings = settings;


	}
	VulkanImage2D::VulkanImage2D(const std::string& path, const TextureSettings& settings) {

	}
	VulkanImage2D::~VulkanImage2D() {
		if (m_imageView) {
			vkDestroyImageView(VulkanRenderAPI::getDevice().device, m_imageView, nullptr);
			m_imageView;
		}
		if (m_image) {
			vkDestroyImage(VulkanRenderAPI::getDevice().device, m_image, nullptr);
			m_image = nullptr;
		}
	}
	void VulkanImage2D::bind(uint32_t slot) const
	{
	}
	void VulkanImage2D::setData(const void* data, uint32_t size, uint32_t mipLevel)
	{
	}
	void VulkanImage2D::setPixel(uint32_t hexData, uint32_t posX, uint32_t posY, uint32_t mipLevel)
	{
	}
	void VulkanImage2D::getData(void* data, uint32_t size, uint32_t mipLevel) const
	{
	}
	uint32_t VulkanImage2D::getPixel(uint32_t posX, uint32_t posY, uint32_t mipLevel) const
	{
		return 0;
	}
	void VulkanImage2D::update()
	{
	}
	void VulkanImage2D::updateParameters()
	{
	}
	bool VulkanImage2D::operator==(const Texture& other) const
	{
		return false;
	}
}