#ifndef BULLET_H
#define BULLET_H

#include <SDL.h>

class Bullet {
public:
    float x, y;
    float vx, vy;
    SDL_Texture* texture;

    Bullet(float x, float y, float vx, float vy, SDL_Texture* texture);

    void update(float deltaTime);
    void render(SDL_Renderer* renderer) const; // Make sure this is const
};

#endif // BULLET_H
