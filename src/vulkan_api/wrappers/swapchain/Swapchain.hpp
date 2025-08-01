#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <array>


class Swapchain
{
public:
    static constexpr uint32_t buffer_count = 3;

    Swapchain() noexcept;
    ~Swapchain();

    bool create(struct VkPhysicalDevice_T* phisycalDevice, struct VkDevice_T* logicalDevice, struct VkSurfaceKHR_T* surface, uint32_t width, uint32_t height) noexcept;
    void destroy(struct VkDevice_T* logicalDevice) noexcept;

    VkSwapchainKHR_T* handle;
    int32_t format;
    std::array<struct VkImage_T*, buffer_count> images;
    std::array<struct VkImageView_T*, buffer_count> imageViews;
    uint32_t m_width; 
    uint32_t m_height;
};

#endif // !SWAPCHAIN_HPP