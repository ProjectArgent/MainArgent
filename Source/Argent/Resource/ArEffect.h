#pragma once
#include "ArResource.h"
#include "Effekseer.h"

namespace Argent::Resource::Effect
{
	class ArEffect:
		public Resource::ArImportedResource
	{
	public:
		ArEffect(const char* filePath);
		~ArEffect() override = default;

		void Play(const DirectX::XMFLOAT3& position, int32_t startFrame = 0);
		void Stop() const;
		bool IsExist() const;
	private:

		Effekseer::EffectRef effect{};
		Effekseer::Handle handle;
	};
	
}

