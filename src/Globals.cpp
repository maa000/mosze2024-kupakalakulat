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
const float BULLET_SPEED = 0.5f;
int enemiesDefeated = 0;

bool checkCollision(const SDL_Rect& rectA, const SDL_Rect& rectB) {
    return rectA.x < rectB.x + rectB.w &&
           rectA.x + rectA.w > rectB.x &&
           rectA.y < rectB.y + rectB.h &&
           rectA.y + rectA.h > rectB.y;
}
