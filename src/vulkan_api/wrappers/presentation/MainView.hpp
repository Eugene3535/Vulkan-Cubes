#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <vector>

#include "vulkan_api/wrappers/setup/VulkanApi.hpp"


class MainView
{
public:
    MainView() noexcept;
    ~MainView();

    VkResult create(VulkanApi& api, struct GLFWwindow* window) noexcept;
    VkResult recreate() noexcept;
    void     destroy()  noexcept;

    VkSwapchainKHR&   getSwapchain() noexcept;
    VkFormat          getFormat()    const noexcept;
    const VkExtent2D& getExtent()    const noexcept;

    VkImage     getImage(uint32_t index)     const noexcept;
    VkImageView getImageView(uint32_t index) const noexcept;

    VulkanApi* getVulkanApi() const noexcept;

private:
    VulkanApi* m_api;

    VkSurfaceKHR   m_surface;
    VkSwapchainKHR m_swapchain;

    std::vector<VkImage>     m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat   m_format;
    VkExtent2D m_extent;
};

#endif // !MAIN_VIEW_HPP