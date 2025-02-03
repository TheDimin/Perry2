#include "Core.h"

using namespace Perry;

float Easings::easeInOutQuart(float t)
{
	if (t < 0.5f) {
		t *= t;
		return 8 * t * t;
	}
	else {
		t -= 1;
		t = t * t;
		return 1 - 8 * t * t;
	}
}


float Easings::easeInExpo(float t)
{
	return (pow(2.f, 8.f * t) - 1.f) / 255.f;
}

float Easings::easeOutExpo(float t)
{
	return 1.f - pow(2.f, -8.f * t);
}

float Easings::easeInCubic(float t)
{
	return t * t * t;
}
