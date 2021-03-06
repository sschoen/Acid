#pragma once

#include "Renderer/RenderPipeline.hpp"
#include "Renderer/Pipelines/PipelineGraphics.hpp"
#include "Renderer/Handlers/UniformHandler.hpp"

namespace acid
{
	class ACID_EXPORT RendererGuis :
		public RenderPipeline
	{
	private:
		PipelineGraphics m_pipeline;
		UniformHandler m_uniformScene;
	public:
		explicit RendererGuis(const GraphicsStage &graphicsStage);

		void Render(const CommandBuffer &commandBuffer, const Camera &camera) override;
	};
}
