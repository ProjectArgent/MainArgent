#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl.h>
#include "../ArGraphics.h"
#include "../../Other/Misc.h"
#include "../../Other/ArHelper.h"

namespace Argent
{
	namespace Dx12
	{
		class ArRenderingPipeline
		{
		public:

		protected:
			ArRenderingPipeline(const char* vsFileName, const char* psFileName):
				pipelineState(nullptr)
			,	rootSignature(nullptr)
			,	vertexShader(nullptr)
			,	pixelShader(nullptr)
			{
				LoadShader(vsFileName, psFileName);
			}

		public:
			const ArRenderingPipeline& operator=(const ArRenderingPipeline&) = delete;
			const ArRenderingPipeline& operator=(const ArRenderingPipeline&&) = delete;
			virtual ~ArRenderingPipeline() = default;

			ArRenderingPipeline(const ArRenderingPipeline&) = delete;
			ArRenderingPipeline(const ArRenderingPipeline&&) = delete;


		private:
			HRESULT CreateRootSignature(ID3D12Device* device);
			HRESULT CreatePipelineState(ID3D12Device* device);

		public:

			void SetOnCommand(ID3D12GraphicsCommandList* cmdList) const;

			/**
			 * \brief 
			 * \tparam T åpè≥ÇµÇΩGraphicsPipelineÉNÉâÉX
			 * \param ppPipeline 
			 * \param vsFileName 
			 * \param psFileName 
			 */
			template <class T>
			static void CreateGraphicsPipeline(std::unique_ptr<ArRenderingPipeline>* ppPipeline, const char* vsFileName, const char* psFileName)
			{
				*ppPipeline = std::make_unique<T>(vsFileName, psFileName);
				ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();

				HRESULT hr{ S_OK };
				(*ppPipeline)->SetUpRootSigDesc();
				hr = (*ppPipeline)->CreateRootSignature(device);
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
				(*ppPipeline)->SetUpInputElement();
				(*ppPipeline)->SetUpPipelineDesc();
				hr = (*ppPipeline)->CreatePipelineState(device);
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				(*ppPipeline)->range.clear();
				(*ppPipeline)->rootParam.clear();
				(*ppPipeline)->inputElementDesc.clear();
			}
		protected:

			void LoadShader(const char* vsFilename, const char* psFilename)
			{
				vertexShader = std::make_unique<Shader::ArShader>(vsFilename);
				pixelShader = std::make_unique<Shader::ArShader>(psFilename);
			}


			Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState{};
			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{};
			std::unique_ptr<Shader::ArShader> vertexShader{};
			std::unique_ptr<Shader::ArShader> pixelShader{};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
			D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
			//static constexpr UINT numSampler = 3;
			std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDesc{};
			std::vector<D3D12_DESCRIPTOR_RANGE> range{};
			std::vector<D3D12_ROOT_PARAMETER> rootParam{};
			std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDesc{};
		private:

			void SetUpPipelineDesc();
			virtual void SetUpRootSigDesc() = 0;
			virtual void SetUpInputElement() = 0;
			virtual D3D12_RASTERIZER_DESC SetUpRasterizerDesc()
			{
				D3D12_RASTERIZER_DESC rasDesc{};
				rasDesc.MultisampleEnable = FALSE;
				rasDesc.FrontCounterClockwise = TRUE;
				rasDesc.CullMode = D3D12_CULL_MODE_BACK;
				rasDesc.FillMode = D3D12_FILL_MODE_SOLID;
				rasDesc.DepthClipEnable = TRUE;
				return rasDesc;
			}
			virtual D3D12_BLEND_DESC SetUpBlendState()
			{
				D3D12_BLEND_DESC desc{};
				desc.AlphaToCoverageEnable = FALSE;
				desc.IndependentBlendEnable = FALSE;
				desc.RenderTarget[0] = Helper::Dx12::Blend::GenerateRenderTargetBlendDesc(Helper::Dx12::Blend::BlendMode::bAlpha);
				return desc;
			}
			virtual DXGI_SAMPLE_DESC SetUpSamplerDesc()
			{
				DXGI_SAMPLE_DESC desc{};
				desc.Count = 1;
				desc.Quality = 0;
				return desc;
			}
			virtual D3D12_DEPTH_STENCIL_DESC SetUpDepthStencilState()
			{
				D3D12_DEPTH_STENCIL_DESC desc{};
				desc.DepthEnable = TRUE;
				desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
				desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				return desc;
			}
			virtual DXGI_FORMAT SetUpDSVFormat()
			{
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			}
		};


