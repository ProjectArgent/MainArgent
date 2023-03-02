#include "ArHelper.h"
#include <DirectXTex.h>
#include <wrl/client.h>

#include "Misc.h"
#include "WICTextureLoader12.h"
#include "ArResourceManager.h"
#include "../Graphic/ArGraphics.h"

namespace Argent
{
	namespace Helper
	{
		namespace Sprite
		{
			DirectX::XMFLOAT3 Rotate(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& center, float angle)
			{
				DirectX::XMFLOAT3 tmp = pos;
				tmp.x -= center.x;
				tmp.y -= center.y;

				const float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
				const float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
				const float tx{ tmp.x };
				const float ty{ tmp.y };

				tmp.x = cos * tx - sin * ty;
				tmp.y = sin * tx + cos * ty;

				tmp.x += center.x;
				tmp.y += center.y;
				tmp.z = 0;

				return tmp;
			}
			DirectX::XMFLOAT3 ToNDC(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& windowSize)
			{
				DirectX::XMFLOAT3 tmp{ 0, 0, 0 };
				tmp.x = 2.0f * pos.x / windowSize.x - 1.0f;
				tmp.y = 1.0f - 2.0f * pos.y / windowSize.y;
				tmp.z = 0;
				tmp = { 2.0f * pos.x / windowSize.x - 1.0f, 1.0f - 2.0f * pos.y / windowSize.y , 0 };
				return tmp;
			}
			DirectX::XMFLOAT2 CalcUVValue(const DirectX::XMFLOAT2& texPos, const DirectX::XMFLOAT2& texSize,
				const DirectX::XMFLOAT2& rowTextureSize)
			{
				DirectX::XMFLOAT2 tmp{};

				tmp.x = texPos.x + texSize.x;
				tmp.y = texPos.y + texSize.y;
				tmp.x /= rowTextureSize.x;
				tmp.y /= rowTextureSize.y;

				return tmp;
			}



			DirectX::XMFLOAT3 CalcScale(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT2& center,
				const DirectX::XMFLOAT2& scale)
			{
				DirectX::XMFLOAT3 tmp{ pos };
				const DirectX::XMFLOAT2 dist{ pos.x - center.x, pos.y - center.y };

				tmp.x = center.x + dist.x * scale.x;
				tmp.y = center.y + dist.y * scale.y;
				tmp.z = 0;



				return tmp;
			}
		}

		namespace Math
		{
			std::vector<float> GetGaussianWeights(size_t count, float s)
			{
				std::vector<float> weights(count);
				float x{}, total{};

				for (auto& w : weights)
				{
					w = expf(-(x * x) / (2 * s * s));
					total += w;
					x += 1.0f;
				}

				total = total * 2 - 1;

				for (auto& w : weights)
				{
					w /= total;
				}

				return weights;
			}
		}

		namespace String
		{
			std::string GetTexturePathFromModelAndPath(const std::string& modelPath, const char* texturePath)
			{
				auto folderPath = modelPath.substr(0, modelPath.rfind('/'));
				return folderPath + "/" + texturePath;
			}

			//モデルパスからテクスチャパスへの変更
			// todo filesystemを使ったスマートな形へ変更する
			std::wstring GetWideStringFromString(const std::string& str)
			{
				auto num1 = MultiByteToWideChar(CP_ACP,
					MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
					str.c_str(), -1, nullptr, 0);

				std::wstring wstr;
				wstr.resize(num1);

				auto num2 = MultiByteToWideChar(CP_ACP,
					MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
					str.c_str(), -1, &wstr[0], num1);

				assert(num1 == num2);
				return wstr;
			}

			//拡張子を取得
			std::string GetExtension(const std::string& path)
			{
				size_t index = path.rfind(".");
				std::string r = path.substr(index + 1, path.length() - index - 1);
				return  r;
			}

			std::pair<std::string, std::string> SplitFileName(const std::string& path, const char splitter)
			{
				size_t index = path.find(splitter);
				std::pair<std::string, std::string> ret;
				ret.first = path.substr(0, index);
				ret.second = path.substr(index + 1, path.length() - index - 1);

				return ret;
			}
		}

