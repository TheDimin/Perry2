#pragma once

namespace Perry
{
	/// <summary>
	/// Base class of any resource, defines the default functions and sets up copy/move constructors.
	/// </summary>
	class IResourceType
	{
	public:

		typedef  std::function<void(IResourceType*)> loadCallback;
		virtual ~IResourceType() = default;
		// Cannot copy or move the actual resource
		IResourceType(const IResourceType& baseResource) = delete;
		IResourceType& operator=(const IResourceType& baseResource) = delete;
		IResourceType(IResourceType&& other) = delete;
		IResourceType& operator=(IResourceType&& other) = delete;

		/// <summary>
		/// Returns true if the Resource itself is loaded, if its not its unsafe to use any of the resource data.
		/// </summary>
		/// <returns>true if its loaded</returns>
		virtual bool IsLoaded() const;
		virtual bool IsLoading() const;
		/// <summary>
		/// Returns the resource path on disk (Aka the path used to loaded the asset)
		/// </summary>
		inline const std::string& GetPath() const;

		/// <summary>
		/// Called when this resource has been loaded.
		/// </summary>
		/// <param name=""></param>
		void OnLoad(const loadCallback&);

	protected:
		template<typename T>
		friend class ResourceManager;
		template<typename T>
		friend class Resource;
		IResourceType() = default;
		IResourceType(const std::string& path) : m_ResourcePath(path) {}

		/// <summary>
		/// Called when a asset is being loaded by the resource manager
		/// </summary>
		virtual void Load() = 0;
		/// <summary>
		/// Called when the resource manager unloads this resource.
		///	Its possible this gets called but the deconstructor does not.
		///	(As that only gets called before reloading the asset)
		/// </summary>
		virtual void Unload() = 0;


		/// <summary>
		/// Function user should call when its done loading! this because loading can spawn more async tasks
		/// </summary>
		virtual void CompleteLoad();


		std::string m_ResourcePath;
		bool m_Loaded = false;
		bool m_IsLoading = false;


		Concurrency::concurrent_queue<loadCallback> m_OnLoadCallbacks;
	};

	inline bool IResourceType::IsLoading() const
	{
		return m_IsLoading;
	}

	inline bool IResourceType::IsLoaded() const
	{
		return m_Loaded;
	}

	inline const std::string& IResourceType::GetPath() const
	{
		return m_ResourcePath;
	}

	inline void IResourceType::OnLoad(const loadCallback& callback)
	{
		if (IsLoaded())
		{
			callback(this);
			return;
		}

		m_OnLoadCallbacks.push(callback);
	}
	inline void IResourceType::CompleteLoad()
	{
		m_Loaded = true;
		m_IsLoading = false;

		loadCallback callback ; 

		while(m_OnLoadCallbacks.try_pop(callback))
		{
			callback(this);
		}

		WARN(LOG_RESOURCE,"completed loading resource: %s",GetPath().data());
	}
}; // namespace Ball