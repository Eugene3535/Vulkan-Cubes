
#include <cstring>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.hpp"
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

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            if(auto app = static_cast<Application*>(glfwGetWindowUserPointer(window)))
            {
                app->m_width = width;
                app->m_height = height;
                app->framebufferResized = true;
            }
                
        });
    }
}


bool Application::initVulkan() noexcept
{
    glfwGetFramebufferSize(window, &m_width, &m_height);

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
        std::array<ShaderStage, 2> shaders;

        if(shaders[0].loadFromFile(device, VK_SHADER_STAGE_VERTEX_BIT, "res/shaders/vertex_shader.spv") != VK_SUCCESS)
            return false;

        if(shaders[1].loadFromFile(device, VK_SHADER_STAGE_FRAGMENT_BIT, "res/shaders/fragment_shader.spv") != VK_SUCCESS)
            return false;

        std::array<const VertexInputState::Attribute, 3> attributes =
        {
            VertexInputState::Attribute::Float2,
            VertexInputState::Attribute::Float3,
            VertexInputState::Attribute::Float2
        };

        DescriptorSetLayout uniformDescriptors;
        uniformDescriptors.addDescriptor(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        uniformDescriptors.addDescriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        GraphicsPipeline::State state;

        state.setupShaderStages(shaders)->
            setupVertexInput(attributes)->
            setupInputAssembler(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)->
            setupViewport()->
            setupRasterization(VK_POLYGON_MODE_FILL)->
            setupMultisampling()->
            setupColorBlending(VK_FALSE)->
            setupDescriptorSetLayout(uniformDescriptors);


        if(m_pipeline.create(m_mainView, state) != VK_SUCCESS) 
            return false;

        shaders[0].destroy(device);
        shaders[1].destroy(device);
    }

    if(!m_commandPool.create(device, m_api.getMainQueueFamilyIndex()))
        return false;

    if(!m_sync.create(device)) 
        return false;

    VulkanData data = 
    {
        physicalDevice,
        device,
        m_api.getQueue(),
        m_commandPool.handle,
        m_pipeline.getDescriptorSetLayout(),
        &m_texture
    };

    if(!m_texture.loadFromFile("res/textures/container.jpg", data))
        return false;

    if(!m_mesh.create(data)) 
        return false;

    if(!m_uniformBuffers.create(data)) 
        return false;

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

    m_pipeline.destroy(device);

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
    UniformBufferObject ubo = 
    {
        .model = glm::translate(glm::mat4(1.0f), glm::vec3(0)),
        .view = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)),
        .proj = glm::perspective(glm::radians(60.0f), m_width / (float)m_height, 0.1f, 100.0f)
    };

    ubo.proj[1][1] *= -1;

    memcpy(m_uniformBuffers.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}


bool Application::writeCommandBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t imageIndex, const Mesh& mesh, VkDescriptorSet descriptorSet) noexcept
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        return false;
    
    const VkImageMemoryBarrier image_memory_barrier_begin 
    {
        .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout     = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .image         = m_mainView.getImage(imageIndex),
        .subresourceRange = 
        {
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1
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

    VkExtent2D extent = m_mainView.getExtent();

    const VkRenderingAttachmentInfoKHR color_attachment_info = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .imageView = m_mainView.getImageView(imageIndex),
        .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {{ 0.0f, 0.0f, 0.0f, 1.0f }}
    };

    const VkRenderingInfoKHR render_info =
    {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .renderArea = { {0, 0}, extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_info
    };

    vkCmdBeginRendering(commandBuffer, &render_info);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getHandle());

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

    VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getLayout(), 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, mesh.getIndexCount(), 1, 0, 0, 0);

    vkCmdEndRendering(commandBuffer);

    const VkImageMemoryBarrier image_memory_barrier_end
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .image = m_mainView.getImage(imageIndex),
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

    return (vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);
}


void Application::drawFrame() noexcept
{
    auto frame  = m_sync.currentFrame;
    auto device = m_api.getDevice();
    auto queue  = m_api.getQueue();

    vkWaitForFences(device, 1, &m_sync.inFlightFences[frame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, m_mainView.getSwapchain(), UINT64_MAX, m_sync.imageAvailableSemaphores[frame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        printf("failed to acquire swap chain image!");
    }

    updateUniformBuffer(frame);

    vkResetFences(device, 1, &m_sync.inFlightFences[frame]);

    auto commandBuffer = m_commandPool.commandBuffers[frame];
    auto descriptorSet = m_uniformBuffers.descriptorSets[frame];

    vkResetCommandBuffer(m_commandPool.commandBuffers[frame], /*VkCommandBufferResetFlagBits*/ 0);
    writeCommandBuffer(commandBuffer, frame, imageIndex, m_mesh, descriptorSet);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = m_sync.imageAvailableSemaphores.data() + frame;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandPool.commandBuffers[frame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &m_sync.renderFinishedSemaphores[frame];

    if (auto result = vkQueueSubmit(queue, 1, &submitInfo, m_sync.inFlightFences[frame]); result != VK_SUCCESS)
    {
        printf("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &m_sync.renderFinishedSemaphores[frame];
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

    m_sync.currentFrame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    
#ifdef _WIN32
    Sleep(16);
#endif
}