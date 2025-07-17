#ifndef PHYSICAL_DEVICE_HPP
#define PHYSICAL_DEVICE_HPP


class PhysicalDevice
{
public:
    PhysicalDevice() noexcept;
    ~PhysicalDevice();

    bool select(struct VkInstance_T* instance) noexcept;

    struct VkPhysicalDevice_T* handle;
};

#endif // !PHYSICAL_DEVICE_HPP