#include <vector>
#include <string>
#include <cstdio>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/physical_device/PhysicalDevice.hpp"


PhysicalDevice::PhysicalDevice() noexcept:
    handle(nullptr)
{

}


PhysicalDevice::~PhysicalDevice() = default;


bool PhysicalDevice::select(VkInstance_T* instance) noexcept
{
    if(handle)
        return true;
        
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount)
    {
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        auto find_device = [&devices](const VkPhysicalDeviceType type) -> VkPhysicalDevice
        {
            for (const auto device : devices)
            {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(device, &properties);

                if(properties.deviceType == type)
                    return device;
            }

            return nullptr;
        };

        if(auto device = find_device(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU); device)
        {
            handle = device;
        }
        else if(auto device = find_device(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU); device)
        {
            handle = device;
        }
    }

    return (handle != nullptr);
}