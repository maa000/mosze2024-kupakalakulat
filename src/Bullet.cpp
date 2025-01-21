#include "Bullet.h"
#include <iostream>

Bullet::Bullet(float x, float y, float vx, float vy, SDL_Texture* texture)
    : x(x), y(y), vx(vx), vy(vy), texture(texture) {}

void Bullet::update(float deltaTime) {
    std::cout << "Updating bullet at (" << x << ", " << y << "), vx: " << vx << ", vy: " << vy << std::endl;
    x += vx * deltaTime;
    y += vy * deltaTime;
}

void Bullet::render(SDL_Renderer* renderer) const {
    if (!texture) {
        std::cerr << "HIBA: Bullet textúra üres, nem lehet kirajzolni!" << std::endl;
        return;
    }

    SDL_Rect destRect = { static_cast<int>(x), static_cast<int>(y), 16, 16 };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}
