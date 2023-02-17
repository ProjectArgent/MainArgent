#pragma once

#include <random>

namespace Argent::Random
{
	inline std::random_device randomDevice;
	inline std::mt19937 randomEngine(randomDevice());

	inline float Generate(float min, float max)
	{
		return min + static_cast<float>(randomEngine()) / (max - min);
	}
}
