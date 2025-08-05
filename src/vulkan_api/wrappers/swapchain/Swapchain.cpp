#include <glm/common.hpp>

#include "vulkan_api/utils/Structures.hpp"
#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"


static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) noexcept
{
    if (capabilities.currentExtent.width != UINT_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent = 
        {
            width,
            height
        };

        actualExtent.width  = glm::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = glm::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}


Swapchain::Swapchain() noexcept:
    handle(nullptr),
	format(0),
	m_width(0),
	m_height(0)
{

}


Swapchain::~Swapchain() = default;


bool Swapchain::create(VkPhysicalDevice phisycalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, uint32_t width, uint32_t height) noexcept
{
	if(handle)
		return true;

    auto swapChainSupport = vk::query_swapchain_support(phisycalDevice, surface);

    if (buffer_count > swapChainSupport->capabilities.maxImageCount)
		return false;

	format = static_cast<VkFormat>(swapChainSupport->getSurfaceFormat().format);
	auto extent = choose_swap_extent(swapChainSupport->capabilities, width, height);
	m_width = extent.width;
	m_height = extent.height;

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType			 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface			 = surface;
	swapchainCreateInfo.minImageCount	 = buffer_count;
	swapchainCreateInfo.imageFormat		 = static_cast<VkFormat>(format);
	swapchainCreateInfo.imageColorSpace	 = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchainCreateInfo.imageExtent		 = extent;
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
        uint32_t imageCount = buffer_count;

        if (vkGetSwapchainImagesKHR(logicalDevice, handle, &imageCount, images.data()) == VK_SUCCESS)
        {
			for (size_t i = 0; i < images.size(); ++i)
			{
				VkImageViewCreateInfo viewInfo = 
				{
					.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image = images[i],
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format = static_cast<VkFormat>(format),
					.subresourceRange = 
					{
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 1
					}
				};

				if (VkImageView imageView; (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView) == VK_SUCCESS))
					imageViews[i] = imageView;
				else
					return false;
			}

			return true;
		}
	}

    return false;
}


void Swapchain::destroy(struct VkDevice_T* logicalDevice) noexcept
{
	if(handle)
	{
		for (auto imageView : imageViews)
        	vkDestroyImageView(logicalDevice, imageView, nullptr);

		vkDestroySwapchainKHR(logicalDevice, handle, nullptr);
		handle = nullptr;
	}
}