#include "ArRandom.h"
#include <random>


namespace Argent::Random
{
	static std::random_device randomDevice;
	static std::mt19937 randomEngine(randomDevice());

	float Generate(float min, float max)
	{
		return min + static_cast<float>(randomEngine()) / (max - min);
	}
}