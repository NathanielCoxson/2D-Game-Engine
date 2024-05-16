#include "doctest.hpp"
#include "../src/Action.hpp"

TEST_CASE("An action is initialized with the correct name and type.") {
    Action a = Action("move", "keydown");

    CHECK(a.getName().compare("move") == 0);
    CHECK(a.getType().compare("keydown") == 0);
}
