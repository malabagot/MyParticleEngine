#include "playerInteraction.h"
#include "config.h"
#include "raylib.h"

void particleSpawnOnClick(std::vector<Particle>& particle_list) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                Particle new_particle {
                    {
                        GetRandomValue(-100,100)/500.0+(double)GetMouseX()/worldToScreenScale,
                        GetRandomValue(-100,100)/500.0+(double)(worldHeight*worldToScreenScale - GetMouseY())/worldToScreenScale
                    },
                    {
                        (double)GetMouseX()/worldToScreenScale,
                        (double)(worldHeight*worldToScreenScale - GetMouseY())/worldToScreenScale
                    },
                    {0.0, -9.81
                    }
                };
                particle_list.push_back(new_particle);
            }
}