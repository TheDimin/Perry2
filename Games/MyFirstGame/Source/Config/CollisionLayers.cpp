#include "EnginePCH.h"
#include <EnttEditor/MetaReflectImplement.h>
#include "Collision/CollisionLayerSetup.h"

const std::unordered_map<CollisionLayer, CollisionLayer> Perry::CollisionWorld::CollisionMatrix =
{
	{LAYER_1, static_cast <CollisionLayer>(LAYER_STATIC | LAYER_1 | LAYER_2 | LAYER_3)},
	{LAYER_2,static_cast <CollisionLayer>(LAYER_STATIC | LAYER_1 | LAYER_3)},
	{LAYER_3,static_cast <CollisionLayer>(LAYER_STATIC | LAYER_1)},
	{MELEE,static_cast <CollisionLayer>(0)},
	{LAYER_STATIC,static_cast <CollisionLayer>(0)}
};

IMPLEMENT_REFLECT_ENUM(CollisionLayer)
{
	meta.data<CollisionLayer::LAYER_STATIC>("LAYER_STATIC"_hs)
		PROP_DISPLAYNAME("LAYER STATIC");

	meta.data<CollisionLayer::LAYER_1>("LAYER_1"_hs)
		PROP_DISPLAYNAME("LAYER 1");

	meta.data<CollisionLayer::LAYER_2>("LAYER_2"_hs)
		PROP_DISPLAYNAME("LAYER 2");

	meta.data<CollisionLayer::LAYER_3>("LAYER_3"_hs)
		PROP_DISPLAYNAME("LAYER 3");

	meta.data<CollisionLayer::LAYER_4>("LAYER_4"_hs)
		PROP_DISPLAYNAME("LAYER 4");

	meta.data<CollisionLayer::MELEE>("MELEE"_hs)
		PROP_DISPLAYNAME("MELEE");
}
FINISH_REFLECT()