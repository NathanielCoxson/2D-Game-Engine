#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.hpp"

int number() { return 10; }

TEST_CASE("testing that number returns 10") {
    CHECK(number() == 10);
}
