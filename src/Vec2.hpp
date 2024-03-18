#pragma once

class Vec2 {

public:

    float x;
    float y;

    Vec2(float x_in, float y_in);

    Vec2 operator +  (const Vec2& rhs) const;
    Vec2 operator -  (const Vec2& rhs) const;
    Vec2 operator *  (const Vec2& rhs) const;
    Vec2 operator /  (const Vec2& rhs) const;

    void operator += (const Vec2& rhs);
    void operator -= (const Vec2& rhs);
    void operator *= (const Vec2& rhs);
    void operator /= (const Vec2& rhs);

    bool operator == (const Vec2& rhs) const;
    bool operator != (const Vec2& rhs) const;

    void  normalize ();
    float length    ()                const;
    float dist      (const Vec2& rhs) const;

};

