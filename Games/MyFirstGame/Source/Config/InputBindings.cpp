#include "EnginePCH.h"
#include "Input.h"
#include "KeyCodes.h"

using namespace Perry;

void Input::SetupBindings()
{	//FlyCameraSystem bindings
	CreateAxis("cameraFront").Key(KEY_S, KEY_W).Stick(LEFTY);
	CreateAxis("cameraUp").Key(KEY_Q, KEY_E).Key(GAMEPAD_R2, GAMEPAD_L2);
	CreateAxis("cameraSide").Key(KEY_A, KEY_D).Stick(LEFTX);
	CreateAxis("RotateCameraX").Stick(RIGHTX);
	CreateAxis("RotateCameraY").Stick(RIGHTY);
	CreateAxis("CameraSpeed").Key(KEY_Z, KEY_X);
	CreateAction("CameraRightClick").Key(MOUSE_R);
}
