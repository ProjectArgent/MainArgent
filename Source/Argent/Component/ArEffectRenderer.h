#pragma once
#include "ArComponent.h"
#include "../Resource/ArEffect.h"

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

		//エフェクトの再生
		void OnPlay() { isPlay = true; }
	private:
		bool isPlay;

		std::shared_ptr<Argent::Resource::Effect::ArEffect> effect;
	};
	
}

