#pragma once
#include <DirectXMath.h>
#include "ArData.h"


namespace Argent::Data
{
	namespace Sprite
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 texcoord;
			DirectX::XMFLOAT4 color;
		};

		//スプライト描画に使う頂点バッファを頂点情報を持ってるクラス
		//hack　こいつにテクスチャのcenter とかtexpos持たせてもいいかも…

		class ArSpriteData:
			public ArData
		{
		public:
			ArSpriteData();

			/**
				 * \brief 頂点情報の更新用
				 * \param position 座標(Z値は無視される)
				 * \param scale スケール(Z値は無視される)
				 * \param center 回転の中心地
				 * \param angle 回転量
				 * \param textureWidth テクスチャの幅
				 * \param textureHeight テクスチャの高さ
				 * \param color 色
				 */
			void UpdateVertexMap(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT2& center, float angle, float textureWidth, float textureHeight, const DirectX::XMFLOAT4& color) const;

		private:
			Vertex* vertexMap{};
		};
	}

}

