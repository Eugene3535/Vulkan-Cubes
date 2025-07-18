#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/surface/Surface.hpp"


Surface::Surface() noexcept:
    handle(nullptr)
{

}


Surface::~Surface() = default;


bool Surface::create(VkInstance instance, struct GLFWwindow* window) noexcept
{
    if(handle)
        return true;

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hwnd = glfwGetWin32Window(window);
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    return (vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &handle) == VK_SUCCESS);
}


void Surface::destroy(VkInstance instance) noexcept
{
    if(handle)
    {
        vkDestroySurfaceKHR(instance, handle, nullptr);
        handle = nullptr;
    }  
}