#include "ArAudioPlayer.h"
#include "../Other/ArHelper.h"
#include "../Input/Keyboard.h"


namespace Argent::Component::Audio
{
	AudioPlayer::AudioPlayer(const char* filePath):
		ArComponent(Helper::String::ExtractFileName(filePath, false))
	{
		audio = std::make_shared<Argent::Resource::Audio::ArAudio>(filePath);
	}

	void AudioPlayer::Update()
	{
		if(Argent::Input::Keyboard::Instance().IsKeyPressEnter(Input::Keyboard::Keys::Space))
		{
			audio->Play();
		}
	}

}