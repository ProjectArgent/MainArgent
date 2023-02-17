#pragma once
#include <memory>
#include <d3d12.h>
#include "../ArComponent.h"
#include "Data/ArMaterial.h"
#include "../../../Graphic/ArShader.h"
#include "../../../Graphic/Dx12/PipelineState.h"


//描画用のコンポーネント
namespace Argent::Component
{
	namespace Renderer
	{
		class ArRenderer : public ArComponent
		{
		public:
			ArRenderer(const std::string& name);
			virtual ~ArRenderer() override;


			void Initialize() override {}

			void Render(ID3D12GraphicsCommandList* cmdList) const;
			void Render() override {}

#ifdef _DEBUG
			void DrawDebug() override{}
#endif
		protected:
			std::unique_ptr<Material::ArMaterial> material;
			std::unique_ptr<Dx12::ArRenderingPipeline> renderingPipeline;
		};
	}
}
