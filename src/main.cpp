#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VulkanApi.hpp"


int main()
{
    const uint32_t width = 800;
    const uint32_t height = 600;

    VulkanApi api;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if(auto window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr); window)
    {
        glfwSetWindowUserPointer(window, static_cast<void*>(&api));

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            if(auto app = static_cast<VulkanApi*>(glfwGetWindowUserPointer(window)); app)
            {

            }
        });
        
        if(api.initialize())
        {
            while (!glfwWindowShouldClose(window))
            {


                glfwPollEvents();
            }
        }

        glfwDestroyWindow(window);
    }


    glfwTerminate();

    return 0;
}
