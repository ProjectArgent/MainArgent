#pragma once
#include "Effekseer.h"
#include "../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "ArEffect.h"

namespace Argent::Resource::Effect
{
	class ArEffectManager
	{
	private:

		ArEffectManager();


	public:
		~ArEffectManager() = default;

		void Update();
		void Render() const;

		Effekseer::Manager* GetManager() const { return efkManager.Get();  }  // NOLINT(modernize-use-nodiscard)
		EffekseerRenderer::Renderer* GetRenderer() const { return efkRenderer.Get();  }  // NOLINT(modernize-use-nodiscard)

		static ArEffectManager* Instance() { return instance;  }
		
	private:
		EffekseerRenderer::RendererRef efkRenderer{};
		Effekseer::ManagerRef efkManager{};
		Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
		Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCmdList{};

		static ArEffectManager* instance;
	};
}
