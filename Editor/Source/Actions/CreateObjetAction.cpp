#include "EditorPCH.h"
#pragma hdrstop

using namespace Perry::Editor::Actions;

void CreateObject::Execute()
{
	//TODO we should do some logic that would spawn it infront of the camera by default?
	auto ent = Perry::Entity::Create(*GetEditor().SelectedRegistry);

	GetEditor().SetSelectedEntity(ent);
}
void CreateObject::RollBack()
{
	IAction::RollBack();
}
IMPLEMENT_REFLECT_ACTION(CreateObject)
{

}
FINISH_REFLECT()