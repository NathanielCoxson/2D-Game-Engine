#include "doctest.hpp"
#include "../src/EntityManager.hpp"

TEST_CASE("Entity manager is initially empty") {
    EntityManager entityManager;

    CHECK(entityManager.getEntities().size() == 0);
}

TEST_CASE("Entity manager correctly creates entities") {
    EntityManager entityManager;
    entityManager.addEntity("test");
    entityManager.update();

    CHECK(entityManager.getEntities().size() == 1);
    CHECK(entityManager.getEntities()[0]->tag() == "test");
    CHECK(entityManager.getEntities("test").size() == 1);
    CHECK(entityManager.getEntities("test")[0]->tag() == "test");
}

TEST_CASE("Entity manager correctly updates with one entity") {
    EntityManager entityManager;
    entityManager.addEntity("test");

    CHECK(entityManager.getEntities().size() == 0);
    CHECK(entityManager.getEntities("test").size() == 0);

    entityManager.update();
    
    CHECK(entityManager.getEntities().size() == 1);
    CHECK(entityManager.getEntities("test").size() == 1);

    entityManager.getEntities("test")[0]->destroy();
    entityManager.update();

    CHECK(entityManager.getEntities().size() == 0);
    CHECK(entityManager.getEntities("test").size() == 0);
}

TEST_CASE("Entity manager correctly updates with multiple of one entity type") {
    EntityManager entityManager;
    entityManager.addEntity("entity");
    entityManager.addEntity("entity");
    entityManager.addEntity("entity");
    entityManager.addEntity("entity");
    
    CHECK(entityManager.getEntities("entity").size() == 0);

    entityManager.update();
    
    CHECK(entityManager.getEntities("entity").size() == 4);

    entityManager.getEntities()[0]->destroy();
    entityManager.getEntities()[2]->destroy();
    entityManager.update();

    CHECK(entityManager.getEntities("entity").size() == 2);
}

TEST_CASE("Entity manager correctly updates with multiple entity types") {
    EntityManager entityManager;
    entityManager.addEntity("test");
    entityManager.addEntity("test");
    entityManager.addEntity("entity");
    entityManager.addEntity("entity");
    entityManager.addEntity("entity");

    CHECK(entityManager.getEntities().size() == 0);

    entityManager.update();

    CHECK(entityManager.getEntities().size() == 5);
    CHECK(entityManager.getEntities("test").size() == 2);
    CHECK(entityManager.getEntities("entity").size() == 3);

    entityManager.getEntities("test")[0]->destroy();
    entityManager.getEntities("entity")[0]->destroy();
    entityManager.getEntities("entity")[1]->destroy();

    CHECK(entityManager.getEntities().size() == 5);
    CHECK(entityManager.getEntities("test").size() == 2);
    CHECK(entityManager.getEntities("entity").size() == 3);

    entityManager.update();

    CHECK(entityManager.getEntities().size() == 2);
    CHECK(entityManager.getEntities("test").size() == 1);
    CHECK(entityManager.getEntities("entity").size() == 1);
}

TEST_CASE("Assigns the correct ids to new entities") {
    EntityManager entityManager;

    entityManager.addEntity("test");
    entityManager.update();
    CHECK(entityManager.getEntities().size() == 1);
    CHECK(entityManager.getEntities()[0]->id() == 0);

    entityManager.addEntity("test");
    entityManager.update();
    CHECK(entityManager.getEntities().size() == 2);
    CHECK(entityManager.getEntities()[1]->id() == 1);

    entityManager.getEntities()[0]->destroy();
    entityManager.update();
    entityManager.addEntity("test");
    entityManager.update();
    CHECK(entityManager.getEntities().size() == 2);
    CHECK(entityManager.getEntities()[1]->id() == 2);
}

TEST_CASE("Correctly destroys entities") {
    EntityManager entityManager;

    entityManager.addEntity("test");
    entityManager.update();
    CHECK(entityManager.getEntities().size() == 1);
    CHECK(entityManager.getEntities("test").size() == 1);

    entityManager.getEntities()[0]->destroy();
    entityManager.update();
    CHECK(entityManager.getEntities().size() == 0);
    CHECK(entityManager.getEntities("test").size() == 0);
}
