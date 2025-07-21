#include "vulkan_api/utils/Structures.hpp"
#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"

#define SWAPCHAIN_BUFFER_COUNT 3


Swapchain::Swapchain() noexcept:
    handle(nullptr),
	format(0),
	m_logicalDevice(nullptr)
{

}


Swapchain::~Swapchain() = default;


bool Swapchain::create(VkPhysicalDevice phisycalDevice, VkDevice logicalDevice, VkSurfaceKHR surface) noexcept
{
	m_logicalDevice = logicalDevice;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phisycalDevice, surface, &surfaceCapabilities) != VK_SUCCESS)
		return false;

    auto swapChainSupport = vk::query_swapchain_support(phisycalDevice, surface);

    if (SWAPCHAIN_BUFFER_COUNT > swapChainSupport->capabilities.maxImageCount)
		return false;

	format = static_cast<VkFormat>(swapChainSupport->getSurfaceFormat().format);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType			 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface			 = surface;
	swapchainCreateInfo.minImageCount	 = SWAPCHAIN_BUFFER_COUNT;
	swapchainCreateInfo.imageFormat		 = static_cast<VkFormat>(format);
	swapchainCreateInfo.imageColorSpace	 = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent		 = surfaceCapabilities.currentExtent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage		 = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	swapchainCreateInfo.preTransform	 = swapChainSupport->capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha	 = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode		 = swapChainSupport->getPresentMode();
	swapchainCreateInfo.clipped			 = VK_TRUE;
	swapchainCreateInfo.oldSwapchain	 = handle;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, nullptr, &handle) == VK_SUCCESS)
	{
        uint32_t imageCount = SWAPCHAIN_BUFFER_COUNT;
		images.resize(imageCount);

        if (vkGetSwapchainImagesKHR(logicalDevice, handle, &imageCount, images.data()) == VK_SUCCESS)
            return true;
	}

    return false;
}


void Swapchain::createImageViews() noexcept
{
	if(!images.empty())
	{
		imageViews.resize(images.size());

		for (size_t i = 0; i < images.size(); ++i)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = images[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = static_cast<VkFormat>(format);
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (VkImageView imageView; (vkCreateImageView(m_logicalDevice, &viewInfo, nullptr, &imageView) == VK_SUCCESS))
				imageViews[i] = imageView;	
		}
	}
}


void Swapchain::cleanup() noexcept
{
	if(m_logicalDevice)
	{
		// for (auto framebuffer : swapChainFramebuffers)
		// {
		//     vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		// }

		for (auto imageView : imageViews)
		    vkDestroyImageView(m_logicalDevice, imageView, nullptr);
		
		vkDestroySwapchainKHR(m_logicalDevice, handle, nullptr);
	}
}