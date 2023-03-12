#pragma once

#include "Effekseer.h"
#include "../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "../3rdParty/LLGI/src/DX12/LLGI.BaseDX12.h"
#include "../3rdParty/LLGI/src/DX12/LLGI.CommandListDX12.h"
#include "../3rdParty/LLGI/src/DX12/LLGI.GraphicsDX12.h"
#include "../3rdParty/LLGI/src/Utils/LLGI.CommandListPool.h"

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
		Effekseer::Handle handle{};
	};
}
