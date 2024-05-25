#pragma once

#include <vector>
#include <map>
#include <memory>
#include "Entity.hpp"

class EntityManager {

    friend class Entity;

    std::map<std::string, std::vector<std::shared_ptr<Entity>>> m_entityMap = {};
    std::vector<std::shared_ptr<Entity>>                        m_entities = {};
    std::vector<std::shared_ptr<Entity>>                        empty = {};
    std::vector<std::shared_ptr<Entity>>                        m_entitiesToAdd;
    int                                                         next_id = 0;

    void init();
    void removeDeadEntities(std::vector<std::shared_ptr<Entity>>& vec);

public:
    EntityManager();
    void update(); // deletes dead entities and adds new ones
    const std::shared_ptr<Entity> addEntity(const std::string& tag);
    const std::vector<std::shared_ptr<Entity>>& getEntities() const;
    const std::vector<std::shared_ptr<Entity>>& getEntities(const std::string& tag) const;
    
};
