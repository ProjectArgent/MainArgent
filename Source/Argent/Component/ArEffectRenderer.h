#pragma once
#include "ArComponent.h"
#include "../Resource/ArEffect.h"
#include "ArColor.h"

namespace Argent::Component::Renderer
{
	class ArEffectRenderer:
		public ArComponent
	{
	public:
		ArEffectRenderer(const char* filePath, const char* materialPath);
		~ArEffectRenderer() override = default;

		void Update() override;
		void Render() const override;
#ifdef _DEBUG
		void DrawDebug() override;
#endif
		//エフェクトの再生
		void OnPlay() { isPlay = true; }
	private:
		bool isPlay;
		Argent::Color color;

		std::shared_ptr<Argent::Resource::Effect::ArEffect> effect;
	};
	
}

