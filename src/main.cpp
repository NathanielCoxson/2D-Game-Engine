#include "EntityManager.hpp"
#include "Entity.hpp"
#include <iostream>

int main() {

    EntityManager entities;
    std::shared_ptr<Entity> entity = entities.addEntity("test");

    entity->addComponent<CScore>();
    std::cout << entity->getComponent<CScore>().score << "\n";
    entity->getComponent<CScore>().score = 10;
    std::cout << "Score is now:\n";
    std::cout << entity->getComponent<CScore>().score << "\n";

    return 0;
}
