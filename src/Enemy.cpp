#include "Enemy.h"
#include "Bullet.h"
#include "Globals.h"
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

void Enemy::update(float deltaTime, const SDL_DisplayMode& displayMode) {
    // Calculate direction to the player
    float dirX = player.position.x + player.position.w / 2 - (x + width / 2);
    float dirY = player.position.y + player.position.h / 2 - (y + height / 2);
    float length = sqrt(dirX * dirX + dirY * dirY);

    if (length != 0) {
        dirX /= length; // Normalize direction vector
        dirY /= length;
    }

    // Update velocity based on direction
    float speed = 0.1f; // Adjust this value for enemy speed
    vx = dirX * speed;
    vy = dirY * speed;

    // Update position
    x += vx * deltaTime;
    y += vy * deltaTime;

    // Update bullets
    for (auto& bullet : bullets) {
        bullet.update(deltaTime); // Ensure bullets move
    }

    // Remove bullets that are off-screen
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [&](const Bullet& b) {
                                     return b.x < 0 || b.x > displayMode.w || b.y < 0 || b.y > displayMode.h;
                                 }),
                  bullets.end());
}


void Enemy::render(SDL_Renderer* renderer) {
    if (!texture) {
        std::cerr << "HIBA: Enemy textúra üres, nem lehet kirajzolni!" << std::endl;
        return;
    }

    // Render the enemy
    SDL_Rect destRect = {static_cast<int>(x), static_cast<int>(y), width, height};
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    // Render bullets
    for (const auto& bullet : bullets) {
        bullet.render(renderer);
    }

    // Render the enemy's health bar
    int barWidth = width; // Same as enemy width
    int barHeight = 5;    // Thin health bar
    int barX = static_cast<int>(x);
    int barY = static_cast<int>(y) - barHeight - 5; // Positioned just above the enemy

    float healthRatio = static_cast<float>(health) / 3.0f; // Assuming enemy max health is 3
    int filledWidth = static_cast<int>(barWidth * healthRatio);

    // Draw the empty part of the health bar (gray background)
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray
    SDL_Rect emptyBarRect = {barX, barY, barWidth, barHeight};
    SDL_RenderFillRect(renderer, &emptyBarRect);

    // Draw the filled part of the health bar (green foreground)
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
    SDL_Rect filledBarRect = {barX, barY, filledWidth, barHeight};
    SDL_RenderFillRect(renderer, &filledBarRect);
}

void Enemy::shoot(SDL_Texture* enemyBulletTexture) {
    if (!enemyBulletTexture) {
        std::cerr << "HIBA: Lövedék textúra üres, nem lehet lövést létrehozni!" << std::endl;
        return;
    }

    // Calculate direction to the player
    float dirX = player.position.x + player.position.w / 2 - (x + width / 2);
    float dirY = player.position.y + player.position.h / 2 - (y + height / 2);
    float length = sqrt(dirX * dirX + dirY * dirY);

    if (length != 0) {
        dirX /= length; // Normalize
        dirY /= length;

        // Add a new bullet toward the player
        bullets.emplace_back(x + width / 2, y + height / 2, dirX * BULLET_SPEED, dirY * BULLET_SPEED, enemyBulletTexture);
    }
}

bool Enemy::isAlive() const {
    return health > 0;
}
