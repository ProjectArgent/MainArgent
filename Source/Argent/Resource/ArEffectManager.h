#pragma once
#include "Effekseer.h"
#include "../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "ArEffect.h"

namespace Argent::Resource::Effect
{
	class ArEffectManager
	{
	public:

		ArEffectManager(ID3D12Device* device, ID3D12CommandQueue* cmdQueue, UINT numBackBuffers);


	public:
		~ArEffectManager() = default;

		void Update();
		void Render() const;

		Effekseer::ManagerRef GetManager() const { return efkManager;  }  // NOLINT(modernize-use-nodiscard)
		EffekseerRenderer::RendererRef GetRenderer() const { return efkRenderer;  }  // NOLINT(modernize-use-nodiscard)

		static ArEffectManager* Instance() { return instance;  }
		
	private:
		EffekseerRenderer::RendererRef efkRenderer{};
		Effekseer::ManagerRef efkManager{};
		Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
		Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCmdList{};

		static ArEffectManager* instance;
	};
}
