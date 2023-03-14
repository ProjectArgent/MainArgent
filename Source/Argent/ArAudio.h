#pragma once
#include <xaudio2.h>
#include <wrl.h>

namespace Argent::Audio
{
	class ArAudio
	{
	public:
		ArAudio();

		void Play(UINT loopCounts);

		Microsoft::WRL::ComPtr<IXAudio2> audioEngine;
		IXAudio2MasteringVoice* masterVoice{};
		IXAudio2SourceVoice* sourceVoice;

		WAVEFORMATEXTENSIBLE wfx = {0};
		XAUDIO2_BUFFER buffer = {0};
	};
}
