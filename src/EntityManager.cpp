#include "EntityManager.hpp"
#include "Entity.hpp"

EntityManager::EntityManager() {}

void EntityManager::update() {
    // Updates to entities vector and map are only processed
    // when this function is called to prevent any invalidation
    // from occuring by manipulating these structures during
    // a main game loop iteration.
    // I.e. the data used during the loop shouldn't change
    // during that loop since each frame is a discrete unit.
    // E.g. you wouldn't want to update an entity's position
    // before rendering the previous change from the last loop.

    // Add entities from m_entitiesToAdd to entites vector and map
    for (auto e: m_entitiesToAdd) {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_entitiesToAdd.clear();
    
    // Remove entities marked as destroyed from m_entities
    removeDeadEntities(m_entities);

    // Remove entities marked as destroyed from m_entityMap
    for (auto& [tag, entityVec]: m_entityMap) {
        removeDeadEntities(entityVec); 
    }
}

void EntityManager::removeDeadEntities(std::vector<std::shared_ptr<Entity>>& vec) {
    for (auto it = vec.begin(); it != vec.end(); ) {
        if (!(*it)->isActive()) {
            it = vec.erase(it); 
        } else {
            it++;
        }
    }
}

const std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
    // Create entity
    Entity newEntity(next_id, tag);

    // Create shared pointer to new entity.
    std::shared_ptr<Entity> ptr = std::make_shared<Entity>(newEntity);

    // Add pointer to entities vector
    m_entitiesToAdd.push_back(ptr);

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
