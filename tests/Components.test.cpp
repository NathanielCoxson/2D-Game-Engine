#include "doctest.hpp"
#include "../src/Components.hpp"

TEST_CASE("CBoundingBox has correct initial size using default constructor.") {
    CBoundingBox bb = CBoundingBox();

    CHECK(bb.size.x == 0);
    CHECK(bb.size.y == 0);
}

TEST_CASE("CBoundingBox has correct initial size when width and height are provided.") {
    CBoundingBox bb = CBoundingBox(1.0, 2.0);
    
    CHECK(bb.size.x == 1.0);
    CHECK(bb.size.y == 2.0);
}
