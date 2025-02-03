#pragma once

namespace Perry
{
	struct Entity;
}
class ContextMenu
{
public:
	static void GenerateComponentContext(const Perry::Entity& owner, entt::meta_any& component);

	static void GenerateEntityContext(const Perry::Entity& e);

private:

	static inline std::string relPath = "";
};