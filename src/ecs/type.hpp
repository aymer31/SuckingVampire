#pragma once

#include <bitset>
#include <set>

namespace ecs::details
{
class SystemManager;
}

namespace ecs
{

using Entity = uint32_t;
constexpr Entity MAX_ENTITIES = 100'000u;

using Component = uint8_t;
constexpr Component MAX_COMPONENTS = 32u;

using Signature = std::bitset<MAX_COMPONENTS>;

class System
{
    friend class details::SystemManager;

public:
    virtual ~System() = default;

protected:
    std::set<Entity>& entities()
    {
        return m_entities;
    }

private:
    std::set<Entity> m_entities{};
};

} // namespace ecs
