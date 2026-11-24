#pragma once
struct Vec2 {double x; double y;};

// defined here (with inline) so every file can see the bodies and the compiler can paste them in instead of making a function call
inline Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
inline Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
inline Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
inline Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}
inline Vec2 operator/(Vec2 a, double b) {return {a.x/b, a.y/b};}
inline void operator+=(Vec2 &a, Vec2 b) {a.x += b.x; a.y += b.y;}
inline void operator-=(Vec2 &a, Vec2 b) {a.x -= b.x; a.y -= b.y;}