		namespace Sprite
		{
			class ArDefaultGraphicsPipeline:
				public ArRenderingPipeline
			{
			public:
				ArDefaultGraphicsPipeline(const char* vsFileName, const char* psFileName):
					ArRenderingPipeline(vsFileName, psFileName)
				{}

				ArDefaultGraphicsPipeline(const ArDefaultGraphicsPipeline&) = delete;
				ArDefaultGraphicsPipeline(const ArDefaultGraphicsPipeline&&) = delete;
				ArDefaultGraphicsPipeline operator=(const ArDefaultGraphicsPipeline&) = delete;
				ArDefaultGraphicsPipeline operator=(const ArDefaultGraphicsPipeline&&) = delete;

				~ArDefaultGraphicsPipeline() override = default;

			private:
				void SetUpRootSigDesc() override;
				void SetUpInputElement() override
				{
					inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
					inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));
					inputElementDesc.emplace_back(Helper::Dx12::InputElement::GenerateInputLayoutDesc("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT));
				}
				D3D12_RASTERIZER_DESC SetUpRasterizerDesc() override
				{
					D3D12_RASTERIZER_DESC desc{};
					desc.MultisampleEnable = FALSE;
					desc.DepthClipEnable = FALSE;
					desc.CullMode = D3D12_CULL_MODE_NONE;
					desc.FillMode = D3D12_FILL_MODE_SOLID;
					return desc;
				}
				D3D12_DEPTH_STENCIL_DESC SetUpDepthStencilState() override
				{
					D3D12_DEPTH_STENCIL_DESC desc{};
					desc.DepthEnable = false;
					return desc;
				}
				DXGI_FORMAT SetUpDSVFormat() override
				{
					return DXGI_FORMAT_UNKNOWN;
				}
			};
		}

		namespace Mesh
		{
			class ArDefaultGraphicsPipeline:
				public ArRenderingPipeline
			{
			public:
				ArDefaultGraphicsPipeline(const char* vsFileName = "Resource/Shader/GeometricPrimitiveVertex.cso", const char* psFileName = "Resource/Shader/GeometricPrimitivePixel.cso"):
					ArRenderingPipeline(vsFileName, psFileName)
				{}


				ArDefaultGraphicsPipeline(const ArDefaultGraphicsPipeline&) = delete;
				ArDefaultGraphicsPipeline(const ArDefaultGraphicsPipeline&&) = delete;
				ArDefaultGraphicsPipeline operator=(const ArDefaultGraphicsPipeline&) = delete;
				ArDefaultGraphicsPipeline operator=(const ArDefaultGraphicsPipeline&&) = delete;

				~ArDefaultGraphicsPipeline() override = default;

			private:
				void SetUpRootSigDesc() override;
				void SetUpInputElement() override;
			};
		}

		namespace SkinnedMesh
		{
			class ArDefaultGraphicsPipeline:
				public ArRenderingPipeline
			{
			public:
				ArDefaultGraphicsPipeline(	const char* vsFileName = "Resource/Shader/SkinnedMeshVertex.cso",
											const char* psFileName = "Resource/Shader/SkinnedMeshPixel.cso"):
					ArRenderingPipeline(vsFileName, psFileName)
				{}


			private:
				void SetUpRootSigDesc() override;
				void SetUpInputElement() override;
			};
		}
	}
}
	