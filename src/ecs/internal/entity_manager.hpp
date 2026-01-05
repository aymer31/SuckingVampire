#pragma once

#include "ecs/export.h"
#include "ecs/type.hpp"

#include <array>
#include <queue>
#include <cstddef>

namespace ecs::details
{
class EntityManager;
}

namespace ecs::details
{

/**
 * @brief Manages creation and destruction of entities.
 *
 * Entities are represented by integer IDs in the range [0, MAX_ENTITIES].
 * IDs are reused after destruction.
 *
 * @warning This class does not prevent double-destroy or use-after-destroy.
 *
 * Usage:
 * @code
 * ecs::Entity e = entities().create_entity();
 *
 * ecs::Signature s;
 * s.set(0); // component type 0 present
 * entities().set_entity_signature(e, s);
 *
 * entities().destroy_entity(e);
 * @endcode
 */

[[nodiscard]] ECS_EXPORT EntityManager& entities();

class ECS_EXPORT EntityManager
{
public:
	EntityManager();

	[[nodiscard]] Entity create_entity();
	void destroy_entity(Entity entity);

	void set_entity_signature(Entity entity, Signature signature);
	[[nodiscard]] Signature get_entity_signature(Entity entity);

private:
	std::queue<Entity> m_availableEntities{};
    std::array<Signature, MAX_ENTITIES> m_signatures{};
	size_t m_livingEntityCount{ 0u };
};

} // namespace ecs::details
