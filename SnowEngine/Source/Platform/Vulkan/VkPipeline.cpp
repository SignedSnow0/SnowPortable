#include "VkPipeline.h"
#include "VkCore.h"
#include "VkRenderPass.h"
#include "VkShader.h"
#include "VkSurface.h"
#include "VkBuffers.h"
#include "VkDescriptorSet.h"
#include "Core/Logger.h"

namespace Snow {
    VkPipeline::VkPipeline(const PipelineCreateInfo& createInfo)
        : mShader{ reinterpret_cast<VkShader*>(createInfo.ShaderProgram) } {
        CreatePipelineLayout(createInfo);
        CreatePipeline(createInfo);
    }

    VkPipeline::~VkPipeline() {
        VkCore::Instance()->Device().destroyPipeline(mPipeline);
        
        VkCore::Instance()->Device().destroyPipelineLayout(mPipelineLayout);
    }

    void VkPipeline::Bind() {
        LOG_ASSERT(VkSurface::BoundSurface(), "Trying to bind pipeline without a bound surface");
        VkSurface::BoundSurface()->CommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    }

    DescriptorSet* VkPipeline::CreateDescriptorSet(u32 setIndex) {
        return new VkDescriptorSet(mShader->Layout(setIndex), 2, mPipelineLayout, setIndex); //TODO: get frame count from somewhere
    }

    void VkPipeline::PushConstant(const std::string& name, const void* data, u32 size) {
        for (auto& pushConstant : mShader->PushConstants()) {
            if (pushConstant.Name == name) {
                VkSurface::BoundSurface()->CommandBuffer().pushConstants(mPipelineLayout, pushConstant.Range.stageFlags, pushConstant.Offset, pushConstant.Size, data);
                return;
            }
        }

        LOG_WARN("Trying to push constant \"{}\" that does not exist", name);
    }

    void VkPipeline::CreatePipelineLayout(const PipelineCreateInfo& info) {
        auto layouts = mShader->DescriptorLayouts();
        std::vector<vk::PushConstantRange> pushConstants{};
        for (const auto& pushConstant : mShader->PushConstants()) {
            pushConstants.push_back(pushConstant.Range);
        }

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = static_cast<u32>(layouts.size());
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = static_cast<u32>(pushConstants.size());
        pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();

        mPipelineLayout = VkCore::Instance()->Device().createPipelineLayout(pipelineLayoutInfo);
    }

    void VkPipeline::CreatePipeline(const PipelineCreateInfo& info) {
        static std::vector<vk::DynamicState> dynamicStates{
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        auto bindingDesc = VkVertexBuffer::BindingDescription();
        auto attributeDescs = VkVertexBuffer::AttributeDescriptions();

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescs.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescs.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<f32>(info.TargetPass->Width());
        viewport.height = static_cast<f32>(info.TargetPass->Height());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{ 0, 0 };
        scissor.extent = vk::Extent2D{ info.TargetPass->Width(), info.TargetPass->Height() };

        vk::PipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.viewportCount = 1;
        viewportInfo.pViewports = &viewport;
        viewportInfo.scissorCount = 1;
        viewportInfo.pScissors = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizerInfo{};
        rasterizerInfo.depthClampEnable = VK_FALSE;
        rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerInfo.polygonMode = vk::PolygonMode::eFill;
        rasterizerInfo.lineWidth = 1.0f;
        rasterizerInfo.cullMode = vk::CullModeFlagBits::eBack;
        rasterizerInfo.frontFace = vk::FrontFace::eClockwise;
        rasterizerInfo.depthBiasEnable = VK_FALSE;
        rasterizerInfo.depthBiasConstantFactor = 0.0f;
        rasterizerInfo.depthBiasClamp = 0.0f;
        rasterizerInfo.depthBiasSlopeFactor = 0.0f;

        vk::PipelineMultisampleStateCreateInfo multisamplingInfo{};
        multisamplingInfo.sampleShadingEnable = VK_FALSE;
        multisamplingInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
        multisamplingInfo.minSampleShading = 1.0f;
        multisamplingInfo.pSampleMask = nullptr;
        multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingInfo.alphaToOneEnable = VK_FALSE;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
        colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

        vk::PipelineColorBlendStateCreateInfo colorBlendingInfo{};
        colorBlendingInfo.logicOpEnable = VK_FALSE;
        colorBlendingInfo.logicOp = vk::LogicOp::eCopy;
        colorBlendingInfo.attachmentCount = 1;
        colorBlendingInfo.pAttachments = &colorBlendAttachment;
        colorBlendingInfo.blendConstants[0] = 0.0f;
        colorBlendingInfo.blendConstants[1] = 0.0f;
        colorBlendingInfo.blendConstants[2] = 0.0f;
        colorBlendingInfo.blendConstants[3] = 0.0f;

        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.depthTestEnable = VK_FALSE;
        depthStencilInfo.depthWriteEnable = VK_FALSE;
        depthStencilInfo.depthCompareOp = vk::CompareOp::eLess;
        depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilInfo.minDepthBounds = 0.0f;
        depthStencilInfo.maxDepthBounds = 1.0f;
        depthStencilInfo.stencilTestEnable = VK_FALSE;
        depthStencilInfo.front = vk::StencilOpState{};
        depthStencilInfo.back = vk::StencilOpState{};
        
        if (info.TargetPass->HasDepth()) {
            depthStencilInfo.depthTestEnable = VK_TRUE;
            depthStencilInfo.depthWriteEnable = VK_TRUE;
        }
        
        auto shaders = reinterpret_cast<VkShader*>(info.ShaderProgram)->ShaderStages();
        
        vk::GraphicsPipelineCreateInfo createInfo{};
        createInfo.stageCount = static_cast<u32>(shaders.size());
        createInfo.pStages = shaders.data();
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssemblyInfo;
        createInfo.pViewportState = &viewportInfo;
        createInfo.pRasterizationState = &rasterizerInfo;
        createInfo.pMultisampleState = &multisamplingInfo;
        createInfo.pDepthStencilState = &depthStencilInfo;
        createInfo.pColorBlendState = &colorBlendingInfo;
        createInfo.pDynamicState = &dynamicStateInfo;
        createInfo.layout = mPipelineLayout;
        createInfo.renderPass = reinterpret_cast<VkRenderPass*>(info.TargetPass)->RenderPass();
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = nullptr;
        createInfo.basePipelineIndex = -1;
        
        mPipeline = VkCore::Instance()->Device().createGraphicsPipeline(nullptr, createInfo).value;
    }
}