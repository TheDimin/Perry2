#pragma once

namespace Perry
{
	/// <summary>
	/// Giving this to a Transform means that it cannot be moved.
	/// Systems such as rendering and phsyics can use the knowledge that this object won't move to optmize storage.
	/// Note that Parent and child objects should also be marked static
	/// For now this is not enforced but be aware !
	/// </summary>
	class StaticTransform
	{
	public:
		StaticTransform() = default;
		REFLECT()
	};

	class TransformComponent
	{
	public:
		TransformComponent() = default;
		TransformComponent(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scl);
		TransformComponent(const glm::mat4& data);


		friend class Transform;
		glm::vec3 m_Position{ 0.0f, 0.0f, 0.f };
		glm::quat m_Rotation = glm::identity<glm::quat>();
		glm::vec3 m_Scale{ 1.0f, 1.0f, 1.0f };

		mutable glm::mat4 m_Local = glm::identity<glm::mat4>();
		mutable glm::mat4 m_Global = glm::identity<glm::mat4>();
		mutable bool m_Dirty = true;

		REFLECT()
	};
}
