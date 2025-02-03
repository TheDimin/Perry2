#include "catch.hpp"
#include "FileIO.h"
#include <filesystem>

auto resourcesPath = std::filesystem::current_path().append("Resources\\").string();


TEST_CASE("FileIO Unit Tests", "[FileIO]") {

	SECTION("GetResourcesPath")
	{
		REQUIRE(Perry::FileIO::GetResourcesPath() == resourcesPath);

		/*BENCHMARK("[GetResourcesPath]")
		{
			return Perry::FileIO::GetResourcesPath();
		};*/
	}
	SECTION("ReadBinaryFile")
	{
		auto data = Perry::FileIO::ReadBinaryFile("Textures/Cat.png");
		REQUIRE(data != nullptr);
	}
	SECTION("ReadTextFile")
	{
		auto data = Perry::FileIO::ReadTextFile("Test/test.txt");
		REQUIRE(data == "bunger");
	}
	SECTION("Exists")
	{
		auto res = Perry::FileIO::Exists("Test/test.txt");
		REQUIRE(res);
	}
}