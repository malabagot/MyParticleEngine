#pragma once
struct Vec2 {double x; double y;};

Vec2 operator+(Vec2 a, Vec2 b);
Vec2 operator-(Vec2 a, Vec2 b);
Vec2 operator*(Vec2 a, double b);
Vec2 operator*(double a, Vec2 b);
Vec2 operator/(Vec2 a, double b);
void operator+=(Vec2 &a, Vec2 b);
void operator-=(Vec2 &a, Vec2 b);