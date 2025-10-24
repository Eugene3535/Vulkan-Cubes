#include <array>
#include <vector>
#include <unordered_set>
#include <string>
#include <cstring>
#include <cstdio>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/setup/VulkanApi.hpp"


namespace
{
#ifdef DEBUG
    constexpr std::array<const char*, 1> VALIDATION_LAYERS = 
    {
        "VK_LAYER_KHRONOS_validation"
    };

    bool check_validation_layer_support() noexcept
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, VK_NULL_HANDLE);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : VALIDATION_LAYERS)
        {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
                return false;
        }

        return true;
    }
#endif // !DEBUG
}


VulkanApi::VulkanApi() noexcept:
    m_instance(nullptr),
    m_physicalDevice(nullptr),
    m_device(nullptr),
    m_queue(nullptr),
    m_mainQueueFamilyIndex(0)
{

}


VulkanApi::~VulkanApi() = default;


VkResult VulkanApi::initialize() noexcept
{
    if(createInstance() == VK_SUCCESS)
        if(selectVideoCard() == VK_SUCCESS)
            if(createDevice() == VK_SUCCESS)
                return VK_SUCCESS;

    return VK_ERROR_INITIALIZATION_FAILED;
}


void VulkanApi::destroy() noexcept
{
    vkDestroyDevice(m_device, VK_NULL_HANDLE);
    vkDestroyInstance(m_instance, VK_NULL_HANDLE);
}


VkInstance VulkanApi::getInstance() const noexcept
{
    return m_instance;
}


VkPhysicalDevice VulkanApi::getPhysicalDevice() const noexcept
{
    return m_physicalDevice;
}


VkDevice VulkanApi::getDevice() const noexcept
{
    return m_device;
}


VkQueue VulkanApi::getQueue() const noexcept
{
    return m_queue;
}


uint32_t VulkanApi::getMainQueueFamilyIndex() const noexcept
{
    return m_mainQueueFamilyIndex;
}


VkResult VulkanApi::createInstance() noexcept
{
#ifdef DEBUG
    if (!check_validation_layer_support())
        return VK_ERROR_INITIALIZATION_FAILED;
#endif // !DEBUG

    std::vector<const char*> requiredExtensions = 
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

#ifdef _WIN32
    requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef __linux__
    requiredExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#ifdef DEBUG
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    uint32_t availableExtensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

    std::unordered_set<std::string> deviceExtensions;

    for (const auto& it : availableExtensions)
        deviceExtensions.insert(it.extensionName);

    for (const auto it : requiredExtensions)
        if (deviceExtensions.find(it) == deviceExtensions.end())	
            return VK_ERROR_INITIALIZATION_FAILED;

    constexpr VkApplicationInfo appInfo = 
    {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = "Vulkan App",
        .applicationVersion = 1,
        .pEngineName        = "Shiny Engine",
        .engineVersion      = 1,
        .apiVersion         = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo instanceInfo = 
    {
        .sType =                   VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext =                   nullptr,
        .flags =                   0,
        .pApplicationInfo =        &appInfo,
        .enabledLayerCount =       0,
        .ppEnabledLayerNames =     nullptr,
        .enabledExtensionCount =   static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

#ifdef DEBUG
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        instanceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        VkDebugUtilsMessengerCreateInfoEXT debugInfo = 
        {
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext           = nullptr,
            .flags           = 0,
            .messageSeverity = 0,
            .messageType     = 0,
            .pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept
            {
                printf("validation layer: %s\n", pCallbackData->pMessage);

                return VK_FALSE;
            },
            .pUserData = nullptr
        };

        instanceInfo.pNext = static_cast<const void*>(&debugInfo);
#endif // !DEBUG

    return vkCreateInstance(&instanceInfo, nullptr, &m_instance);
}


VkResult VulkanApi::selectVideoCard() noexcept
{    
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount)
    {
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

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
            m_physicalDevice = device;
        }
        else if(auto device = find_device(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU); device)
        {
            m_physicalDevice = device;
        }
    }

    return m_physicalDevice ? VK_SUCCESS : VK_ERROR_INITIALIZATION_FAILED;
}


VkResult VulkanApi::createDevice() noexcept
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(m_physicalDevice, &supportedFeatures);

    VkPhysicalDeviceFeatures enabledFeatures = {};

    if (supportedFeatures.samplerAnisotropy)
        enabledFeatures.samplerAnisotropy = VK_TRUE;

    if (supportedFeatures.fillModeNonSolid)
        enabledFeatures.fillModeNonSolid = VK_TRUE;

    {// Find main queue family index
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (size_t i = 0; i < queueFamilies.size(); ++i)
        {
            if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
            {
                m_mainQueueFamilyIndex = static_cast<uint32_t>(i);
                break;
            }
        }
    }

	if(m_mainQueueFamilyIndex != UINT32_MAX)
    {
        const float queuePriority = 1.0f;

    	const VkDeviceQueueCreateInfo queueInfo = 
        {
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = 0,
            .queueFamilyIndex = m_mainQueueFamilyIndex,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority
        };

        constexpr std::array<const char*, 2> requiredExtensions = 
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        std::unordered_set<std::string> deviceExtensions;

        for (const auto& it : availableExtensions)
            deviceExtensions.insert(it.extensionName);

        for (const auto& extension : requiredExtensions)
            if(deviceExtensions.find(extension) == deviceExtensions.end())
                return VK_ERROR_INITIALIZATION_FAILED;

        constexpr VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = 
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
            .dynamicRendering = VK_TRUE
        };

        VkDeviceCreateInfo deviceInfo = 
        {
            .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext                   = &dynamic_rendering_feature,
            .flags                   = 0,
            .queueCreateInfoCount    = 1,
            .pQueueCreateInfos       = &queueInfo,
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr,
            .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
            .ppEnabledExtensionNames = requiredExtensions.data(),
            .pEnabledFeatures        = &enabledFeatures
        };
#ifdef DEBUG
    	deviceInfo.enabledLayerCount   = static_cast<uint32_t>(VALIDATION_LAYERS.size());
    	deviceInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
#endif

        if(vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device) == VK_SUCCESS)
        {
            vkGetDeviceQueue(m_device, m_mainQueueFamilyIndex, 0, &m_queue);

            return VK_SUCCESS;
        }
    }

    return VK_ERROR_INITIALIZATION_FAILED;
}