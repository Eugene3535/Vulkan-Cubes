
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/pipeline/stages/shader/ShaderModule.hpp"
#include "Application.hpp"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


int Application::run() noexcept
{
    initWindow();

    if(initVulkan())
    {
        mainLoop();
        cleanup();
    }
    else return -1;

    return 0;
}


void Application::initWindow() noexcept
{
    if(glfwInit() == GLFW_TRUE)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, static_cast<void*>(this));

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
        {
            if(auto app = static_cast<Application*>(glfwGetWindowUserPointer(window)); app)
                app->framebufferResized = true;
        });
    }
}


bool Application::initVulkan() noexcept
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

//  Common
    if(m_api.initialize() != VK_SUCCESS) 
        return false;

    auto instance       = m_api.getInstance();
    auto physicalDevice = m_api.getPhysicalDevice();
    auto device         = m_api.getDevice();

//  Main View
    if(m_mainView.create(m_api, window) != VK_SUCCESS) 
        return false;
    
    {// Pipeline
        std::array<ShaderModule, 2> shaders;

        if(shaders[0].loadFromFile(device, VK_SHADER_STAGE_VERTEX_BIT, "res/shaders/vertex_shader.spv") != VK_SUCCESS)
            return false;

        if(shaders[1].loadFromFile(device, VK_SHADER_STAGE_FRAGMENT_BIT, "res/shaders/fragment_shader.spv") != VK_SUCCESS)
            return false;

        if(!m_pipeline.create(m_mainView, shaders)) 
            return false;

        shaders[0].destroy(device);
        shaders[1].destroy(device);
    }

    if(!m_commandPool.create(device, m_api.getMainQueueFamilyIndex())) return false;
    if(!m_sync.create(device)) return false;

    VulkanData api = 
    {
        physicalDevice,
        device,
        m_api.getQueue(),
        m_commandPool.handle,
        m_pipeline.descriptorSetLayout,
        &m_texture
    };

    if(!m_texture.loadFromFile("res/textures/container.jpg", api)) return false;
    if(!m_mesh.create(api)) return false;

    if(!m_uniformBuffers.create(api)) return false;

    return true;
}


void Application::mainLoop() noexcept
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        drawFrame();
    }

    vkDeviceWaitIdle(m_api.getDevice());
}


void Application::cleanup() noexcept
{
    auto device = m_api.getDevice();

    m_mainView.destroy();

    m_pipeline.destroy();

    m_uniformBuffers.destroy(device);

    m_texture.destroy(device);
    m_mesh.destroy(device);

    m_sync.destroy(device);

    m_commandPool.destroy(device);

    m_api.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();
}


void Application::recreateSwapChain() noexcept
{
    auto device = m_api.getDevice();
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    vkDeviceWaitIdle(device);

    m_mainView.recreate();
}


void Application::updateUniformBuffer(uint32_t currentImage) noexcept
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    UniformBufferObject ubo = 
    {
        .model = glm::translate(glm::mat4(1.0f), glm::vec3(0)),
        .view = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)),
        .proj = glm::perspective(glm::radians(60.0f), width / (float)height, 0.1f, 100.0f)
    };

    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffers.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


void Application::drawFrame() noexcept
{
    auto currentFrame = m_sync.currentFrame;

    auto device = m_api.getDevice();
    auto queue  = m_api.getQueue();

    vkWaitForFences(device, 1, &m_sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, m_mainView.getSwapchain(), UINT64_MAX, m_sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        printf("failed to acquire swap chain image!");
    }

    updateUniformBuffer(currentFrame);

    vkResetFences(device, 1, &m_sync.inFlightFences[currentFrame]);

    auto commandBuffer = m_commandPool.commandBuffers[currentFrame];
    auto descriptorSet = m_uniformBuffers.descriptorSets[currentFrame];

    vkResetCommandBuffer(m_commandPool.commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    m_pipeline.writeCommandBuffer(commandBuffer, currentFrame, imageIndex, m_mesh, descriptorSet);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_sync.imageAvailableSemaphores.data() + currentFrame;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandPool.commandBuffers[currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_sync.renderFinishedSemaphores[currentFrame];

    if (auto result = vkQueueSubmit(queue, 1, &submitInfo, m_sync.inFlightFences[currentFrame]); result != VK_SUCCESS)
    {
        printf("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &m_sync.renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &m_mainView.getSwapchain();
    presentInfo.pImageIndices      = &imageIndex;

    result = vkQueuePresentKHR(queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        recreateSwapChain();
    }
    else if (result != VK_SUCCESS)
    {
        printf("failed to present swap chain image!");
    }

    m_sync.currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    Sleep(16);
}