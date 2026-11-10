#include "vec2.h"

Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}
Vec2 operator/(Vec2 a, double b) {return {a.x/b, a.y/b};}
void operator+=(Vec2 &a, Vec2 b) {a.x += b.x; a.y += b.y;}
void operator-=(Vec2 &a, Vec2 b) {a.x -= b.x; a.y -= b.y;}