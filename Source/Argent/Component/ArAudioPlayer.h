#pragma once

#include "ArComponent.h"
#include <memory>
#include "../Resource/ArAudio.h"
namespace Argent::Component::Audio
{
	class AudioPlayer:
		public ArComponent
	{
	public:
		AudioPlayer(const char* filePath);
		~AudioPlayer() override = default;

		void Update() override;

	private:
		std::shared_ptr<Argent::Resource::Audio::ArAudio> audio;

	};
}
