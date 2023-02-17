#pragma once
#include <Windows.h>


namespace Argent::Timer
{
	class ArTimer
	{
	private:
		ArTimer();
		
	public:
		~ArTimer() = default;
		ArTimer(const ArTimer&) = delete;
		ArTimer& operator=(const ArTimer&) = delete;
		ArTimer(ArTimer&&) noexcept = delete;
		ArTimer& operator=(ArTimer&&) noexcept = delete;
		
		static ArTimer& Instance()
		{
			static ArTimer instance;
			return instance;
		}

		void CalcFrameTime(HWND hWnd);
		void Start();
		void Tick();
		float Stamp() const;
		
		
		[[nodiscard]] float DeltaTime() const { return static_cast<float>(deltaTime); }
		[[nodiscard]] double DeltaTimeD() const { return deltaTime; }
		
	private:
		float elapsedTime{};
		UINT frames{};
		double secondsPerCount{ 0.0 };
		double deltaTime{ 0.0 };
		
		LONGLONG thisTime{ 0LL };
		LONGLONG lastTime{ 0LL };
		LONGLONG baseTime{ 0LL };
	};
}