		namespace FBX
		{
			DirectX::XMFLOAT4X4 ToFloat4x4(const FbxAMatrix& fbxMatrix)
			{
				DirectX::XMFLOAT4X4 f4x4{};
				for (int row = 0; row < 4; ++row)
				{
					for (int column = 0; column < 4; ++column)
					{
						f4x4.m[row][column] = static_cast<float>(fbxMatrix[row][column]);
					}
				}
				return f4x4;
			}

			DirectX::XMFLOAT4 ToFloat4(const FbxDouble4& fbxDouble)
			{
				DirectX::XMFLOAT4 f4{};
				f4.x = static_cast<float>(fbxDouble[0]);
				f4.y = static_cast<float>(fbxDouble[1]);
				f4.z = static_cast<float>(fbxDouble[2]);
				f4.w = static_cast<float>(fbxDouble[3]);
				return f4;
			}

			DirectX::XMFLOAT3 Tofloat3(const FbxDouble3& fbxDouble)
			{
				DirectX::XMFLOAT3 f3{};
				f3.x = static_cast<float>(fbxDouble[0]);
				f3.y = static_cast<float>(fbxDouble[1]);
				f3.z = static_cast<float>(fbxDouble[2]);
				return f3;
			}

		}

		namespace Texture
		{
			HRESULT LoadTexture(ID3D12Device* device, Argent::Dx12::ArCommandBundle* cmdBundle, Argent::Dx12::ArCommandQueue* cmdQueue, const wchar_t* filepath, ID3D12Resource** resource)
			{
				DirectX::TexMetadata metaData{};
				DirectX::ScratchImage scratchImage{};

				HRESULT hr{ S_OK };

				//すでにロードしたテクスチャか確認
				if (Argent::Resource::ArResourceManager::Instance().FindTexture(filepath, resource)) return S_OK;

				hr = DirectX::LoadFromWICFile(filepath, DirectX::WIC_FLAGS_NONE, &metaData, scratchImage);



				//todo 直せ
				if(FAILED(hr))
				{
					hr =Argent::Graphics::ArGraphics::Instance()->CreateWhiteTexture(resource);
					_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
					return hr;
				}
				{
					auto rowImage = scratchImage.GetImage(0, 0, 0);
					//中間バッファ
					Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
					{
						D3D12_HEAP_PROPERTIES uploadHeapProp{};
						uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
						uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
						uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
						uploadHeapProp.CreationNodeMask = 0;
						uploadHeapProp.VisibleNodeMask = 0;

						D3D12_RESOURCE_DESC resDesc{};
						resDesc.Format = DXGI_FORMAT_UNKNOWN;
						resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
						resDesc.Width = Argent::Helper::Math::CalcAlignmentSize(rowImage->rowPitch) * rowImage->height;
						resDesc.Height = 1;
						resDesc.DepthOrArraySize = 1;
						resDesc.MipLevels = 1;
						resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
						resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
						resDesc.SampleDesc.Count = 1;
						resDesc.SampleDesc.Quality = 0;
						hr = device->CreateCommittedResource(&uploadHeapProp, D3D12_HEAP_FLAG_NONE,
							&resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf()));
						_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
					}

					//アップロード用のバッファ
					{
						D3D12_HEAP_PROPERTIES heapProp{};
						heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
						heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
						heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
						heapProp.CreationNodeMask = 0;
						heapProp.VisibleNodeMask = 0;

						D3D12_RESOURCE_DESC resDesc{};
						resDesc.Format = rowImage->format;
						resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metaData.dimension);
						resDesc.Width = metaData.width;
						resDesc.Height = static_cast<UINT>(metaData.height);
						resDesc.DepthOrArraySize = static_cast<UINT16>(metaData.arraySize);
						resDesc.MipLevels = static_cast<UINT16>(metaData.mipLevels);
						resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
						resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
						resDesc.SampleDesc.Count = 1;
						resDesc.SampleDesc.Quality = 0;

						hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
							&resDesc, D3D12_RESOURCE_STATE_COPY_DEST,
							nullptr,
							IID_PPV_ARGS(resource));
						_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
					}


