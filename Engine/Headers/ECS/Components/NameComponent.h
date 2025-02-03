#pragma once

namespace Perry
{

	class NameComponent
	{
	public:
		NameComponent() = default;
#ifndef SHIPPING
		NameComponent(const std::string& newName) : name(newName)
		{
		};

		std::string name = "Entity";

		REFLECT()
#else
		NameComponent(const std::string& newName) {}
#endif
	};
}
