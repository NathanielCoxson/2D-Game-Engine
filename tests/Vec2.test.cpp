#include "../src/Vec2.hpp"
#include "doctest.hpp"
#include <cmath>

Vec2 vec1 = Vec2(1, 1);
Vec2 vec2 = Vec2(2, 2);

TEST_CASE("equals operator works") {
    CHECK(!(vec1 == vec2));
}

TEST_CASE("not equals operator works") {
    CHECK(vec1 != vec2);
}

TEST_CASE("addition operators work") {
    Vec2 added_vectors = vec1 + vec2;
    Vec2 shorthand_add(1, 1);
    shorthand_add += Vec2(1, 1);

    CHECK(added_vectors.x == vec1.x + vec2.x);
    CHECK(added_vectors.y == vec1.y + vec2.y);
    CHECK(shorthand_add.x == 2);
    CHECK(shorthand_add.y == 2);
}

TEST_CASE("subtraction operators") {
    Vec2 subtracted_vectors = vec1 - vec2;
    Vec2 shorthand_subtract(1, 1);
    shorthand_subtract -= Vec2(1, 1);

    CHECK(subtracted_vectors.x == vec1.x - vec2.x);
    CHECK(subtracted_vectors.y == vec1.y - vec2.y);
    CHECK(shorthand_subtract.x == 0);
    CHECK(shorthand_subtract.y == 0);
}

TEST_CASE("multiplication operators work") {
    Vec2 multiplied_vectors = vec1 * vec2;
    Vec2 shorthand_multiply(2, 2);
    shorthand_multiply *= Vec2(2, 2);

    CHECK(multiplied_vectors.x == vec1.x * vec2.x);
    CHECK(multiplied_vectors.y == vec1.y * vec2.y);
    CHECK(shorthand_multiply.x == 4);
    CHECK(shorthand_multiply.y == 4);
}

TEST_CASE("division operators work") {
    Vec2 divided_vectors = vec1 / vec2;
    Vec2 shorthand_divide(2, 2);
    shorthand_divide /= Vec2(1, 1);

    CHECK(divided_vectors.x == vec1.x / vec2.x);
    CHECK(divided_vectors.y == vec1.y / vec2.y);
    CHECK(shorthand_divide.x == 2);
    CHECK(shorthand_divide.y == 2);
}

TEST_CASE("dist returns the correct distance") {
    float distance = vec1.dist(vec2);
    float expected_distance = sqrt( pow(vec1.x - vec2.x, 2) + pow(vec1.y - vec2.y, 2) );

    CHECK(distance == expected_distance);
}

TEST_CASE("length returns the correct length") {
    float length = vec1.length();
    float expected_length = sqrt( pow(vec1.x, 2) + pow(vec1.y, 2) );

    CHECK(length == expected_length);
}

TEST_CASE("normalize works") {
    Vec2 normalize_test(2, 2);
    float length = sqrt( pow(normalize_test.x, 2) + pow(normalize_test.y, 2) );
    float expected_x = normalize_test.x / length;
    float expected_y = normalize_test.y / length;
    float expected_new_length = 1;

    normalize_test.normalize();
    // round had to be used because the new length was not precise enough
    // to test equality with 1. The result was something like 0.999999986, 
    // and not exactly one.
    float new_length = round(normalize_test.length());

    CHECK(normalize_test.x == expected_x);
    CHECK(normalize_test.y == expected_y);
    CHECK(new_length == expected_new_length);
}
