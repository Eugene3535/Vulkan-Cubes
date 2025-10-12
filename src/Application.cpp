
#include <cstring>

#include <GLFW/glfw3.h>

#include <stb_image.h>

#include "vulkan_api/utils/Helpers.hpp"
#include "vulkan_api/wrappers/pipeline/stages/shader/ShaderStage.hpp"
#include "vulkan_api/wrappers/render/Render.hpp"
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

    auto instance = m_api.getInstance();
    auto GPU      = m_api.getPhysicalDevice();
    auto device   = m_api.getDevice();

//  Main View
    if(m_mainView.create(m_api, window) != VK_SUCCESS) 
        return false;
    
    {// Pipeline
        std::array<ShaderStage, 2> shaders;

        if(shaders[0].loadFromFile(device, VK_SHADER_STAGE_VERTEX_BIT, "res/shaders/vertex_shader.spv") != VK_SUCCESS)
            return false;

        if(shaders[1].loadFromFile(device, VK_SHADER_STAGE_FRAGMENT_BIT, "res/shaders/fragment_shader.spv") != VK_SUCCESS)
            return false;

        std::array<const VertexInputState::Attribute, 2> attributes =
        {
            VertexInputState::Attribute::Float2,
            VertexInputState::Attribute::Float2
        };

        DescriptorSetLayout uniformDescriptors;
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

        m_descriptorPool = std::make_unique<DescriptorPool>(device);

        {
            std::array<VkDescriptorPoolSize, 1> poolSizes = 
            {
                VkDescriptorPoolSize
                {
                    .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .descriptorCount = MAX_FRAMES_IN_FLIGHT
                }
            };

            if(m_descriptorPool->create(poolSizes) != VK_SUCCESS)
                return false;
        }

        {
            std::array<VkDescriptorSetLayout, MAX_FRAMES_IN_FLIGHT> layouts = 
            { 
                m_pipeline.getDescriptorSetLayout(), 
                m_pipeline.getDescriptorSetLayout() 
            };

            if(m_descriptorPool->allocateDescriptorSets(m_descriptorSets, layouts) != VK_SUCCESS)
                return false;
        }
    }

    if(!m_commandPool.create(device, m_api.getMainQueueFamilyIndex()))
        return false;

    if(!m_sync.create(device)) 
        return false;

    auto queue = m_api.getQueue();
    auto pool = m_commandPool.handle;

    {
        if(!m_texture.loadFromFile("res/textures/container.jpg", GPU, device, pool, queue))
            return false;
                
        VkDescriptorImageInfo imageInfo = 
        {
            .sampler     = m_texture.textureSampler,
            .imageView   = m_texture.textureImageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        m_descriptorPool->writeCombinedImageSampler(&imageInfo, m_descriptorSets[0], 0);
        m_descriptorPool->writeCombinedImageSampler(&imageInfo, m_descriptorSets[1], 0);
    }

    if(!m_mesh.create(GPU, device, pool, queue)) 
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
    m_descriptorPool->destroy();

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


void Application::updateUniformBuffer(uint32_t currentImage, bool b) noexcept
{
    if(b)
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 0));
        glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), m_width / (float)m_height, 0.1f, 100.0f);

        proj[1][1] *= -1;

        m_mvp = proj * view * model;
    }
    else
    {
        static float cnt = 0;
        cnt += 1;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0));
        model = glm::rotate(model, glm::radians(cnt), glm::vec3(0, 0, 1));
        glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), m_width / (float)m_height, 0.1f, 100.0f);

        proj[1][1] *= -1;

        m_mvp = proj * view * model;
    }
}


void Application::writeCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex, const Mesh& mesh, VkDescriptorSet descriptorSet) noexcept
{
    VkDeviceSize offsets[] = {0};
    VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
    
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getHandle());
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getLayout(), 0, 1, &descriptorSet, 0, nullptr);
    vkCmdPushConstants(cmd, m_pipeline.getLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &m_mvp);
    vkCmdDrawIndexed(cmd, mesh.getIndexCount(), 1, 0, 0, 0);
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

    vkResetFences(device, 1, &m_sync.inFlightFences[frame]);

    auto commandBuffer = m_commandPool.commandBuffers[frame];
    auto descriptorSet = m_descriptorSets[frame];

    vkResetCommandBuffer(commandBuffer, /*VkCommandBufferResetFlagBits*/ 0);

    if(Render::begin(commandBuffer, m_mainView, imageIndex) != VK_SUCCESS)
        return;

    updateUniformBuffer(frame, true);
    writeCommandBuffer(commandBuffer, imageIndex, m_mesh, descriptorSet);

    updateUniformBuffer(frame, false);
    writeCommandBuffer(commandBuffer, imageIndex, m_mesh, descriptorSet);

    if(Render::end(commandBuffer, m_mainView, imageIndex) != VK_SUCCESS)
        return;

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