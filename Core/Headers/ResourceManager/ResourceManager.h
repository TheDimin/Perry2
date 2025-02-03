#pragma once

namespace Perry
{
	struct FiberContext;
	class Fiber;
	template<typename T>
	class ResourceManager
	{
	public:
		static std::vector<std::string> GetAllPaths();
		static std::vector<Resource<T>> GetAll();
		/// <summary>
		/// Returns the Resource, even if its not loaded (But it has to exist !)
		/// </summary>
		/// <param name="filePath">Path to the resource</param>
		/// <returns>A valid resource if its exists, but nullptr Resource if it doesn't</returns>
		static Resource<T> Get(const std::string& filePath);
		/// <summary>
		/// Loads the current resource, use this to call constructor ect
		/// </summary>
		/// <param name="path">Path to resource to be loaded</param>
		/// <param name="...args">Additional constructor parameters for T</param>
		/// <returns>The resource that has been loaded</returns>
		template<typename... Args>
		static Resource<T> Load(const std::string& path, Args&&... args);

		/// <summary>
		/// Loads the resource async
		/// </summary>
		/// <param name="path">Path to resource to be loaded</param>
		/// <param name="...args">Additional constructor parameters for T</param>
		/// <returns>The resource that has been loaded</returns>
		template<typename... Args>
		static Resource<T> LoadAsync(const std::string& path, Args&&... args);

		/// <summary>
		/// Unloads the given resource
		/// </summary>
		/// <param name="path">resource to unload</param>
		static void Unload(const std::string& path);
		/// <summary>
		/// Checks if an item is loaded (an item can exists in the cache but still be unloaded)
		/// </summary>
		/// <param name="path">The path to item you want to check</param>
		/// <returns>True if item is loaded</returns>
		static bool IsLoaded(const std::string& path);
		/// <summary>
		/// Remove all items from this resource manager
		///	Compared to unload, this invalidates all pointers !
		/// </summary>
		static void Clear();
		/// <summary>
		/// Get the amount of Items stored in this resourceManager
		/// </summary>
		/// <returns> number of items</returns>
		static int Size();

	private:
		static inline std::unordered_map<std::string, std::unique_ptr<T>> m_Cache{};
	};

	template<typename T>
	std::vector<std::string> ResourceManager<T>::GetAllPaths()
	{
		std::vector<std::string> paths;
		paths.reserve(m_Cache.size());

		for (const auto& cachePair : m_Cache)
			paths.emplace_back(cachePair.first);

		return paths;
	}

	template <typename T>
	std::vector<Resource<T>> ResourceManager<T>::GetAll()
	{
		std::vector<Resource<T>> paths;
		paths.reserve(m_Cache.size());

		for (const auto& cachePair : m_Cache)
			paths.emplace_back(cachePair.second.get());

		return paths;
	}

	template<typename T>
	Resource<T> ResourceManager<T>::Get(const std::string& filePath)
	{
		// Check if the resource path has already been prefixed
		std::string tempPath;
		if (filePath.find(FileIO::GetPath(DirectoryType::Resource)) == std::string::npos)
			tempPath = FileIO::GetPath(DirectoryType::Resource) + filePath;
		else
			tempPath = filePath;

		const auto& cachePair = m_Cache.find(tempPath);
		if (cachePair != m_Cache.end())
		{
			return Resource(cachePair->second.get());
		}

		INFO(LOG_RESOURCE, "Attempted to get '%s' but Resource is not loaded yet", tempPath.c_str());

		m_Cache.emplace(tempPath, std::move(std::make_unique<T>(tempPath)));

		return Resource(m_Cache.find(tempPath)->second.get());
	}

	template<typename T>
	template<typename... Args>
	Resource<T> ResourceManager<T>::Load(const std::string& path, Args&&... args)
	{
		// Check if the resource path has already been prefixed
		std::string tempPath;
		if (path.find(FileIO::GetPath(DirectoryType::Resource)) == std::string::npos)
			tempPath = FileIO::GetPath(DirectoryType::Resource) + path;
		else
			tempPath = path;

		const auto& cachePair = m_Cache.find(tempPath);

		IResourceType* resource = nullptr;
		if (cachePair == m_Cache.end())
		{
			// Its not found create it
			auto asset = std::make_unique<T>(tempPath, std::forward<Args>(args)...);
			resource = static_cast<IResourceType*>(asset.get());

			m_Cache.emplace(tempPath, std::move(asset));
		}
		else
		{
			resource = static_cast<IResourceType*>(cachePair->second.get());
			if (resource->IsLoaded())
			{
				INFO(LOG_RESOURCE, "Attempted to load an already loaded asset '%s' this is not allowed and is ignored !", tempPath.c_str());

				return Resource((T*)resource);
			}

			auto callbacks = resource->m_OnLoadCallbacks;

			// We have a smart pointer and want to reset it without it changing the underlying T* address.
			// I achieve this by using the placement new operator.

			resource->~IResourceType();
			new (resource) T(tempPath, std::forward<Args>(args)...);

			IResourceType::loadCallback callback;
			while (callbacks.try_pop(callback))
			{
				resource->m_OnLoadCallbacks.push(callback);
			}
		}

		if (!resource->m_Loaded)
			resource->Load();

		return Resource<T>(static_cast<T*>(resource));
	}

