#pragma once
#include <memory>
#include <d3d12.h>
#include "../ArComponent.h"
#include "Data/ArMaterial.h"
#include "../../../Graphic/ArShader.h"
#include "../../../Graphic/Dx12/ArPipelineState.h"
#include "../../../Graphic/ArRenderingPipeline.h"


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

			/**
			 * \brief 継承して使う　元の関数は一番上で呼び出すこと
			 * \param cmdList 
			 */
			virtual void Render(ID3D12GraphicsCommandList* cmdList) const;
			void Render() override;

#ifdef _DEBUG
			void DrawDebug() override{}
#endif
		protected:
			std::unique_ptr<Material::ArMaterial> material;
			//std::unique_ptr<Dx12::ArRenderingPipeline> renderingPipeline;
			std::shared_ptr<Argent::Graphics::RenderingPipeline::ArBaseRenderingPipeline> renderingPipeline;
		};
	}
}
