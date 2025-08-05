
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/shader/ShaderModule.hpp"
#include "Application.hpp"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


void Application::run() noexcept
{
    initWindow();

    if(initVulkan())
    {
        mainLoop();
        cleanup();
    }
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

    {// Common
        if(!m_instance.create())                             return false;
        if(!m_physicalDevice.select(m_instance.handle))      return false;
        if(!m_logicalDevice.create(m_physicalDevice.handle)) return false;
        if(!m_surface.create(m_instance.handle, window))     return false;
        if(!m_swapchain.create(m_physicalDevice.handle, m_logicalDevice.handle, m_surface.handle, width, height)) return false;
    }

    {// Pipeline
        std::array<ShaderModule, 2> shaders;

        if(!shaders[0].loadFromFile(m_logicalDevice.handle, { "res/shaders/vertex_shader.spv" }))
            return false;

        if(!shaders[1].loadFromFile(m_logicalDevice.handle, { "res/shaders/fragment_shader.spv" }))
            return false;

        if(!m_pipeline.create(m_logicalDevice.handle, { shaders }, m_swapchain)) 
            return false;

        shaders[0].destroy(m_logicalDevice.handle);
        shaders[1].destroy(m_logicalDevice.handle);
    }

    if(!m_commandPool.create(m_logicalDevice)) return false;
    if(!m_sync.create(m_logicalDevice.handle)) return false;

    VulkanApi api = 
    {
        m_physicalDevice.handle,
        m_logicalDevice.handle,
        m_logicalDevice.queue,
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

    vkDeviceWaitIdle(m_logicalDevice.handle);
}


void Application::cleanup() noexcept
{
    auto device = m_logicalDevice.handle;

    m_swapchain.destroy(device);
    m_pipeline.destroy(device);

    m_uniformBuffers.destroy(device);

    m_texture.destroy(device);
    m_mesh.destroy(device);

    m_sync.destroy(device);

    m_commandPool.destroy(device);

    m_logicalDevice.destroy();

    m_surface.destroy(m_instance.handle);
    m_instance.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();
}


void Application::recreateSwapChain() noexcept
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);

    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_logicalDevice.handle);

    m_swapchain.destroy(m_logicalDevice.handle);
    m_swapchain.create(m_physicalDevice.handle, m_logicalDevice.handle, m_surface.handle, width, height);
}


void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) noexcept
{
    auto currentFrame = m_sync.currentFrame;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        printf("failed to begin recording command buffer!");
        return;
    }

    const VkImageMemoryBarrier image_memory_barrier_begin 
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .image = m_swapchain.images[imageIndex],
        .subresourceRange = 
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
        0,
        0,
        nullptr,
        0,
        nullptr,
        1, // imageMemoryBarrierCount
        &image_memory_barrier_begin // pImageMemoryBarriers
    );

    VkExtent2D extent = { m_swapchain.m_width, m_swapchain.m_height };
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    const VkRenderingAttachmentInfoKHR color_attachment_info = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = m_swapchain.imageViews[imageIndex],
        .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clearColor
    };

    VkRect2D renderArea = { {0, 0}, extent };

    const VkRenderingInfoKHR render_info =
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea = renderArea,
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_info,
    };

    vkCmdBeginRendering(commandBuffer, &render_info);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.handle);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {m_mesh.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, m_mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.layout, 0, 1, &m_uniformBuffers.descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_mesh.getIndexCount()), 1, 0, 0, 0);

    vkCmdEndRendering(commandBuffer);

    const VkImageMemoryBarrier image_memory_barrier_end
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = m_swapchain.images[imageIndex],
        .subresourceRange = 
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
        0,
        0,
        nullptr,
        0,
        nullptr,
        1, // imageMemoryBarrierCount
        &image_memory_barrier_end // pImageMemoryBarriers
    );

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        printf("failed to record command buffer!");
    }
}


void Application::updateUniformBuffer(uint32_t currentImage) noexcept
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    VkExtent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    UniformBufferObject ubo{};
    ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0));
    ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(60.0f), width / (float)height, 0.1f, 100.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffers.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


void Application::drawFrame() noexcept
{
    auto currentFrame = m_sync.currentFrame;

    auto device = m_logicalDevice.handle;

    vkWaitForFences(device, 1, &m_sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, m_swapchain.handle, UINT64_MAX, m_sync.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

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

    vkResetCommandBuffer(m_commandPool.commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(m_commandPool.commandBuffers[currentFrame], imageIndex);

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

    if (auto result = vkQueueSubmit(m_logicalDevice.queue, 1, &submitInfo, m_sync.inFlightFences[currentFrame]); result != VK_SUCCESS)
    {
        printf("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &m_sync.renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &m_swapchain.handle;
    presentInfo.pImageIndices      = &imageIndex;

    result = vkQueuePresentKHR(m_logicalDevice.queue, &presentInfo);

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