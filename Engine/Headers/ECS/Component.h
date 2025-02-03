#pragma once

//TODO get rid of this file, not that much use anymore
//Just have to find what is the better place for it.
namespace Perry
{

	/// <summary>
	/// Check if there are any components of type loaded
	/// </summary>
	/// <typeparam name="...ComponentType"></typeparam>
	/// <returns></returns>
	template<typename... ComponentType>
	static bool  IsComponentLoaded(entt::registry& reg)
	{
		return !reg.view<ComponentType...>().empty();
	}

}


//TODO inject defaults functions for components