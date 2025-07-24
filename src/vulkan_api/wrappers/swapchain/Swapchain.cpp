#include "vulkan_api/utils/Structures.hpp"
#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"




Swapchain::Swapchain() noexcept:
    handle(nullptr),
	format(0)
{

}


Swapchain::~Swapchain() = default;


bool Swapchain::create(VkPhysicalDevice phisycalDevice, VkDevice logicalDevice, VkSurfaceKHR surface) noexcept
{
	if(handle)
		return true;

	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phisycalDevice, surface, &surfaceCapabilities) != VK_SUCCESS)
		return false;

    auto swapChainSupport = vk::query_swapchain_support(phisycalDevice, surface);

    if (buffer_count > swapChainSupport->capabilities.maxImageCount)
		return false;

	format = static_cast<VkFormat>(swapChainSupport->getSurfaceFormat().format);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType			 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.surface			 = surface;
	swapchainCreateInfo.minImageCount	 = buffer_count;
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
        uint32_t imageCount = buffer_count;

        if (vkGetSwapchainImagesKHR(logicalDevice, handle, &imageCount, images.data()) == VK_SUCCESS)
            return true;
	}

    return false;
}


void Swapchain::destroy(struct VkDevice_T* logicalDevice) noexcept
{
	if(handle)
	{
		vkDestroySwapchainKHR(logicalDevice, handle, nullptr);
		handle = nullptr;
	}
}