#include <iostream>
#include <string>
#include <vector>

int main() {
    struct Vec2 {double x; double y;};
    Vec2 position{0.0, 0.0};
    position.x = 5.0;
    position.y = 10.0;
    std::cout << position.x << ", " << position.y;

    Vec2 velocity{1.0, 1.0};

    for (int i=0; i<10; i++){
        position.x += velocity.x;
        position.y += velocity.y;
    }
    std::cout << "\n" << position.x << ", " << position.y;
}
