#include "Vec2.hpp"
#include <cmath>

Vec2::Vec2(float x_in, float y_in): x(x_in), y(y_in) {}

Vec2 Vec2::operator + (const Vec2& rhs) const {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const Vec2& rhs) const {
    return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator / (const Vec2& rhs) const {
    return Vec2(x / rhs.x, y / rhs.y);
}

bool Vec2::operator == (const Vec2& rhs) const {
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const {
    return x != rhs.x || y != rhs.y;
}

void Vec2::normalize() {
    double length = this->length();
    x = x / length;
    y = y / length;
}

double Vec2::length() const {
    return std::sqrt((x * x) + (y * y));
}
