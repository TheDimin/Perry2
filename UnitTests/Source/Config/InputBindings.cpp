#include "Input.h"
#include "KeyCodes.h"

using namespace Perry;

void Input::SetupBindings()
{
	CreateAxis("cameraFront").Key(KEY_S, KEY_W).Stick(LEFTY);
	CreateAxis("cameraSide").Key(KEY_A, KEY_D).Stick(LEFTX);
	CreateAxis("RotateCameraX").Stick(RIGHTX);
	CreateAxis("RotateCameraY").Stick(RIGHTY);
	CreateAction("CameraRightClick").Key(MOUSE_R);
	CreateAction("ControllerTest").Key(KEY_A).Key(GAMEPAD_R1);
	CreateAxis("Trigger").Stick(L2);
	CreateAction("RotateLeft").Key(KEY_Q);
	CreateAction("RotateRight").Key(KEY_E);
}
