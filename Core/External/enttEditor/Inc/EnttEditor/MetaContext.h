#pragma once
#include <entt/entt.hpp>

namespace EnttEditor
{
class MetaContext
{
public:
    static decltype(auto) GetMetaContext()
    {
        [[maybe_unused]] auto l = entt::locator<entt::meta_ctx>::value_or();

        static auto nodeType = entt::locator<entt::meta_ctx>::handle();

        return nodeType;
    }
};
}  // namespace EnttEditor