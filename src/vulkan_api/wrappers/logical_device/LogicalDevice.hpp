#ifndef LOGICAL_DEVICE_HPP
#define LOGICAL_DEVICE_HPP


class LogicalDevice
{
public:
    LogicalDevice() noexcept;
    ~LogicalDevice();

    bool create(struct VkPhysicalDevice_T* physicalDevice) noexcept;

    struct VkDevice_T* handle;
    struct VkQueue_T*  queue;
};

#endif // !LOGICAL_DEVICE_HPP