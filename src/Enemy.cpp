#include "Enemy.h"
#include "Bullet.h"
#include <cmath>
#include <iostream>
#include <algorithm> // For std::remove_if

// Enemy constructor
Enemy::Enemy(float x, float y, float vx, float vy, int health, SDL_Texture* texture)
    : x(x), y(y), vx(vx), vy(vy), health(health), texture(texture) {
    if (texture) {
        // Get the width and height of the texture
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    } else {
        // Fallback dimensions if the texture is missing
        width = 64;
        height = 64;
    }
}

void Enemy::update(float deltaTime) {
    x += vx * deltaTime;
    y += vy * deltaTime;

    if (x <= 0 || x >= 1280 - width) {
        vx = -vx; // Reverse direction
    }

    // Simulate shooting
    if (std::rand() % 100 < 5) { // 5% chance to shoot per update
        shoot(texture); // Ensure the correct texture is passed
    }

    // Update bullets
    for (auto& bullet : bullets) {
        std::cout << "Updating bullet at (" << bullet.x << ", " << bullet.y << ")" << std::endl;
        bullet.update(deltaTime);
    }

    // Remove bullets that are off-screen
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet& b) {
                                     return b.x < 0 || b.x > 1280 || b.y < 0 || b.y > 720;
                                 }),
                  bullets.end());
}
void Enemy::render(SDL_Renderer* renderer) {
    if (!texture) {
        std::cerr << "HIBA: Enemy textúra üres, nem lehet kirajzolni!" << std::endl;
        return;
    }

    // Render the enemy
    SDL_Rect destRect = { static_cast<int>(x), static_cast<int>(y), width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    // Render bullets
    for (const auto& bullet : bullets) {
        bullet.render(renderer);
    }
}

void Enemy::shoot(SDL_Texture* bulletTexture) {
    if (!bulletTexture) {
        std::cerr << "HIBA: Lövedék textúra üres, nem lehet lövést létrehozni!" << std::endl;
        return;
    }
    else{
        std::cerr << bulletTexture << std::endl;
    }

    // Add a new bullet to the vector
    bullets.emplace_back(x + width / 2, y + height, 0, 0.3f, bulletTexture);
}

bool Enemy::isAlive() const {
    return health > 0;
}
