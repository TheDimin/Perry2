#pragma once

namespace Perry {
	class Easings
	{
	public:
		static float easeInOutQuart(float t);
		static float easeInExpo(float t);
		static float easeOutExpo(float t);
		static float easeInCubic(float t);
	};
}