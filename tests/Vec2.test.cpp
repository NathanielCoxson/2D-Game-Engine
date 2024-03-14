#include "Vec2.hpp"
#include <assert.h>

int main() {
    Vec2 vec1 = Vec2(1, 1);
    Vec2 vec2 = Vec2(2, 2);

    bool vecEquals = vec1 == vec2;
    assert(!vecEquals);

    bool vecNotEquals = vec1 != vec2;
    assert(vecNotEquals);

    return 0;
}
