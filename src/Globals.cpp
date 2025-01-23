#include "Globals.h"
#include "Bullet.h" // Szükséges a std::vector<Bullet> miatt

// Globális változók definíciói
Player player(100, 0, 0);
SDL_Texture* shipTexture = nullptr;
SDL_Texture* newGameTexture = nullptr;
SDL_Texture* settingsTexture = nullptr;
SDL_Texture* creditsTexture = nullptr;
SDL_Texture* exitTexture = nullptr;
SDL_Texture* gameBackgroundTexture = nullptr;
std::vector<int> buttonDistances;
SDL_Texture* button1Texture = nullptr;
SDL_Texture* button2Texture = nullptr;
SDL_Texture* button3Texture = nullptr;
SDL_Texture* button4Texture = nullptr;
SDL_Texture* button5Texture = nullptr;
SDL_Texture* button6Texture = nullptr;
SDL_Texture* button7Texture = nullptr;
SDL_Texture* playerBulletTexture = nullptr;
SDL_Texture* enemyBulletTexture = nullptr;
std::vector<Enemy> enemies;
SDL_Texture* enemyTexture = nullptr;
Uint32 lastEnemySpawnTime = 0;
const Uint32 spawnInterval = 3000;
std::vector<Bullet> playerBullets;
GameState currentState = MENU;
const float BULLET_SPEED = 0.8f;
int enemiesDefeated = 0;

bool checkCollision(const SDL_Rect& rectA, const SDL_Rect& rectB) {
    return rectA.x < rectB.x + rectB.w &&
           rectA.x + rectA.w > rectB.x &&
           rectA.y < rectB.y + rectB.h &&
           rectA.y + rectA.h > rectB.y;
}

void renderHealthBar(SDL_Renderer* renderer, const Player& player, int screenWidth) {
    int barWidth = 200; // Width of the health bar
    int barHeight = 25; // Height of the health bar
    int borderThickness = 2; // Border thickness

    // Calculate bar position
    int x = screenWidth / 2 - barWidth / 2; // Center horizontally
    int y = 20; // Distance from the top of the screen

    // Calculate filled width based on player's health
    float healthRatio = static_cast<float>(player.health) / 100.0f;
    int filledWidth = static_cast<int>(barWidth * healthRatio);

    // Draw the border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_Rect borderRect = {x - borderThickness, y - borderThickness, barWidth + 2 * borderThickness, barHeight + 2 * borderThickness};
    SDL_RenderFillRect(renderer, &borderRect);

    // Draw the empty part of the bar
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray color
    SDL_Rect emptyBarRect = {x, y, barWidth, barHeight};
    SDL_RenderFillRect(renderer, &emptyBarRect);

    // Draw the filled part of the bar
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color
    SDL_Rect filledBarRect = {x, y, filledWidth, barHeight};
    SDL_RenderFillRect(renderer, &filledBarRect);
}
