#include "VulkanApi.hpp"


VulkanApi::VulkanApi() noexcept:
    m_instance(VK_NULL_HANDLE),
    m_physicalDevice(VK_NULL_HANDLE)
{

}


VulkanApi::~VulkanApi()
{
    vkDestroyInstance(m_instance, VK_NULL_HANDLE);
}


bool VulkanApi::initialize() noexcept
{
    if(createInstance())
        if(selectPhysicalDevice())
            return true;
    
    return false;
}


bool VulkanApi::createInstance() noexcept
{
    return true;
}


bool VulkanApi::selectPhysicalDevice() noexcept
{
    return true;
}