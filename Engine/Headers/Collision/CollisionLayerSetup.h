#pragma once

enum CollisionLayer : unsigned short
{
	LAYER_1 = 1,
	LAYER_2 = 1 << 1,
	LAYER_3 = 1 << 2,
	LAYER_4 = 1 << 3,
	MELEE = 1 << 4,
	LAYER_STATIC = 1 << 5,
	LAYER_BLEND_ENV = 1 << 6,
	LAYER_BLEND = 1 << 7,
};

REFLECT_ENUM(CollisionLayer);

constexpr int CollisionLayerCount = 7;