#pragma once
#include <memory>
#include "ArRenderer.h"
#include "../Resource/ArSprite.h"

namespace Argent::Component::Renderer
{
	//スプライト（画像）描画用のクラス
	//todo 描画に多分texpos とか指定できないようにしてるからスプライトアニメーションが出来ない
	class ArSpriteRenderer final:
		public ArRenderer
	{

	public:
		ArSpriteRenderer(const char* filePath);
		~ArSpriteRenderer() override;


		void Initialize() override;
		void Finalize() override;
		void Begin() override;
		void End() override;
		void Update() override;
		void Render(ID3D12GraphicsCommandList* cmdList) const override;
		
#ifdef _DEBUG
		void DrawDebug() override;
#endif


		[[nodiscard]] Material::ArMaterial* GetMaterial() const { return materials.at(0).get();  }

	private:
		std::unique_ptr<Mesh::Sprite::ArSprite> sprite;
	};
}
