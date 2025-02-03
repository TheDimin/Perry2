#pragma once

namespace Perry
{

	class TransformScope
	{
	public:
		TransformScope(entt::registry* reg);
		~TransformScope();

	private:
		entt::registry* newOne = nullptr;
		entt::registry* Fallback = nullptr;
	};

	/// <returns>(1.0f, 0.0f, 0.0f)</returns>
	constexpr static glm::vec3 Right() {
		return glm::vec3(1.0f, 0.0f, 0.0f);
	};
	/// <returns>(0.0f, 1.0f, 0.0f)</returns>
	static glm::vec3 Up() { return { 0.0f, 1.0f, 0.0f }; };
	/// <returns>(0.0f, 0.0f, -1.0f)</returns>
	constexpr static glm::vec3 Forward() {
		return glm::vec3(0.0f, 0.0f, -1.0f);
	};

	class HierarchyComponent;
	class TransformComponent;

	//Helper class for users to interact with Transforms
	class Transform
	{
	public:
		static glm::vec3 GetPosition(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);
		static glm::quat GetRotation(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);
		static glm::vec3 GetScale(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);
		static glm::mat4 GetMatrix(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);

		static glm::vec3 GetLocalPosition(const TransformComponent& transform);
		static glm::quat GetLocalRotation(const TransformComponent& transform);
		static glm::vec3 GetLocalScale(const TransformComponent& transform);
		static glm::mat4 GetLocalMatrix(const TransformComponent& transform);

		static void SetPosition(TransformComponent& transform, const glm::vec3& position, const HierarchyComponent* hierarchy = nullptr);
		static void SetRotation(TransformComponent& transform, const glm::quat& rotation, const HierarchyComponent* hierarchy = nullptr);
		static void SetRotation(TransformComponent& transform, const glm::vec3& rotation, const HierarchyComponent* hierarchy = nullptr);
		static void SetScale(TransformComponent& transform, const glm::vec3& scale, const HierarchyComponent* hierarchy = nullptr);

		static void SetLocalPosition(TransformComponent& transform, const glm::vec3& position);
		static void SetLocalRotation(TransformComponent& transform, const glm::quat& rotation);
		static void SetLocalRotation(TransformComponent& transform, const glm::vec3& rotation);
		static void SetLocalScale(TransformComponent& transform, const glm::vec3& scale);

		static void AddTranslation(TransformComponent& transform, const glm::vec3& translationToAdd);
		static void AddRotation(TransformComponent& transform, const glm::quat& rotationToAdd);
		static void AddRotation(TransformComponent& transform, const glm::vec3& rotationToAdd);
		static void AddScale(TransformComponent& transform, const glm::vec3& scaleToAdd);

		static glm::vec3 GetForwardVector(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);
		static glm::vec3 GetUpVector(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);
		static glm::vec3 GetRightVector(const TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);

		static glm::vec3 GetPosition(const glm::mat4& matrix);
		static glm::quat GetRotation(const glm::mat4& matrix);
		static glm::vec3 GetScale(const glm::mat4& matrix);

		static glm::vec3 GetLocalForwardVector(const TransformComponent& transform);
		static glm::vec3 GetLocalUpVector(const TransformComponent& transform);
		static glm::vec3 GetLocalRightVector(const TransformComponent& transform);

		static void SetParent(entt::entity parent, entt::entity child, bool worldPositionStays = true);

		static entt::entity GetParent(const HierarchyComponent* hierarchy);
		static entt::entity GetTopParent(const HierarchyComponent* hierarchy);
		static std::vector<entt::entity> GetAllParents(const HierarchyComponent* hierarchy);
		static bool HasParent(const HierarchyComponent* hierarchy);
		static bool HasChild(const HierarchyComponent* hierarchy);
		static entt::entity GetChild(const HierarchyComponent* hierarchy);
		static void DeleteChilderen(const HierarchyComponent* hierarchy);
		static std::vector<entt::entity> GetDirectChildren(entt::entity parent);

		static void SetRegistery(entt::registry* reg);

		template<typename T = void>
		static std::vector<entt::entity> GetChildren(entt::entity parent)
		{
			std::vector<entt::entity> children = {};
			GetChildrenRecursiveInternal<T>(*Registry, Transform::GetChild(&GetHierarchy(parent)), children);
			return children;
		}

		static void RecalculateMatrix(const TransformComponent& transform, const HierarchyComponent* hiearchy);
		static bool IsDirty(const TransformComponent& transform);

		/// <summary>
		/// The current registery we read/write with. unique per thread, requires user to call SetRegistry before using anything of this class...
		/// </summary>
		static inline thread_local entt::registry* Registry = nullptr;
	private:
		friend class TransformScope;
		static entt::registry& GetRegistry();
		static void SetDirty(TransformComponent& transform, const HierarchyComponent* hierarchy = nullptr);

		static std::vector<entt::entity> HiearchyRelation(HierarchyComponent&);
		static bool HasHiearchy(entt::entity e);
		static HierarchyComponent& GetHierarchy(entt::entity e);

		template<typename T>
		static void GetChildrenRecursiveInternal(entt::entity parent, std::vector<entt::entity>& result)
		{
			if constexpr (!std::is_same_v<T, void>) {
				if (Registry->try_get<T>(parent))
					result.emplace_back(parent);
			}
			else
			{
				result.emplace_back(parent);
			}

			if (HasHiearchy(parent))
			{
				auto& Hierarchy = GetHierarchy(parent);

				if (Transform::HasChild(&Hierarchy))
					Transform::GetChildrenRecursiveInternal<T>(Transform::GetChild(&Hierarchy), result);

				for (auto child : HiearchyRelation(Hierarchy))
				{
					GetChildrenRecursiveInternal<T>(child, result);
				}
			}
		}


		static void RecalculateLocalMatrix(const TransformComponent& transform, const HierarchyComponent* hiearchy);


		static void MarkChildrenDirty(entt::entity e);
	};
}