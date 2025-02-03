#include "Engine.h"
#include "UnitTestsGame.h"

int main(int argc, char* argv[])
{
	Perry::GetEngine().SetGame<UnitTestsGame>();
	Perry::GetEngine().Run();
}
