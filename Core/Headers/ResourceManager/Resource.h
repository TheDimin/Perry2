#pragma once

namespace Perry
{

	/// <summary>
	/// Wrapper around resources
	///	When dealing with ResourceManager this is the type it will return.
	///	Can contain null or unloaded resources.
	/// </summary>
	/// <typeparam name="T">Type of the Resource we contain</typeparam>
	template<typename T>
	class Resource
	{
	public:
		Resource() = default;
		~Resource();
		Resource(const Resource& baseResource);
		Resource& operator=(const Resource& baseResource);
		Resource(Resource&& other) noexcept;
		Resource& operator=(Resource&& other) noexcept;
		T* operator->() const;
		bool operator!() const;
		explicit operator bool() const {
			return this->IsLoaded();
		}
		bool operator==(const Resource& rhs) const {
			if (m_Instance == nullptr)
				return false;

			return m_Instance == rhs.m_Instance;
		}
		bool operatorResource() const { return IsLoaded(); }

		/// <summary>
		/// Checks if the underlying Resource is loaded
		/// </summary>
		/// <returns>true if its loaded</returns>
		bool IsLoaded() const;
		bool IsLoading() const;
		/// <summary>
		/// Get the object that the underlying Resource is storing
		/// </summary>
		/// <returns>Returns a raw pointer to T</returns>
		T* Get() const;

		typedef  std::function<void(T*)> loadCallback;

		/// <summary>
		/// Function that will be called when the underlying resource is loaded.
		/// </summary>
		/// <param name="load"></param>
		void OnLoad(const loadCallback& load) const;

		explicit Resource(T* instance) : m_Instance(instance) {}
	private:
		template<typename TT>
		friend class ResourceManager;

		IResourceType* m_Instance = nullptr;
	};

	template<typename T>
	Resource<T>::~Resource()
	{
		m_Instance = nullptr;
	}

	template<typename T>
	T* Resource<T>::operator->() const
	{
		return Get();
	}

	template<typename T>
	bool Resource<T>::operator!() const
	{
		return m_Instance == nullptr;
	}

	template<typename T>
	Resource<T>::Resource(const Resource& baseResource)
	{
		m_Instance = baseResource.m_Instance;
	}

	template<typename T>
	Resource<T>& Resource<T>::operator=(const Resource& baseResource)
	{
		m_Instance = baseResource.m_Instance;
		return *this;
	}

	template<typename T>
	Resource<T>::Resource(Resource&& other) noexcept : m_Instance(other.m_Instance)
	{
	}

	template<typename T>
	Resource<T>& Resource<T>::operator=(Resource&& other) noexcept
	{
		m_Instance = other.m_Instance;
		return *this;
	}

	template<typename T>
	bool Resource<T>::IsLoaded() const
	{
		if (m_Instance == nullptr)
			return false;

		return m_Instance->IsLoaded();
	}
	template<typename T>
	bool Resource<T>::IsLoading() const
	{
		if (m_Instance == nullptr)
			return false;

		return m_Instance->IsLoading();
	}

	template<typename T>
	void Resource<T>::OnLoad(const loadCallback& load) const
	{
		ASSERT_MSG(LOG_RESOURCE, m_Instance, "Instance is nullptr");

		m_Instance->OnLoad([load] (IResourceType* i ){load(static_cast<T*>(i));});
	}

	template<typename T>
	T* Resource<T>::Get() const
	{
		ASSERT_MSG(LOG_RESOURCE, m_Instance, "Instance is nullptr");

		if (!m_Instance->IsLoaded())
			//If the callstack comes for calling `resource.get()->OnLoad()` use `resource.OnLoad()`
			WARN(LOG_RESOURCE,
				"Attempted to Get resource '%s', but this is not loaded yet",
				m_Instance->GetPath().c_str());

		return static_cast<T*>(m_Instance);
	}

} // namespace Ball