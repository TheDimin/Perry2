#pragma once

namespace Perry
{
	namespace Utilities
	{
		typedef long HRESULT;

		void ThrowIfFailed(const HRESULT hr);

		extern float Random01();

		extern float RandomInRange(float min, float max);

		extern int RandomInRange(int min, int max);

		template<typename T>
		inline T RandomInRange(T min, T max)
		{
			return static_cast<T>(min + (max - min) * Random01());
		}

		extern int PackBools(bool b1, bool b2, bool b3, bool b4);

		extern std::string GetSimpleName(const std::string& path);
	}
}
