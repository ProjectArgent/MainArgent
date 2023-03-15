#pragma once

#include <xaudio2.h>
#include <wrl.h>


namespace Argent::Resource::Audio
{
	class ArAudioManager
	{
	public:
		ArAudioManager();
		virtual ~ArAudioManager() = default;


		IXAudio2* GetAudioEngine() const { return audioEngine.Get();  }
		IXAudio2MasteringVoice* GetMasterVoice() const { return masterVoice; }
	private:
		Microsoft::WRL::ComPtr<IXAudio2> audioEngine;
		IXAudio2MasteringVoice* masterVoice{};

	public:
		static ArAudioManager& Instance()
		{
			static ArAudioManager ins;
			return ins;
		}

	private:
		static ArAudioManager* instance;
	public:
		//static ArAudioManager* Instance()
		//{
		//	return instance;
		//}
	};
	
}

