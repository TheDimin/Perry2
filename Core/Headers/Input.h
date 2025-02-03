#pragma once

namespace Perry {
	class EngineManager;
	enum controllerMotor : unsigned char;
	enum JoyStick : unsigned char;
	enum KeyCode : unsigned int;

	class Axis {
	public:
		IMPLEMENT_MEM_DEBUG(Axis);
		Axis(const std::string& name);
		//We need to support this constructor for meta
		Axis(const Axis&) { \
			printf("[WARNING] Axis copy constrcutor called \n"); };
		Axis(Axis&&) = default;

		Axis& Key(KeyCode minKey, KeyCode maxKey);
		Axis& Stick(JoyStick key);
		Axis& Magnitude(float mag);
		Axis& Deadzone(float val);

		float GetMagnitude() const;
		float GetDeadZone() const;

	private:
		friend class Input;
		//This should always match the owning controller (Stored in Input)
		void Update(int m_ControllerID);

		float m_Deadzone = 0.125f;
		float m_Magnitude = 1.0f;
		KeyCode m_Keys[4]{};
		JoyStick m_JoyStick{};
		std::string m_Name;  // Do we save name here or only in the Input maps ?

		//The value of this Axis in this frame (Magnitude and clamping is applied)
		float m_Value = 0;
	};

	class Action {
	public:
		IMPLEMENT_MEM_DEBUG(Action);
		Action() =default;
		Action(const std::string& name);
		//We need to support this constructor for meta
		Action(const Action&) { printf("[WARNING] Action copy constrcutor called \n"); };
		Action(Action&&) = default;

		Action& Key(KeyCode key);

	private:
		friend class Input;
		enum PressState
		{
			Pressed,//The key got pressed this frame
			NONE,//Key is not being pressed/held down
			//Note this order allows for >= DOwn or <= Down
			Down,//Key is being held down
			Released//Key has been released this frame
		};
		void Update(int m_ControllerID);
		void UpdateState(bool KeyDown);
		static const int m_KeyCount = 3;
		KeyCode m_Keys[m_KeyCount]{};
		//How many keys are bound to this action
		int m_BoundKeyCount = 0;
		std::string m_Name;  // Do we save name here or only in the Input maps ?

		PressState m_PressState = NONE;
	};

	enum ControllerType {
		Playstation,
		Xbox,
		Switch,
	};

	//
	class Input {
	public:
		Input(const Input&) = delete;

		Axis& CreateAxis(const std::string& name);
		bool ContainsAxis(const std::string& name);

		Action& CreateAction(const std::string& name);
		bool ContainsAction(const std::string& name);

		static bool IsControllerKey(KeyCode key);
		//CHeck If this Action is being held down
		bool GetAction(const std::string& name);
		//Check if the Action got pressed this frame
		bool GetActionDown(const std::string& name);
		//Check if the action got Released this frame
		bool GetActionReleased(const std::string& name);

		float GetAxis(const std::string& name);

		bool IsControllerConnected() const;
		ControllerType GetControllerType() const;

		glm::vec2 GetMousePosition();

		// value = 0,1
		void SetControllerVibration(controllerMotor side, float value);

	private:
		friend EngineManager;
		Input() = default;
		void Init(unsigned short playerIndex = 0);
		void Update();
		void SetupBindings();
		void SetController(unsigned short id);

		std::map<std::string, Action> m_Actions{};
		std::map<std::string, Axis> m_Axes;
		float m_Vibrations[2];
		int m_ControllerID = 0;


		//Reflect needs default constructor
		//friend class EngineManager;
		//Input() = default;
		REFLECT()
	};
}  // namespace Perry