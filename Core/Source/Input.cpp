#include "Core.h"
#pragma hdrstop

using namespace Perry;

//IMPLEMENT_REFLECT_OBJECT(Axis)
//{
//}
//FINISH_REFLECT_OBJECT();


IMPLEMENT_REFLECT_OBJECT(Input)
{
	meta
		PROP_SCRIPTCALLABLE;

	meta.func<&Input::GetAxis>("GetAxis"_hs)
		PROP_SCRIPTCALLABLE
		PROP_DISPLAYNAME("GetAxis");

	meta.func<&Input::GetAction>("GetAction"_hs)
		PROP_SCRIPTCALLABLE
		PROP_DISPLAYNAME("GetAction");

}
FINISH_REFLECT();


void Input::Init(unsigned short playerIndex)
{
	SetupBindings();
	SetController(playerIndex);
}

void Input::Update()
{
	for (auto&& [name, action] : m_Actions)
	{
		action.Update(m_ControllerID);
	}

	for (auto&& [name, axis] : m_Axes)
	{
		axis.Update(m_ControllerID);
	}
}

Action::Action(const std::string& n)
{
	m_Name = n;
	for (size_t i = 0; i < m_KeyCount; i++)
	{
		m_Keys[i] = KeyCode::NONE;
	}
}

Axis::Axis(const std::string& name) : m_Name(name)
{
}

Action& Action::Key(KeyCode key)
{
	//TODO instead we should have preproccesor to check if keyboard keys are supported
#ifndef PLATFORM_WINDOWS
	if (!Input::IsControllerKey(key))
		return *this;
#endif


	m_Keys[m_BoundKeyCount] = key;
	m_BoundKeyCount++;
	if (m_BoundKeyCount > m_KeyCount - 1) {
		//TODO(Damian) Yeah this is weird...
		m_BoundKeyCount = 0;
	}
	return *this;
}

void Action::UpdateState(bool KeyDown)
{
	if (KeyDown)
		switch (m_PressState)
		{
		case Pressed:
		case Down: m_PressState = Down; break;

		case Released:
		case NONE:  m_PressState = Pressed; break;
		}
	else
		switch (m_PressState)
		{
		case Pressed:
		case Down: m_PressState = Released; break;

		case Released:
		case NONE:  m_PressState = NONE; break;
		}
}

float Axis::GetMagnitude() const
{
	return m_Magnitude;
}

float Axis::GetDeadZone() const
{
	return m_Deadzone;
}

Axis& Axis::Key(KeyCode minKey, KeyCode maxKey)
{
	//TODO instead we should have preproccesor to check if keyboard keys are supported
#ifndef PLATFORM_WINDOWS
	if (!Input::IsControllerKey(minKey))
		return *this;
#endif

	assert(m_Keys[2] == KeyCode::NONE);//Out of key slots

	int i = m_Keys[0] == KeyCode::NONE ? 0 : 2;

	m_Keys[i] = minKey;
	m_Keys[i + 1] = maxKey;

	return *this;
}

Axis& Axis::Stick(JoyStick stick)
{
	assert(m_JoyStick == JOYSTICK_NONE);
	m_JoyStick = stick;
	return *this;
}

Axis& Axis::Magnitude(float mag)
{
	m_Magnitude = mag;
	return *this;
}

Axis& Axis::Deadzone(float val)
{
	m_Deadzone = val;
	return *this;
}

Action& Input::CreateAction(const std::string& name)
{
	auto inserted = m_Actions.emplace(name, Action(name));

	return inserted.first->second;
}

bool Input::IsControllerKey(KeyCode key)
{
	return key >= 200;
}

bool Input::GetAction(const std::string& name)
{
	assert(m_Actions.find(name) != m_Actions.end());
	return m_Actions.find(name)->second.m_PressState == Action::Down;
}

bool Input::GetActionDown(const std::string& name)
{
	assert(m_Actions.find(name) != m_Actions.end());
	return m_Actions.find(name)->second.m_PressState == Action::Pressed;
}

bool Input::GetActionReleased(const std::string& name)
{
	assert(m_Actions.find(name) != m_Actions.end());
	return m_Actions.find(name)->second.m_PressState == Action::Released;
}

float Input::GetAxis(const std::string& name)
{
	assert(m_Axes.find(name) != m_Axes.end());
	return m_Axes.find(name)->second.m_Value;
}

Axis& Input::CreateAxis(const std::string& name)
{
	//Axis already exists
	assert(m_Axes.find(name) == m_Axes.end());

	auto inserted = m_Axes.emplace(name, Axis(name));
	return inserted.first->second;
}

bool Perry::Input::ContainsAxis(const std::string& name)
{
	return m_Axes.find(name) != m_Axes.end();
}

bool Input::ContainsAction(const std::string& name)
{
	return m_Actions.find(name) != m_Actions.end();
}