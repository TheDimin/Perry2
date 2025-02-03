#include "pch.h"
#include <exception>
#pragma hdrstop


using namespace Perry;

void Utilities::ThrowIfFailed(const HRESULT hr)
{
	if (FAILED(hr))
		throw std::exception();
}
float Utilities::Random01()
{
	//	std::uniform_int_distribution<int> dice_distribution(0, RAND_MAX);
	//	static std::mt19937 random_number_engine; // pseudorandom number generator
	//	auto dice_roller = std::bind(dice_distribution, random_number_engine);

	return float(rand() / (RAND_MAX + 1.0));
}
float Utilities::RandomInRange(float min, float max)
{
	return min + (max - min) * Random01();
}
int Utilities::RandomInRange(int min, int max)
{
	return static_cast<int>(min + (max - min) * Random01());
}
int Utilities::PackBools(bool b1, bool b2, bool b3, bool b4)
{
	int packedValue = 0;
	packedValue |= (b1 << 0);
	packedValue |= (b2 << 1);
	packedValue |= (b3 << 2);
	packedValue |= (b4 << 3);
	return packedValue;
}

std::string Utilities::GetSimpleName(const std::string& path)
		{
			auto simpleName = path;
			//Remove Extension
			auto pos = simpleName.find_last_of('.');
			if (pos != std::string::npos)
				simpleName = simpleName.substr(0, pos - 1);

			pos = simpleName.find_last_of('\\');

			// Check if a backslash was found
			if (pos != std::string::npos) {
				simpleName = simpleName.substr(pos + 1);
			}

			pos = simpleName.find_last_of('/');

			// Check if a backslash was found
			if (pos != std::string::npos) {
				simpleName = simpleName.substr(pos + 1);
			}

			return simpleName;
		}