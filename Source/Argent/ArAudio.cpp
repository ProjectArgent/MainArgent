#include "ArAudio.h"
#include "Other/Misc.h"
namespace Argent::Audio
{
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'


	//チャンクの読み込み 
	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & chunkSize, DWORD & chunkDataPosition)
	{
	    HRESULT hr = S_OK;
	    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
	        return HRESULT_FROM_WIN32( GetLastError() );

	    DWORD chunkType;
	    DWORD chunkDataSize;
	    DWORD RIFFDataSize = 0;
	    DWORD fileType;
	    DWORD bytesRead = 0;
	    DWORD offset = 0;

	    while (hr == S_OK)
	    {
	        DWORD numBytesRead;

			//ReadFileは読むたびにポインタの位置が変わる？　読んだ文だけずれる？ぽい


	    	//chunkType と chunkDataSizeに書き込む
	        if( 0 == ReadFile( hFile, &chunkType, sizeof(DWORD), &numBytesRead, nullptr ) )
	            hr = HRESULT_FROM_WIN32( GetLastError() );

	        if( 0 == ReadFile( hFile, &chunkDataSize, sizeof(DWORD), &numBytesRead, nullptr ) )
	            hr = HRESULT_FROM_WIN32( GetLastError() );

	        switch (chunkType)
	        {
	        case fourccRIFF:
	            RIFFDataSize = chunkDataSize;
	            chunkDataSize = 4; // sizeof(DWORD)が4だから？
	            if( 0 == ReadFile( hFile, &fileType, sizeof(DWORD), &numBytesRead, nullptr ) )
	                hr = HRESULT_FROM_WIN32( GetLastError() );
	            break;

	        default:
	            if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, static_cast<LONG>(chunkDataSize), nullptr, FILE_CURRENT ) )
	            return HRESULT_FROM_WIN32( GetLastError() );            
	        }

			//ファイルを読んだ分だけずらす
	        offset += sizeof(DWORD) * 2;

	        if (chunkType == fourcc)
	        {
	            chunkSize = chunkDataSize;
	            chunkDataPosition = offset;
	            return S_OK;
	        }

			//チャンクタイプが違う場合のみずらす
	        offset += chunkDataSize;

	        if (bytesRead >= RIFFDataSize) return S_FALSE;

	    }
	    return S_OK;
	}

	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD bufferSize, DWORD bufferOffset)
	{
	    HRESULT hr = S_OK;
	    if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, static_cast<LONG>(bufferOffset), nullptr, FILE_BEGIN ) )
	        return HRESULT_FROM_WIN32( GetLastError() );
	    DWORD numBytesRead;
	    if( 0 == ReadFile( hFile, buffer, bufferSize, &numBytesRead, nullptr ) )
	        hr = HRESULT_FROM_WIN32( GetLastError() );
	    return hr;
	}

	ArAudio::ArAudio():
		state(State::Stopping)
	{
		HRESULT hr{ S_OK };

		//オーディオエンジンの作成
		hr = XAudio2Create(audioEngine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		//マスターボイスの作成
		hr = audioEngine->CreateMasteringVoice(&masterVoice);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		const wchar_t* filePath = L"./Resources/Music/maou.wav";


		//ファイルオープン
		HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, nullptr, FILE_BEGIN))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		DWORD chunkSize;
		DWORD chunkPosition;
		//'WAVE' か 'XWMA'　のみOK
		FindChunk(hFile, fourccRIFF, chunkSize, chunkPosition);
		DWORD fileType;
		ReadChunkData(hFile, &fileType, sizeof(DWORD), chunkPosition);
		_ASSERT_EXPR(fileType == fourccWAVE, L"Only support 'WAVE'");


		//fourccFMTのチャンクにあるデータを WAVEFORMATEXTENSIBLE(wfx)にコピーする.
		FindChunk(hFile, fourccFMT, chunkSize, chunkPosition);
		ReadChunkData(hFile, &wfx, chunkSize, chunkPosition);

		//fourccDATAのチャンクにあるデータをコピーする
		FindChunk(hFile, fourccDATA, chunkSize, chunkPosition);
		BYTE* data = new BYTE[chunkSize];
		ReadChunkData(hFile, data, chunkSize, chunkPosition);

		buffer.AudioBytes = chunkSize;  //バッファのサイズ
		buffer.pAudioData = data;  //バッファの中身
		buffer.Flags = XAUDIO2_END_OF_STREAM; //バッファの後ろにはもうデータがないことを明示

		//ソースボイスを作る
		hr = audioEngine->CreateSourceVoice(&sourceVoice, reinterpret_cast<WAVEFORMATEX*>(&wfx));
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));


		XAUDIO2_VOICE_STATE voice_state = {};
		sourceVoice->GetState(&voice_state);

		if (voice_state.BuffersQueued)
		{
			return;
		}
	}

	void ArAudio::Play()
	{
		if(IsPlaying()) return;

		HRESULT hr{ S_OK };
		/// <summary>
		/// 停止した場合はもう一回submitしないと再生できない
		/// </summary>
		if(state == State::Stopping)
		{
			buffer.LoopCount = 10;
			hr = sourceVoice->SubmitSourceBuffer(&buffer);
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		hr = sourceVoice->Start(0);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		state = State::Playing;
	}

	void ArAudio::Stop()
	{
		if(!IsLastAudioQueue()) return;

		HRESULT hr{};
		hr = sourceVoice->Stop();
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		hr = sourceVoice->FlushSourceBuffers();
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		state = State::Stopping;
	}

	void ArAudio::Pause()
	{
		if(!IsLastAudioQueue()) return;

		HRESULT hr{};
		hr = sourceVoice->Stop();
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		state = State::Pausing;
	}
}
