#include "Collision/CollisionLayerSetup.h"

const std::map<CollisionLayer, std::vector<CollisionLayer>> CollisionLayerSetup::COLLISIONLAYERSETUP =
{
	{LAYER_1,std::vector<CollisionLayer>{LAYER_STATIC, LAYER_1,LAYER_2,LAYER_3}},
	{LAYER_2,std::vector<CollisionLayer>{LAYER_STATIC, LAYER_1,LAYER_3}},
	{LAYER_3,std::vector<CollisionLayer>{LAYER_STATIC, LAYER_1}},
	{LAYER_STATIC,std::vector<CollisionLayer>{}}
};