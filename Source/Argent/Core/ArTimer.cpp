#include "ArTimer.h"

#include <sstream>

namespace Argent::Timer
{
	ArTimer::ArTimer()
	{
		LONGLONG countsPerSecond;
		//how many time does this PC counts per seconds
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&countsPerSecond));

		secondsPerCount = 1.0 / static_cast<double>(countsPerSecond); //how many seconds need to finish one count

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		baseTime = thisTime;
		lastTime = thisTime;
	}

	void ArTimer::Start()
	{
		deltaTime = 0;
	}

	void ArTimer::Tick()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&thisTime));
		deltaTime = static_cast<double>(thisTime - lastTime) * secondsPerCount;

		lastTime = thisTime;

		if(deltaTime < 0.0)
		{
			deltaTime = 0.0;
		}
	}

	float ArTimer::Stamp() const
	{
		return static_cast<float>((static_cast<double>(thisTime - baseTime) * secondsPerCount));
	}


	void ArTimer::CalcFrameTime(HWND hWnd)
	{
		const float stamp = Stamp();
		++frames;
		if (stamp - elapsedTime >= 1.0f)
		{
			const float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs  << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)" << "     :  DetaTime :"  << deltaTime;
			SetWindowTextW(hWnd, outs.str().c_str());

			frames = 0;
			elapsedTime += 1.0f;
		}
	}
}
