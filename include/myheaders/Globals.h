#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"

#include <SDL_ttf.h>

// Enum a játék állapotaihoz
enum GameState {
    MENU,
    MAP,
    ROOM1, // A ROOM1-ROOM7 az egyes gombokhoz tartozó oldalak.
    ROOM2,
    ROOM3,
    ROOM4,
    ROOM5,
    ROOM6,
    ROOM7
};

// Globális változók deklarációi
extern Player player;
extern SDL_Texture* shipTexture;
extern SDL_Texture* newGameTexture;
extern SDL_Texture* settingsTexture;
extern SDL_Texture* creditsTexture;
extern SDL_Texture* exitTexture;
extern SDL_Texture* gameBackgroundTexture;
extern std::vector<int> buttonDistances;
extern SDL_Texture* button1Texture;
extern SDL_Texture* button2Texture;
extern SDL_Texture* button3Texture;
extern SDL_Texture* button4Texture;
extern SDL_Texture* button5Texture;
extern SDL_Texture* button6Texture;
extern SDL_Texture* button7Texture;
extern SDL_Texture* bulletTexture;
extern std::vector<Enemy> enemies;
extern SDL_Texture* enemyTexture;
extern Uint32 lastEnemySpawnTime;
extern const Uint32 spawnInterval;
extern std::vector<Bullet> playerBullets;
extern GameState currentState;
extern const float BULLET_SPEED;
bool checkCollision(const SDL_Rect& rectA, const SDL_Rect& rectB);
//eletero
extern float playerHealth; // Játékos életereje százalékban
extern TTF_Font* font;


#endif // GLOBALS_H
