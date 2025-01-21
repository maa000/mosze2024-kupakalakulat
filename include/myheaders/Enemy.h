#ifndef ENEMY_H
#define ENEMY_H

#include "Bullet.h"
#include <SDL.h>
#include <vector>

class Enemy {
public:
    float x, y;                   // Position
    float vx, vy;                 // Velocity
    int health;                   // Health
    SDL_Texture* texture;         // Texture
    std::vector<Bullet> bullets;  // Bullets
    int width, height;            // Dimensions

    // Constructor
    Enemy(float x, float y, float vx, float vy, int health, SDL_Texture* texture);

    // Movement
    void update(float deltaTime);

    // Rendering
    void render(SDL_Renderer* renderer);

    // Shooting
    void shoot(SDL_Texture* bulletTexture);

    // Check if the enemy is alive
    bool isAlive() const;
};

#endif // ENEMY_H
