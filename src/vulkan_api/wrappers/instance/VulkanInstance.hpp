#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP


class VulkanInstance
{
public:
    VulkanInstance() noexcept;
    ~VulkanInstance();

    bool create()  noexcept;
    void destroy() noexcept;

    struct VkInstance_T* handle;
};

#endif // !VULKAN_INSTANCE_HPP