#pragma once

#include "Effekseer.h"
#include "../EffekseerRendererDX12/EffekseerRendererDX12.h"

namespace Argent::Effect
{
	class EffectRenderer
	{
	public:
		EffectRenderer();
		~EffectRenderer() = default;


		void Update();
	private:
		EffekseerRenderer::RendererRef efkRenderer{};
		Effekseer::ManagerRef efkManager{};
		Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
		Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCmdList{};

		Effekseer::EffectRef effect{};
		Effekseer::EffectRef effect1{};
		Effekseer::Handle handle{};
		Effekseer::Handle handle1{};
	};
}
