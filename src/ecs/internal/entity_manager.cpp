#include "ecs/internal/entity_manager.hpp"

#include <cassert>

namespace ecs::details
{

EntityManager::EntityManager()
{
	for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
	{
		m_availableEntities.push(entity);
	}
}

Entity EntityManager::create_entity()
{
	assert(m_livingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

	Entity id = m_availableEntities.front();
	m_availableEntities.pop();
	++m_livingEntityCount;

	return id;
}

void EntityManager::destroy_entity(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	m_signatures[entity].reset();
	m_availableEntities.push(entity);
	--m_livingEntityCount;
}

void EntityManager::set_entity_signature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	m_signatures[entity] = signature;
}

Signature EntityManager::get_entity_signature(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	return m_signatures[entity];
}

[[nodiscard]] EntityManager& entities()
{
	static EntityManager s_singleton;
	return s_singleton;
}

} // namespace ecs::details