#include "catch.hpp"

#include "AssetManager.h"
#include "Model.h"

using namespace Perry;

TEST_CASE("AssetManager Unit Tests", "[AssetManager]")
{
    SECTION("AssetManager can get an asset by file path")
    {
        auto asset = AssetManager<Model>::Get("Models/Avocado/Avocado.gltf");

        REQUIRE(asset != nullptr);
    }    

    SECTION("AssetManager caches the asset retrieved so two model instances can be assigned to the same asset")
    {
        AssetManager<Model>::CollectGarbage();

        auto asset1 = AssetManager<Model>::Get("Models/Avocado/Avocado.gltf");
        auto asset2 = AssetManager<Model>::Get("Models/Avocado/Avocado.gltf");

        REQUIRE(asset1 == asset2);
    }
}