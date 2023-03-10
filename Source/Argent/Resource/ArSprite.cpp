#include "ArSprite.h"

#include "../Graphic/ArGraphics.h"
#include "../Other/ArHelper.h"

namespace Argent::Mesh::Sprite
{
	ArSprite::ArSprite()
	{
		SetIndexCountPerInstance(4);
		ID3D12Device* device = Graphics::ArGraphics::Instance()->GetDevice();
		HRESULT hr{ S_OK };

		std::vector<Vertex> vertices(4);
		vertices.at(0).position = { -1.f,  1.f, 0.0f };//lt
		vertices.at(1).position = {  1.f,  1.f, 0.0f };//rt
		vertices.at(2).position = { -1.f, -1.f, 0.0f };//lb
		vertices.at(3).position = {  1.f, -1.f, 0.0f };//rb
		vertices.at(0).texcoord = { 0.0f, 0.0f };
		vertices.at(1).texcoord = { 1.0f, 0.0f };
		vertices.at(2).texcoord = { 0.0f, 1.0f };
		vertices.at(3).texcoord = { 1.0f, 1.0f };
		vertices.at(0).color = vertices[1].color = vertices[2].color = vertices[3].color = DirectX::XMFLOAT4(1, 1, 1, 1);

		std::vector<uint32_t> indices(4);
		indices.at(0) = 0;
		indices.at(1) = 1;
		indices.at(2) = 2;
		indices.at(3) = 3;
		//uint32_t index[4]{ 0, 1, 2, 3 };

		vertexBuffer = std::make_unique<Argent::Dx12::ArVertexBuffer<Vertex>>(device, vertices, &vertexMap);
		indexBuffer = std::make_unique<Argent::Dx12::ArIndexBuffer>(device, indices);

		//vertexView = Argent::Helper::Dx12::Buffer::CreateVertex<Vertex>(device, 4, std::begin(vertices), std::end(vertices), &vertexMap, 
		//	vertexBuffer.ReleaseAndGetAddressOf(), false);
		//indexView = Argent::Helper::Dx12::Buffer::CreateIndex(device, 4, std::begin(index), std::end(index), indexBuffer.ReleaseAndGetAddressOf());
	}

	void ArSprite::UpdateVertexMap(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale,
	                                   const DirectX::XMFLOAT2& center, float angle, float textureWidth, float textureHeight,
	                                   const DirectX::XMFLOAT4& color) const
	{
		const D3D12_VIEWPORT viewport = Argent::Graphics::ArGraphics::Instance()->GetViewport();
		const DirectX::XMFLOAT2 viewportSize{ viewport.Width, viewport.Height };
		const DirectX::XMFLOAT2 textureSize{ textureWidth,
			textureHeight };


		DirectX::XMFLOAT3 leftTop = { position.x - center.x,					position.y - center.y, 0 };
		DirectX::XMFLOAT3 rightTop = { position.x + textureSize.x - center.x,		position.y - center.y, 0 };
		DirectX::XMFLOAT3 leftBottom = { position.x - center.x,					position.y + textureSize.y - center.y, 0 };
		DirectX::XMFLOAT3 rightBottom = { position.x + textureSize.x - center.x,	position.y + textureSize.y - center.y, 0 };
		const DirectX::XMFLOAT2 Scale{ scale.x, scale.y };
		const DirectX::XMFLOAT2 Center{ position.x - center.x, position.y - center.y };

		leftTop = Helper::Sprite::CalcScale(leftTop, Center, Scale);
		rightTop = Helper::Sprite::CalcScale(rightTop, Center, Scale);
		leftBottom = Helper::Sprite::CalcScale(leftBottom, Center, Scale);
		rightBottom = Helper::Sprite::CalcScale(rightBottom, Center, Scale);

		leftTop = Helper::Sprite::Rotate(leftTop, Center, angle);
		rightTop = Helper::Sprite::Rotate(rightTop, Center, angle);
		leftBottom = Helper::Sprite::Rotate(leftBottom, Center, angle);
		rightBottom = Helper::Sprite::Rotate(rightBottom, Center, angle);


		leftTop = Helper::Sprite::ToNDC(leftTop, viewportSize);
		rightTop = Helper::Sprite::ToNDC(rightTop, viewportSize);
		leftBottom = Helper::Sprite::ToNDC(leftBottom, viewportSize);
		rightBottom = Helper::Sprite::ToNDC(rightBottom, viewportSize);


		vertexMap[0].position = leftTop;
		vertexMap[1].position = rightTop;
		vertexMap[2].position = leftBottom;
		vertexMap[3].position = rightBottom;
		vertexMap[0].texcoord = Helper::Sprite::CalcUVValue(DirectX::XMFLOAT2(), DirectX::XMFLOAT2(), textureSize);
		vertexMap[1].texcoord = Helper::Sprite::CalcUVValue(DirectX::XMFLOAT2(), DirectX::XMFLOAT2(textureSize.x, 0), textureSize);
		vertexMap[2].texcoord = Helper::Sprite::CalcUVValue(DirectX::XMFLOAT2(), DirectX::XMFLOAT2(0, textureSize.y), textureSize);
		vertexMap[3].texcoord = Helper::Sprite::CalcUVValue(DirectX::XMFLOAT2(), textureSize, textureSize);
		vertexMap[0].color =
			vertexMap[1].color =
			vertexMap[2].color =
			vertexMap[3].color = color;
	}
}
