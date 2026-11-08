#include <iostream>
#include <string>
#include <vector>

struct Vec2 {double x; double y;};
struct Particle {Vec2 position; Vec2 Velocity;};

Vec2 operator+(Vec2 a, Vec2 b) {return {a.x + b.x, a.y + b.y};}
Vec2 operator-(Vec2 a, Vec2 b) {return {a.x - b.x, a.y - b.y};}
Vec2 operator*(Vec2 a, double b) {return {a.x * b, a.y * b};}
Vec2 operator*(double a, Vec2 b) {return {b.x * a, b.y * a};}


int main() {
    Particle particle1 {{0.0,0.0},{1.0,1.0}};
    Particle particle2 {{0.0,0.0},{2.0,2.0}};
    Particle particle3 {{0.0,0.0},{3.0,3.0}};
    Particle particle4 {{0.0,0.0},{4.0,4.0}};

    Vec2 gravity{0.0, -9.81};
    double dt = 0.1;

    std::vector<Particle> particle_list {particle1, particle2, particle3, particle4};

    for (double i=0.0; i<9.999; i+=dt){
        for (Particle &particle : particle_list){
            particle.Velocity = particle.Velocity + gravity * dt;
            particle.position = particle.position + particle.Velocity * dt;
        }
    }

    for (Particle particle : particle_list){
        std::cout << "Particle position: (" << particle.position.x << ", " << particle.position.y << ")\n";
    }

}
