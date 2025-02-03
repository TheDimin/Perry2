#pragma once
namespace Perry
{
	class FlyMovementSystem : public Perry::System
	{
	public:
		void Init(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

		bool m_InvertMouse = true;
		float m_MouseSensitivity = 1.f;

	protected:
		GraphInfo GetGraphBinding() override;
	public:
		REFLECT()
	};

	class FlyingMovementComponent
	{
	public:
		FlyingMovementComponent() = default;
		FlyingMovementComponent(float speed, float rotationSpeed) :m_MovementSpeed(speed), m_RotationSpeed(rotationSpeed) {}
		mutable float m_Pitch = 0;
		mutable float m_Yaw = 0;
		float m_MovementSpeed = 2;
		float m_RotationSpeed = 1;

		REFLECT()
	};
}