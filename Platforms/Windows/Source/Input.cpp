#include "pch.h"

#include <Xinput.h>
#pragma hdrstop

#include "Audio/AudioSystem.h"
#include "Engine.h"


using namespace Perry;

namespace InputInternal
{
	const std::map<unsigned int, uint32_t> KeyCodemap
	{
		{GAMEPAD_START, XINPUT_GAMEPAD_START},
		{GAMEPAD_BACK, XINPUT_GAMEPAD_BACK},
		{GAMEPAD_DPAD_UP, XINPUT_GAMEPAD_DPAD_UP},
		{GAMEPAD_DPAD_RIGHT,XINPUT_GAMEPAD_DPAD_RIGHT},
		{GAMEPAD_DPAD_DOWN,XINPUT_GAMEPAD_DPAD_DOWN},
		{GAMEPAD_DPAD_LEFT,XINPUT_GAMEPAD_DPAD_LEFT},
		{GAMEPAD_L3,XINPUT_GAMEPAD_LEFT_THUMB},
		{GAMEPAD_R3,XINPUT_GAMEPAD_RIGHT_THUMB},
		//GAMEPAD_L2 && GAMEPAD_R2 are implemented a other way (They are axis keys...)
		{GAMEPAD_L1,XINPUT_GAMEPAD_LEFT_SHOULDER},
		{GAMEPAD_R1,XINPUT_GAMEPAD_RIGHT_SHOULDER},
		{GAMEPAD_Y,XINPUT_GAMEPAD_Y},
		{GAMEPAD_B,XINPUT_GAMEPAD_B},
		{GAMEPAD_A,XINPUT_GAMEPAD_A},
		{GAMEPAD_X,XINPUT_GAMEPAD_X}
	};
}
using namespace Perry;

static bool ParseKeyCode(XINPUT_STATE& state, KeyCode key)
{
	if (key == KeyCode::GAMEPAD_R2)
		return state.Gamepad.bRightTrigger > 0;
	if (key == KeyCode::GAMEPAD_L2)
		return state.Gamepad.bLeftTrigger > 0;

	return (bool)(state.Gamepad.wButtons & InputInternal::KeyCodemap.at(key));
}

bool isControllerConnected = false;

bool Input::IsControllerConnected() const
{
	return isControllerConnected;
}

ControllerType Input::GetControllerType() const
{
	return ControllerType::Xbox;
}

void Axis::Update(int m_ControllerID)
{
	//TODO move this out of here...
	if (GetActiveWindow() != glfwGetWin32Window(GetEngine().GetWindow().GetHandle()) || ImGui::IsAnyItemActive())
	{
		m_Value = 0;
		return;
	}

	//keyboard input

	//controller input
	XINPUT_STATE state;
	const DWORD dwResult = XInputGetState(m_ControllerID, &state);
	isControllerConnected = dwResult != ERROR_DEVICE_NOT_CONNECTED;

	for (int i = 0; i < 4; i += 2)
	{
		bool lKey = false;
		bool rKey = false;
		if (Input::IsControllerKey(m_Keys[i]))
		{
			lKey = ParseKeyCode(state, m_Keys[i]);
			rKey = ParseKeyCode(state, m_Keys[i + 1]);
		}
		else
		{
			lKey = GetAsyncKeyState(m_Keys[i + 1]);
			rKey = GetAsyncKeyState(m_Keys[i]);
		}

		m_Value = static_cast<float>(lKey - rKey);

		if (m_Value != 0)
		{
			m_Value = m_Value * GetMagnitude();
			return;
		}
	}

	if (dwResult == ERROR_DEVICE_NOT_CONNECTED)
	{
		m_Value = 0;
		return;
	}

	assert(dwResult == ERROR_SUCCESS);

	float mult = 32767.0;


	m_Value = (&state.Gamepad.sThumbLX)[static_cast<int>(m_JoyStick) - 1];

	if(m_JoyStick >= JoyStick::L2)
		mult = 255;

	m_Value /= mult;

	if (abs(m_Value) < GetDeadZone())
	{
		m_Value = 0;
	}

	float magnitude = GetMagnitude();

	m_Value *= magnitude;

	if (m_Value < -magnitude) { m_Value = -magnitude; }
	if (m_Value > magnitude) { m_Value = magnitude; }
}


void Action::Update(int m_ControllerID)
{
	if (GetActiveWindow() != glfwGetWin32Window(GetEngine().GetWindow().GetHandle()) || ImGui::IsAnyItemActive())
	{
		UpdateState(false);
		return;
	}

	//check action keys
	for (int i = 0; i < m_KeyCount; i++)
	{
		KeyCode key = m_Keys[i];
		if (key == KeyCode::NONE)
			continue;

		if (Input::IsControllerKey(key))
		{
			//controller input
			XINPUT_STATE state;

			DWORD dwResult = XInputGetState(m_ControllerID, &state);

			if (dwResult == ERROR_DEVICE_NOT_CONNECTED)
				continue;
			assert(dwResult == ERROR_SUCCESS);

			if (ParseKeyCode(state, key))
			{
				UpdateState(true);
				return;
			}
		}
		else
		{
			if (GetAsyncKeyState(key))
			{
				UpdateState(true);
				return;
			}
		}
	}

	UpdateState(false);
}

void Input::SetControllerVibration(controllerMotor motor, float value)
{
	//controller vibration
	XINPUT_VIBRATION vib;

	vib.wLeftMotorSpeed = static_cast<WORD>(m_Vibrations[0]);
	vib.wRightMotorSpeed = static_cast<WORD>(m_Vibrations[1]);

	switch (motor)
	{
	case MOTORLOWFREQ:
		vib.wLeftMotorSpeed = static_cast<WORD>(value * 65.535f);
		break;

	case MOTORHIGHFREQ:
		vib.wRightMotorSpeed = static_cast<WORD>(value * 65.535f);
		break;

	case MOTORBOTH:
		vib.wLeftMotorSpeed = static_cast<WORD>(value * 65.535f);
		vib.wRightMotorSpeed = static_cast<WORD>(value * 65.535f);
		break;
	}

	m_Vibrations[0] = vib.wLeftMotorSpeed;
	m_Vibrations[1] = vib.wRightMotorSpeed;

	XInputSetState(m_ControllerID, &vib);//TODO make this fetch correct controllerID
}

void Input::SetController(unsigned short id)
{
	m_ControllerID = id;
}

glm::vec2 Input::GetMousePosition()
{
	if (GetActiveWindow() != glfwGetWin32Window(GetEngine().GetWindow().GetHandle()))
		return {};

	POINT p;
	GetCursorPos(&p);
	if (ScreenToClient(glfwGetWin32Window(GetEngine().GetWindow().GetHandle()), &p))
	{
		return glm::vec2(p.x, p.y);
	}

	return {};
}