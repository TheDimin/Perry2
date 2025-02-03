#pragma once

namespace Perry
{
	class Primitive;
	struct InstanceData;

	struct EntityGenerator
	{
		EntityGenerator();
		entt::entity e;
		const Primitive* m_Prim = nullptr;
		Resource<Material> m_Material{};
		entt::registry* reg;
	};

	struct EntityProxy
	{
		entt::entity e;
		Resource<Material> m_Material;
		const Primitive* m_Prim = nullptr;
	};


	struct PrimitiveBatch
	{
		PrimitiveBatch(const Primitive* prim);

		const Primitive* m_Prim = nullptr;
		int batchCount = 0;
		std::vector<entt::entity> m_Entities{};

		ArrayBuffer<InstanceData> m_InstanceData;

		void Insert(const EntityGenerator& e);
		bool Remove(const EntityGenerator& e);
	};


	struct MaterialBatch
	{
		MaterialBatch(const Resource<Material> mat) :m_Material(mat) {}
		Resource<Material> m_Material{};

		std::vector<PrimitiveBatch> m_primitivebatch{};

		void Insert(const EntityGenerator& e);
		bool Remove(const EntityGenerator& e);
		PrimitiveBatch* Find(Primitive* prim);
	};
	class ProxyContainers
	{
	public:
		std::vector<MaterialBatch> m_SimpleBatch{};
		//Cant build fancy trees, no instancing nothing. 
		std::vector<EntityProxy> m_BlendBatch{};
		void Insert(const EntityGenerator& e);
		bool Remove(const EntityGenerator& e);

		MaterialBatch* Find(const Resource<Material>& material);
	};

	class RenderBatcherSystem : public Perry::System
	{
	public:
		RenderBatcherSystem() { ExecutionPriority = 10000; }

		RenderBatcherSystem(const RenderBatcherSystem& other) : Perry::System(other), batcher(other.batcher) {}
		RenderBatcherSystem(RenderBatcherSystem&& other) noexcept :
			Perry::System(std::move(other)), batcher(std::move(other.batcher))
		{
		}
		RenderBatcherSystem& operator=(const RenderBatcherSystem& other)
		{
			if (this == &other)
				return *this;
			Perry::System::operator=(other);
			batcher = other.batcher;
			return *this;
		}
		RenderBatcherSystem& operator=(RenderBatcherSystem&& other) noexcept
		{
			if (this == &other)
				return *this;
			Perry::System::operator=(std::move(other));
			batcher = std::move(other.batcher);
			return *this;
		}

		void Init(entt::registry& reg) override;
		void ShutDown(entt::registry& reg) override;
		void Update(entt::registry& reg) override;

		void OnMaterialBlendChange(Material* mat);
		void OnMaterialChange(entt::registry& reg, entt::entity, Resource<Material> oldMaterial, unsigned index);
		void OnMeshComponentCreated(entt::registry& reg, entt::entity e);
		void OnMeshComponentDestroyed(entt::registry& reg, entt::entity e);
		void OnMeshComponentUpdate(entt::registry& reg, entt::entity e);

	protected:
		GraphInfo GetGraphBinding() override;
	public:
		ProxyContainers batcher{};
		entt::observer m_Observer;

		std::mutex GlobalReadMutex;

		// TODO move lights here..

		Concurrency::concurrent_queue<EntityGenerator> createQueue;
		Concurrency::concurrent_queue<EntityGenerator> deleteQueue;
		Concurrency::concurrent_queue<EntityGenerator> updateQueue;

		REFLECT()
	};
}