					//テクスチャのマップとコピー
					{
						uint8_t* mapForImage = nullptr;	//texture->Image.format
						auto srcAddress = rowImage->pixels;
						UINT rowPitch = static_cast<UINT>(Argent::Helper::Math::CalcAlignmentSize(rowImage->rowPitch));
						hr = uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mapForImage));
						_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));;
						//todo もっといい方法の模索
						for (int y = 0; y < rowImage->height - 1; ++y)
						{
							std::copy_n(srcAddress, rowPitch, mapForImage);
							srcAddress += rowImage->rowPitch;
							mapForImage += rowPitch;
						}
						uploadBuffer->Unmap(0, nullptr);


						//コピー元
						D3D12_TEXTURE_COPY_LOCATION src{};
						src.pResource = uploadBuffer.Get();
						src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
						src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metaData.width);
						src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metaData.height);
						src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metaData.depth);
						src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(Argent::Helper::Math::CalcAlignmentSize(rowImage->rowPitch));
						src.PlacedFootprint.Footprint.Format = rowImage->format;


						//コピー先
						D3D12_TEXTURE_COPY_LOCATION dst{};
						dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						dst.pResource = *resource;
						dst.SubresourceIndex = 0;


						cmdBundle->cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


						D3D12_RESOURCE_BARRIER barrier{};
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Transition.pResource = *resource;
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
						barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
						barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

						cmdBundle->cmdList->ResourceBarrier(1, &barrier);
						cmdBundle->cmdList->Close();


						ID3D12CommandList* cmdlists[]{ cmdBundle->cmdList.Get() };
						cmdQueue->Execute(1, cmdlists);
						cmdQueue->SetFence();
					}
				}

				cmdBundle->cmdAlloc.Get()->Reset();
				cmdBundle->cmdList.Get()->Reset(cmdBundle->cmdAlloc.Get(), nullptr);
				return hr;
			}
		}

		namespace Dx12
		{
			namespace Blend
			{
				D3D12_RENDER_TARGET_BLEND_DESC GenerateRenderTargetBlendDesc(BlendMode mode)
				{
					D3D12_RENDER_TARGET_BLEND_DESC rtvBlendDesc{};
					rtvBlendDesc.LogicOpEnable = FALSE;
					rtvBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


					switch (mode)
					{
					case BlendMode::bNone:
						rtvBlendDesc.BlendEnable = false;
						break;

					case BlendMode::bAlpha:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
						rtvBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
						break;

					case BlendMode::bAdd:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
						rtvBlendDesc.DestBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
						break;

					case BlendMode::bSubtract:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
						rtvBlendDesc.DestBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
						break;

					case BlendMode::bMultiply:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_DEST_COLOR;
						rtvBlendDesc.DestBlend = D3D12_BLEND_ZERO;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_DEST_ALPHA;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
						break;

					case BlendMode::bLighten:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.DestBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_MAX;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_MAX;
						break;

					case BlendMode::bDarker:
						rtvBlendDesc.BlendEnable = TRUE;
						rtvBlendDesc.SrcBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.DestBlend = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOp = D3D12_BLEND_OP_MIN;
						rtvBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
						rtvBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_MIN;
						break;
					default:
						rtvBlendDesc.BlendEnable = FALSE;
						break;
					}
					return rtvBlendDesc;
				}
			}

			namespace Sampler
			{
				D3D12_STATIC_SAMPLER_DESC GenerateSamplerDesc(FilterMode fMode, WrapMode wMode, D3D12_STATIC_BORDER_COLOR borderColor, FLOAT maxLOD, FLOAT minLOD)
				{
					D3D12_STATIC_SAMPLER_DESC samplerDesc{};

					switch (fMode)
					{
					case FilterMode::fPoint:
						samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
						break;
					case FilterMode::fLinear:
						samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
						break;
					case FilterMode::fAnisotropic:
						samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
						break;
					}

					switch (wMode)
					{
					case WrapMode::wRepeat:
						samplerDesc.AddressU =
							samplerDesc.AddressV =
							samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
						break;

					case WrapMode::wClamp:
						samplerDesc.AddressU =
							samplerDesc.AddressV =
							samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
						break;

					case WrapMode::wMirror:
						samplerDesc.AddressU =
							samplerDesc.AddressV =
							samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
						break;

					case WrapMode::wMirrorOnce:
						samplerDesc.AddressU =
							samplerDesc.AddressV =
							samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
						break;

					case WrapMode::wBorder:
						samplerDesc.AddressU =
							samplerDesc.AddressV =
							samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
						break;
					}

					samplerDesc.BorderColor = borderColor;
					samplerDesc.MaxLOD = maxLOD;
					samplerDesc.MinLOD = minLOD;


					samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
					samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

					return samplerDesc;
				}
			}

		}
	}

}


