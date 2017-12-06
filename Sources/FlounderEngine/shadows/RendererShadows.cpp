#include "RendererShadows.hpp"

#include "../devices/Display.hpp"
#include "../entities/Entities.hpp"
#include "../entities/components/ComponentModel.hpp"
#include "UbosShadows.hpp"

namespace Flounder
{
	const DescriptorType RendererShadows::typeUboObject = UniformBuffer::CreateDescriptor(0, VK_SHADER_STAGE_VERTEX_BIT);
	const PipelineCreateInfo RendererShadows::pipelineCreateInfo =
	{
		PIPELINE_POLYGON_NO_DEPTH, // pipelineModeFlags
		VK_POLYGON_MODE_FILL, // polygonMode
		VK_CULL_MODE_FRONT_BIT, // cullModeFlags

		Vertex::GetBindingDescriptions(), // vertexBindingDescriptions
		Vertex::GetAttributeDescriptions(), // vertexAttributeDescriptions

		{ typeUboObject }, // descriptors

		{ "res/shaders/shadows/shadow.vert.spv", "res/shaders/shadows/shadow.frag.spv" } // shaderStages
	};

	RendererShadows::RendererShadows(const int &subpass) :
		IRenderer(),
		m_uniformObject(new UniformBuffer(sizeof(UbosShadows::UboObject))),
		m_pipeline(new Pipeline("shadows", pipelineCreateInfo, subpass))
	{
	}

	RendererShadows::~RendererShadows()
	{
		delete m_pipeline;
	}

	void RendererShadows::Render(const VkCommandBuffer *commandBuffer, const Vector4 &clipPlane, const ICamera &camera)
	{
		vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipeline());

		for (auto terrain : *Terrains::Get()->GetTerrains())
		{
			if (terrain->GetModel(0) != nullptr)
			{
				Matrix4 modelMatrix = Matrix4();
				terrain->GetTransform()->GetWorldMatrix(&modelMatrix);
				RenderModel(commandBuffer, terrain->GetModel(0), modelMatrix);
			}
		}

		for (auto entity : *Entities::Get()->GetEntities()) // Entities::Get()->GetStructure()->GetAll()
		{
			ComponentModel *componentModel = entity->GetComponent<ComponentModel*>();

			if (componentModel != nullptr && componentModel->GetModel() != nullptr)
			{
				Matrix4 modelMatrix = Matrix4();
				entity->GetTransform()->GetWorldMatrix(&modelMatrix);
				RenderModel(commandBuffer, componentModel->GetModel(), modelMatrix);
			}
		}
	}

	void RendererShadows::RenderModel(const VkCommandBuffer *commandBuffer, Model *object, const Matrix4 &modelMatrix)
	{
		const auto logicalDevice = Display::Get()->GetLogicalDevice();
		const auto descriptorSet = m_pipeline->GetDescriptorSet();

		UbosShadows::UboObject uboObject = {};
		uboObject.mvp = Matrix4();
		Matrix4::Multiply(*Shadows::Get()->GetShadowBox()->GetProjectionViewMatrix(), modelMatrix, &uboObject.mvp);
		m_uniformObject->Update(&uboObject);

		std::vector<VkWriteDescriptorSet> descriptorWrites = std::vector<VkWriteDescriptorSet>{ m_uniformObject->GetWriteDescriptor(0, descriptorSet) };
		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		VkDescriptorSet descriptors[1] = { descriptorSet };
		vkCmdBindDescriptorSets(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetPipelineLayout(), 0, 1, descriptors, 0, nullptr);

		object->CmdRender(*commandBuffer);
	}
}