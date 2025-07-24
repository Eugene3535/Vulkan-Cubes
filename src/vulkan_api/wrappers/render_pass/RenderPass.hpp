#ifndef RENDER_PASS_HPP
#define RENDER_PASS_HPP

#include "vulkan_api/wrappers/swapchain/Swapchain.hpp"

class RenderPass
{
public:
    RenderPass() noexcept;
    ~RenderPass();

    bool create(struct VkDevice_T* logicalDevice, const Swapchain& swapchain, struct GLFWwindow* window) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    struct VkRenderPass_T* handle;
    std::array<struct VkImageView_T*, Swapchain::buffer_count> imageViews;
    std::array<struct VkFramebuffer_T*, Swapchain::buffer_count> framebuffers;
};

#endif // !RENDER_PASS_HPP