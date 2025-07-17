#include <vector>
#include <unordered_set>
#include <string>
#include <cstdio>

#include "vulkan/utils/Constants.hpp"
#include "vulkan/utils/Helpers.hpp"
#include "vulkan/wrappers/VulkanInstance.hpp"


VulkanInstance::VulkanInstance() noexcept:
    handle(VK_NULL_HANDLE)
{

}


VulkanInstance::~VulkanInstance()
{
    destroy();
}


bool VulkanInstance::create() noexcept
{
    if(handle)
        return true;

#ifdef DEBUG
    if (!vk::check_validation_layer_support())
        return false;
#endif // !DEBUG

    std::vector<const char*> neededExtensions = 
    {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

#ifdef _WIN32
    neededExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef __linux__
    neededExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

#ifdef DEBUG
    neededExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    uint32_t availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

    std::unordered_set<std::string> availableExtensionsSet;

    for (uint32_t i = 0; i < availableExtensionCount; ++i)
        availableExtensionsSet.insert(availableExtensions[i].extensionName);

    for (const auto it : neededExtensions)
        if (availableExtensionsSet.find(it) == availableExtensionsSet.end())	
            return false;	

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "Shiny Engine";
    appInfo.engineVersion = 1;
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(neededExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = neededExtensions.data();

#ifdef DEBUG
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        instanceCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        debugCreateInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept
        {
            printf("validation layer: %s\n", pCallbackData->pMessage);

            return VK_FALSE;
        };

        instanceCreateInfo.pNext = static_cast<const void*>(&debugCreateInfo);
#endif // !DEBUG

    return (vkCreateInstance(&instanceCreateInfo, nullptr, &handle) == VK_SUCCESS);
}


void VulkanInstance::destroy() noexcept
{
    if(handle)
    {
        vkDestroyInstance(handle, VK_NULL_HANDLE);
        handle = VK_NULL_HANDLE;
    }
}