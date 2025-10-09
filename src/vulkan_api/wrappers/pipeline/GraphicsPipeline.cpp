#include <array>

#include <glm/mat4x4.hpp>

#include "vulkan_api/wrappers/presentation/MainView.hpp"
#include "vulkan_api/wrappers/pipeline/GraphicsPipeline.hpp"


struct GraphicsPipelineStages
{
    std::vector<VkPipelineShaderStageCreateInfo> shaders;
    std::unique_ptr<VertexInputState>            vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo       inputAssembly;
    VkPipelineViewportStateCreateInfo            viewportState;
    VkPipelineRasterizationStateCreateInfo       rasterizer;
    VkPipelineMultisampleStateCreateInfo         multisampling;
    VkPipelineColorBlendAttachmentState          colorBlending;
    DescriptorSetLayout                          layoutInfo;
};


GraphicsPipeline::State* GraphicsPipeline::State::setupShaderStages(std::span<const ShaderStage> shaders) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    if(!shaders.empty())
    {
        for(const auto shader : shaders)
            stages->shaders.push_back(shader.getInfo());
    }

    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupVertexInput(std::span<const VertexInputState::Attribute> attributes) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());
    stages->vertexInputState = std::make_unique<VertexInputState>(attributes);

    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupInputAssembler(const VkPrimitiveTopology primitive) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->inputAssembly = VkPipelineInputAssemblyStateCreateInfo
    {    
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = primitive,
        .primitiveRestartEnable = VK_FALSE
    };

    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupViewport() noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->viewportState = VkPipelineViewportStateCreateInfo     
    {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr,
        .scissorCount  = 1,
        .pScissors     = nullptr
    };
    
    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupRasterization(VkPolygonMode mode) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->rasterizer = VkPipelineRasterizationStateCreateInfo
    {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = mode,
        .cullMode                = VK_CULL_MODE_FRONT_BIT,
        .frontFace               = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp          = 0.f,
        .depthBiasSlopeFactor    = 0.f,
        .lineWidth               = 1.f
    };
    
    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupMultisampling() noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->multisampling = VkPipelineMultisampleStateCreateInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };
    
    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupColorBlending(VkBool32 enabled) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->colorBlending = VkPipelineColorBlendAttachmentState
    {
        .blendEnable         = enabled,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    
    return this;
}


GraphicsPipeline::State* GraphicsPipeline::State::setupDescriptorSetLayout(const DescriptorSetLayout& uniformDescriptorSet) noexcept
{
    if(!m_data)
        m_data = std::make_shared<GraphicsPipelineStages>();

    auto stages = static_cast<GraphicsPipelineStages*>(m_data.get());

    stages->layoutInfo = uniformDescriptorSet;
    
    return this;
}



GraphicsPipeline::GraphicsPipeline() noexcept:
    m_descriptorSetLayout(nullptr),
    m_layout(nullptr),
    m_handle(nullptr)
{

}


VkResult GraphicsPipeline::create(const class MainView& view, const GraphicsPipeline::State& state) noexcept
{
    auto stages = static_cast<GraphicsPipelineStages*>(state.m_data.get());

    if(!stages)
        return VK_ERROR_INITIALIZATION_FAILED;

    auto device = view.getVulkanApi()->getDevice();
    destroy(device);

    const VkFormat format = view.getFormat();

    const VkPipelineRenderingCreateInfoKHR pipelineRenderingInfo =
    {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &format
    };

    const auto& shaderStages  = stages->shaders;
    const auto vertexInput    = stages->vertexInputState->getinfo();
    const auto& inputAssembly = stages->inputAssembly;
    const auto& viewportState = stages->viewportState;
    const auto& rasterizer    = stages->rasterizer;
    const auto& multisampling = stages->multisampling;

    VkPipelineColorBlendStateCreateInfo colorBlending
    {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &stages->colorBlending
    };

    std::array<VkDynamicState, 2> dynamicStates = 
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = 
    {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates    = dynamicStates.data()
    };

    const VkDescriptorSetLayoutCreateInfo layoutInfo = stages->layoutInfo.getInfo();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
        return VK_ERROR_INITIALIZATION_FAILED;


    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; 
    pushConstantRange.offset = 0; 
    pushConstantRange.size = sizeof(glm::mat4);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = 
    {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = 1,
        .pSetLayouts            = &m_descriptorSetLayout,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges    = &pushConstantRange
    };

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS)
        return VK_ERROR_INITIALIZATION_FAILED;

    VkGraphicsPipelineCreateInfo pipelineInfo = 
    {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipelineRenderingInfo,
        .flags               = 0,
        .stageCount          = static_cast<uint32_t>(shaderStages.size()),
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInput,
        .pInputAssemblyState = &inputAssembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = &dynamicState,
        .layout              = m_layout,
        .renderPass          = nullptr,
        .subpass             = 0,
        .basePipelineHandle  = nullptr,
        .basePipelineIndex   = 0
    };

    return vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_handle);
}


void GraphicsPipeline::destroy(VkDevice device) noexcept
{
    if(m_handle)
    {
        vkDestroyPipeline(device, m_handle, nullptr);
        vkDestroyPipelineLayout(device, m_layout, nullptr);
        vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);

        m_handle = nullptr;
        m_layout = nullptr;
        m_descriptorSetLayout = nullptr;
    }
}


VkDescriptorSetLayout GraphicsPipeline::getDescriptorSetLayout() const noexcept
{
    return m_descriptorSetLayout;
}


VkPipelineLayout GraphicsPipeline::getLayout() const noexcept
{
    return m_layout;
}


VkPipeline GraphicsPipeline::getHandle() const noexcept
{
    return m_handle;
}