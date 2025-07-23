#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include <vector>

class Swapchain
{
public:
    Swapchain() noexcept;
    ~Swapchain();

    bool create(struct VkPhysicalDevice_T* phisycalDevice, struct VkDevice_T* logicalDevice, struct VkSurfaceKHR_T* surface) noexcept;
    void cleanup() noexcept;

    VkSwapchainKHR_T* handle;
    int32_t format;
    std::vector<VkImage_T*> images;

private:
    struct VkDevice_T* m_logicalDevice;
};

#endif // !SWAPCHAIN_HPP