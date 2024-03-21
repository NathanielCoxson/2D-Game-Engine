#include "EntityManager.hpp"
#include "Entity.hpp"

EntityManager::EntityManager() {}

void EntityManager::update() {

}

const std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    // Create entity
    Entity newEntity(next_id, tag);

    // Create shared pointer to new entity.
    std::shared_ptr<Entity> ptr = std::make_shared<Entity>(newEntity);

    // Add pointer to entities vector
    m_entities.push_back(ptr);

    // Add pointer to entity map
    m_entityMap[tag].push_back(ptr);

    next_id++;
    return ptr;
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities() const {
    return m_entities;
}

const std::vector<std::shared_ptr<Entity>>& EntityManager::getEntities(const std::string& tag) const {
    // Return empty vector if key is not found
    if (m_entityMap.count(tag) == 0) {
        return empty;
    }
    return m_entityMap.at(tag);
}
