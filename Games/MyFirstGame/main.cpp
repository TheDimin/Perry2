#include "EnginePCH.h"
#pragma hdrstop
#include "TypeDefs.h"
#include "MyGame.h"

MAIN
{

	Perry::GetEngine().SetGame<MyGame>();
	Perry::GetEngine().Run();
}
