#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "vulkan_api/wrappers/render_pass/RenderPass.hpp"


RenderPass::RenderPass() noexcept:
    handle(nullptr)
{

}


RenderPass::~RenderPass() = default;


bool RenderPass::create(VkDevice logicalDevice, const Swapchain& swapchain, GLFWwindow* window) noexcept
{
    if(handle)
        return true;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = static_cast<VkFormat>(swapchain.format);
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &handle) != VK_SUCCESS)
        return false;

//  Image views
    if(!swapchain.images.empty())
    {
        for (size_t i = 0; i < swapchain.images.size(); ++i)
        {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapchain.images[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = static_cast<VkFormat>(swapchain.format);
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (VkImageView imageView; (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView) == VK_SUCCESS))
                imageViews[i] = imageView;	
        }
    }

//  Framebuffers
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    for (size_t i = 0; i < imageViews.size(); i++)
    {
        VkImageView attachments = imageViews[i];

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = handle;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &attachments;
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
            return false;
    }
    
    return true;
}


void RenderPass::destroy(VkDevice logicalDevice) noexcept
{
    if(handle)
    {
        for (auto framebuffer : framebuffers)
            vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    
        for (auto imageView : imageViews)
		    vkDestroyImageView(logicalDevice, imageView, nullptr);

        vkDestroyRenderPass(logicalDevice, handle, nullptr);
        handle = nullptr;
    }
}