	template <typename T>
	template <typename ... Args>
	Resource<T> ResourceManager<T>::LoadAsync(const std::string& path, Args&&... args)
	{//TODO
		//GLOBAL_SCOPE_LOCK()
			//return Load(path, args...);


		// Check if the resource path has already been prefixed

		std::string tempPath;
		if (path.find(FileIO::GetPath(DirectoryType::Resource)) == std::string::npos)
			tempPath = FileIO::GetPath(DirectoryType::Resource) + path;
		else
			tempPath = path;

		const auto& cachePair = m_Cache.find(tempPath);


		IResourceType* resource = nullptr;
		if (cachePair == m_Cache.end())
		{
			// Its not found create it
			auto asset = std::make_unique<T>(tempPath, std::forward<Args>(args)...);
			resource = static_cast<IResourceType*>(asset.get());

			m_Cache.emplace(tempPath, std::move(asset));
		}
		else
		{
			resource = static_cast<IResourceType*>(cachePair->second.get());
			/*if (resource->IsLoaded())
			{
				INFO(LOG_RESOURCE, "Attempted to load an already loaded asset '%s' this is not allowed and is ignored !", tempPath.c_str());

				return Resource((T*)resource);
			}

			if (resource->IsLoading())
			{

				WARN(LOG_RESOURCE, "Attempted to load resource async while its already loading, this is not allowed");
				return Resource((T*)resource);
			}
			auto callbacks = resource->m_OnLoadCallbacks;

			// We have a smart pointer and want to reset it without it changing the underlying T* address.
			// I achieve this by using the placement new operator.


			T* newT = new T(tempPath, std::forward<Args>(args)...);
			newT->m_OnLoadCallbacks = callbacks;

			resource->~IResourceType();
			resource = newT;*/

		}
		if (!resource->IsLoaded() && !resource->m_IsLoading)
		{
			resource->m_IsLoading = true;
			Fiber* fib = new Perry::Fiber([&](FiberContext&)
				{
					resource->Load();
					if (!resource->IsLoaded())
						INFO(LOG_RESOURCE, "Resource is not done loading yet. did you forget to call CompleteLoad?");

				}, nullptr);

			fib->Execute();
		}

		return Resource<T>(static_cast<T*>(resource));
	}

	template<typename T>
	void ResourceManager<T>::Unload(const std::string& path)
	{
		throw;
		// Check if the resource path has already been prefixed
		std::string tempPath;
		if (path.find(FileIO::GetPath(DirectoryType::Resource)) == std::string::npos)
			tempPath = FileIO::GetPath(DirectoryType::Resource) + path;
		else
			tempPath = path;

		const auto& cachePair = m_Cache.find(tempPath);

		if (cachePair != m_Cache.end())
		{
			IResourceType* resource = static_cast<IResourceType*>(cachePair->second.get());
			resource->Unload();
			resource->m_Loaded = false;
			return;
		}

		ERROR(LOG_RESOURCE, "Attempted to unload '%s' which is not loaded", tempPath.c_str());
	}

	template<typename T>
	bool ResourceManager<T>::IsLoaded(const std::string& path)
	{
		// Check if the resource path has already been prefixed
		std::string tempPath;
		if (path.find(FileIO::GetPath(DirectoryType::Resource)) == std::string::npos)
			tempPath = FileIO::GetPath(DirectoryType::Resource) + path;
		else
			tempPath = path;

		auto cachePair = m_Cache.find(tempPath);
		if (cachePair == m_Cache.end())
			return false;

		return cachePair->second->m_Loaded;
	}

	template<typename T>
	void ResourceManager<T>::Clear()
	{
		for (auto& it : m_Cache)
		{
			ResourceManager<T>::Get(it.first)->Unload();
		}
		m_Cache.clear();
	}

	template<typename T>
	int ResourceManager<T>::Size()
	{
		return m_Cache.size();
	}
} // namespace Ball