#include <vector>
#include <memory>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/common.hpp>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/presentation/MainView.hpp"


namespace 
{
    struct SwapChainSupportDetails 
    {
        VkPresentModeKHR getPresentMode() const noexcept
        {
            for (const auto& availablePresentMode : presentModes)
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    return availablePresentMode;

            return VK_PRESENT_MODE_FIFO_KHR;
        }


        VkSurfaceFormatKHR getSurfaceFormat() const noexcept
        {
            if(!formats.empty())
            {
                for (const auto& availableFormat : formats)
                    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                            return availableFormat;

                return formats[0];
            }

            return {};
        }

        VkSurfaceCapabilitiesKHR		capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>	presentModes;
    };


    std::unique_ptr<SwapChainSupportDetails> query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) noexcept
    {
        auto details = std::make_unique<SwapChainSupportDetails>();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details->capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) 
        {
            details->formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details->formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) 
        {
            details->presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details->presentModes.data());
        }

        return details;
    }
} // empty namespace



MainView::MainView() noexcept:
    m_api(nullptr),
    m_surface(VK_NULL_HANDLE),
    m_swapchain(VK_NULL_HANDLE),
    m_format(VK_FORMAT_UNDEFINED),
    m_extent({})
{

}


MainView::~MainView() = default;


VkResult MainView::create(VulkanApi& api, GLFWwindow* window) noexcept
{
    m_api = &api;

#ifdef _WIN32
    const VkWin32SurfaceCreateInfoKHR surfaceInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = glfwGetWin32Window(window)
    };

    if(vkCreateWin32SurfaceKHR(api.getInstance(), &surfaceInfo, nullptr, &m_surface) == VK_SUCCESS)
        return recreate();
#endif

#ifdef __linux__
    xcb_connection_t* connection = xcb_connect(nullptr, nullptr);

    if (xcb_connection_has_error(connection))
        return VK_ERROR_INITIALIZATION_FAILED;

    const VkXcbSurfaceCreateInfoKHR surfaceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .connection = connection,
        .window = static_cast<xcb_window_t>(glfwGetX11Window(window))
    };

    if (vkCreateXcbSurfaceKHR(api.getInstance(), &surfaceInfo, nullptr, &m_surface) == VK_SUCCESS)
        return recreate();
#endif

    return VK_ERROR_INITIALIZATION_FAILED;
}


VkResult MainView::recreate() noexcept
{
    auto choose_swap_extent = [](const VkSurfaceCapabilitiesKHR& capabilities, const VkExtent2D& currentExtent) -> VkExtent2D
    {
        if (capabilities.currentExtent.width != UINT_MAX)
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = currentExtent;
            actualExtent.width  = glm::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = glm::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    };

    if(m_api && m_surface)
    {
        auto phisycalDevice = m_api->getPhysicalDevice();
        auto device = m_api->getDevice();

        if(m_swapchain)
        {
            for (auto imageView : m_imageViews)
            	vkDestroyImageView(device, imageView, nullptr);

            vkDestroySwapchainKHR(device, m_swapchain, nullptr);
            m_swapchain = nullptr;
        }

        auto swapChainSupport = query_swapchain_support(phisycalDevice, m_surface);

        if (buffer_count > swapChainSupport->capabilities.maxImageCount)
            return VK_ERROR_INITIALIZATION_FAILED;

        m_format = swapChainSupport->getSurfaceFormat().format;
        m_extent = choose_swap_extent(swapChainSupport->capabilities, m_extent);

        const VkSwapchainCreateInfoKHR swapchainInfo = 
        {
            .sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext                 = VK_NULL_HANDLE,
            .flags                 = 0,
            .surface               = m_surface,
            .minImageCount         = buffer_count,
            .imageFormat           = m_format,
            .imageColorSpace       = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent           = m_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr,
            .preTransform          = swapChainSupport->capabilities.currentTransform,
            .compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode           = swapChainSupport->getPresentMode(),
            .clipped               = VK_TRUE,
            .oldSwapchain          = m_swapchain
        };

        if (vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &m_swapchain) == VK_SUCCESS)
        {
            uint32_t imageCount = buffer_count;

            if (vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, m_images.data()) == VK_SUCCESS)
            {
                for (size_t i = 0; i < m_images.size(); ++i)
                {
                    if (vk::createImageView2D(device, m_images[i], m_format, VK_IMAGE_ASPECT_COLOR_BIT, m_imageViews[i]) != VK_SUCCESS)
                        return VK_ERROR_INITIALIZATION_FAILED;  
                }

                return VK_SUCCESS;
            }
        }
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}


void MainView::destroy() noexcept
{
    if(m_api)
    {
        auto instance = m_api->getInstance();
        auto device = m_api->getDevice();

        if(m_swapchain)
        {
            for (auto imageView : m_imageViews)
                vkDestroyImageView(device, imageView, nullptr);

            vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        }

        if(m_surface)
            vkDestroySurfaceKHR(instance, m_surface, VK_NULL_HANDLE);
    }
}


VkSwapchainKHR& MainView::getSwapchain() noexcept
{
    return m_swapchain;
}


VkFormat MainView::getFormat() const noexcept
{
    return m_format;
}


const VkExtent2D& MainView::getExtent() const noexcept
{
    return m_extent;
}


VkImage MainView::getImage(uint32_t index) const noexcept
{
    return m_images[index];
}


VkImageView MainView::getImageView(uint32_t index) const noexcept
{
    return m_imageViews[index];
}


VulkanApi* MainView::getVulkanApi() const noexcept
{
    return m_api;
}