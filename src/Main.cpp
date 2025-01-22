#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stb/stb_image.h>
#include <iostream>
#include <vector>
#include <cstdlib> // Véletlen számokhoz
#include <ctime>
#include <cmath>
#include "myheaders/SaveManager.h"
#include "myheaders/Map.h"
#include "myheaders/Room.h"
#include "myheaders/Player.h"
#include "myheaders/Enemy.h"
#include "myheaders/Globals.h"

#include <SDL_image.h>


// Textúrák betöltése SDL-hez
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        std::cerr << "Kép betöltési hiba! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!newTexture) {
            std::cerr << "Textúra létrehozási hiba! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}
// Menü textúrák betöltése
void LoadMenuTextures(SDL_Renderer* renderer) {
    newGameTexture = loadTexture("res/newgame.png", renderer);
    settingsTexture = loadTexture("res/settings.png", renderer);
    creditsTexture = loadTexture("res/credits.png", renderer);
    exitTexture = loadTexture("res/exit.png", renderer);
    if (!newGameTexture || !settingsTexture || !creditsTexture || !exitTexture) {
        std::cerr << "Egy vagy több textúra betöltése sikertelen. Ellenőrizd az útvonalakat és a fájlformátumokat." << std::endl;
    }
}
// Játék textúrák betöltése
void LoadGameTextures(SDL_Renderer* renderer) {
    gameBackgroundTexture = loadTexture("res/mapbg.png", renderer);
    button1Texture = loadTexture("res/Harc_hatter.png", renderer);
    button2Texture = loadTexture("res/Trade_hatter.png", renderer);
    button3Texture = loadTexture("res/Unknow_hatter.png", renderer);
    button4Texture = loadTexture("res/Hp_hatter.png", renderer);
    button5Texture = loadTexture("res/Finalboss_hatter.png", renderer);
    button6Texture = loadTexture("res/Finalboss_hatter.png", renderer);
    button7Texture = loadTexture("res/FinalFinalboss_hatterr.png", renderer);

    playerBulletTexture = loadTexture("res/loves_2.png", renderer); // Lövedék sprite
    if (!playerBulletTexture) {
        std::cerr << "HIBA: Lövedék textúra betöltése sikertelen!" << std::endl;
    }

    enemyBulletTexture = loadTexture("res/loves_1.png", renderer); // Lövedék sprite
    if (!enemyBulletTexture) {
        std::cerr << "HIBA: Lövedék textúra betöltése sikertelen!" << std::endl;
    }

    if (!gameBackgroundTexture || !button1Texture || !button2Texture || !button3Texture ||
        !button4Texture || !button5Texture || !button6Texture || !button7Texture) {
        std::cerr << "Egy vagy több játék textúra betöltése sikertelen!" << std::endl;
    }

    enemyTexture = loadTexture("res/enemy1.png", renderer);
    if (!enemyTexture) {
        std::cerr << "Ellenség textúra betöltési hiba!" << std::endl;
}

    if (!enemyTexture) {
        std::cerr << "HIBA: Nem sikerült betölteni az enemy textúrát! Ellenőrizd az elérési utat: res/hajo1.png" << std::endl;
    }
}

    void TestTexturesFunc() {
    if (!gameBackgroundTexture) std::cerr << "HIBA: gameBackgroundTexture nem töltődött be!" << std::endl;
    if (!button1Texture) std::cerr << "HIBA: button1Texture nem töltődött be!" << std::endl;
    if (!button2Texture) std::cerr << "HIBA: button2Texture nem töltődött be!" << std::endl;
    if (!button3Texture) std::cerr << "HIBA: button3Texture nem töltődött be!" << std::endl;
    if (!button4Texture) std::cerr << "HIBA: button4Texture nem töltődött be!" << std::endl;
    if (!button5Texture) std::cerr << "HIBA: button5Texture nem töltődött be!" << std::endl;
    if (!button6Texture) std::cerr << "HIBA: button6Texture nem töltődött be!" << std::endl;
    if (!button7Texture) std::cerr << "HIBA: button7Texture nem töltődött be!" << std::endl;
    if (!enemyTexture) std::cerr << "HIBA: enemyTexture nem töltődött be!" << std::endl;
    if (!playerBulletTexture) std::cerr << "HIBA: bulletTexture nem töltődött be!" << std::endl;
    if (!enemyBulletTexture) std::cerr << "HIBA: bulletTexture nem töltődött be!" << std::endl;
}

// Véletlenszerű távolságok generálása a gombokhoz
std::vector<int> GenerateRandomDistances(int screenWidth, int numButtons, int buttonWidth) {
    std::vector<int> positions;
    int totalWidth = numButtons * buttonWidth;
    if (totalWidth > screenWidth) {
        std::cerr << "Gombok nem férnek el a képernyőn!" << std::endl;
        return positions;
    }
    int remainingSpace = screenWidth - totalWidth;
    std::srand(std::time(nullptr)); // Véletlenszám generátor inicializálása
    for (int i = 0; i < numButtons - 1; ++i) {
        int spacing = std::rand() % (remainingSpace / (numButtons - i));
        positions.push_back(spacing);
        remainingSpace -= spacing;
    }
    positions.push_back(remainingSpace); // Az utolsó gombhoz maradék hely
    return positions;
}

std::vector<int> GenerateRandomYOffsets(int numButtons, int maxOffset) {
    std::vector<int> yOffsets;
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Véletlenszám-generátor inicializálása

    for (int i = 0; i < numButtons; ++i) {
        int offset = (std::rand() % (2 * maxOffset + 1)) - maxOffset; // Véletlen érték [-maxOffset, +maxOffset]
        yOffsets.push_back(offset);
    }

    return yOffsets;
}


// Item osztály definiálása
class Item {
public:
    std::string name;           // Az item neve
    std::string description;    // Az item leírása
    int id;                     // Az item egyedi azonosítója
    SDL_Texture* texture;       // Az itemhez tartozó textúra
    std::function<void()> effect; // Az item hatása (lambda vagy függvény)

    // Konstruktor
    Item(const std::string& name, const std::string& description, int id, SDL_Texture* texture, std::function<void()> effect)
        : name(name), description(description), id(id), texture(texture), effect(effect) {}
    
    // Az item használata
    void use() {
        if (effect) {
            effect();
        }
    }
};


void DrawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    float angle = atan2(y2 - y1, x2 - x1); // Vonal irányszöge
    float sinAngle = sin(angle);
    float cosAngle = cos(angle);

    // Téglalap négy sarkának kiszámítása
    SDL_Point points[4] = {
        {static_cast<int>(std::round(x1 + thickness / 2 * -sinAngle)), static_cast<int>(std::round(y1 + thickness / 2 * cosAngle))},
        {static_cast<int>(std::round(x2 + thickness / 2 * -sinAngle)), static_cast<int>(std::round(y2 + thickness / 2 * cosAngle))},
        {static_cast<int>(std::round(x2 - thickness / 2 * -sinAngle)), static_cast<int>(std::round(y2 - thickness / 2 * cosAngle))},
        {static_cast<int>(std::round(x1 - thickness / 2 * -sinAngle)), static_cast<int>(std::round(y1 - thickness / 2 * cosAngle))},
    };

    // Téglalap kitöltése pontok közötti szakaszokkal
    for (int i = 0; i < thickness; ++i) {
        int offsetX = static_cast<int>(std::round(i * sinAngle));
        int offsetY = static_cast<int>(std::round(i * cosAngle));

        SDL_RenderDrawLine(renderer,
                           points[0].x + offsetX, points[0].y + offsetY,
                           points[1].x + offsetX, points[1].y + offsetY);
    }
}

bool handleButtonClick(int mouseX, int mouseY, int screenWidth, int screenHeight) {
    int buttonWidth = 120;
    int buttonHeight = 120;

    // Ellenőrzés: buttonDistances mérete
    if (buttonDistances.size() < 7) {
        std::cerr << "HIBA: buttonDistances mérete nem megfelelő! Méret: " << buttonDistances.size() << std::endl;
        return false;
    }

    // Y eltolások inicializálása (ha szükséges)
    static std::vector<int> yOffsets = GenerateRandomYOffsets(7, 50);
    if (yOffsets.size() < 7) {
        std::cerr << "HIBA: yOffsets mérete nem megfelelő! Méret: " << yOffsets.size() << std::endl;
        return false;
    }

    // Gombok vízszintes pozícióinak számítása
    int totalButtonWidth = 0;
    for (int i = 0; i < 7; ++i) {
        totalButtonWidth += buttonWidth + buttonDistances[i];
    }
    totalButtonWidth -= buttonDistances.back(); // Az utolsó távolságot kivonjuk
    int xPos = (screenWidth - totalButtonWidth) / 2;


    // Gombok pozíciójának ellenőrzése
    for (int i = 0; i < 7; ++i) {
        int yPos = (screenHeight / 2 - buttonHeight / 2) + yOffsets[i];
        SDL_Rect buttonRect = {xPos, yPos, buttonWidth, buttonHeight};

        if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
            std::cout << "Gomb " << i + 1 << " kattintva." << std::endl;
            // Állapot módosítása
            currentState = static_cast<GameState>(ROOM1 + i); // ROOM1-től ROOM7-ig
            return true;
        }

        xPos += buttonWidth + buttonDistances[i];
    }

    //std::cout << "Nem kattintottál egyetlen gombra sem." << std::endl;
    return false;
}

void RenderGameScreen(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    if (!gameBackgroundTexture) {
        std::cerr << "HIBA: gameBackgroundTexture nem lett betöltve!" << std::endl;
        exit(1);
    }

    SDL_RenderCopy(renderer, gameBackgroundTexture, nullptr, nullptr);

    if (buttonDistances.size() < 7) {
        std::cerr << "HIBA: buttonDistances mérete nem megfelelő!" << std::endl;
        exit(1);
    }

    static std::vector<int> yOffsets = GenerateRandomYOffsets(7, 50);
    if (yOffsets.size() < 7) {
        std::cerr << "HIBA: yOffsets mérete nem megfelelő!" << std::endl;
        exit(1);
    }

    int buttonWidth = 120;
    int buttonHeight = 120;
    int totalButtonWidth = 0;

    for (int i = 0; i < 7; ++i) {
        totalButtonWidth += buttonWidth + buttonDistances[i];
    }
    totalButtonWidth -= buttonDistances.back();
    int xPos = (screenWidth - totalButtonWidth) / 2;

    struct ButtonEdge {
        SDL_Point leftEdge;
        SDL_Point rightEdge;
    };
    std::vector<ButtonEdge> buttonEdges;

    for (int i = 0; i < 7; ++i) {
        SDL_Texture* buttonTexture = nullptr;
        switch (i) {
            case 0: buttonTexture = button1Texture; break;
            case 1: buttonTexture = button2Texture; break;
            case 2: buttonTexture = button3Texture; break;
            case 3: buttonTexture = button4Texture; break;
            case 4: buttonTexture = button5Texture; break;
            case 5: buttonTexture = button6Texture; break;
            case 6: buttonTexture = button7Texture; break;
        }

        if (!buttonTexture) {
            std::cerr << "HIBA: A " << i + 1 << ". gomb textúrája nem lett betöltve!" << std::endl;
            exit(1);
        }

        int yPos = (screenHeight / 2 - buttonHeight / 2) + yOffsets[i];
        SDL_Rect buttonRect = {xPos, yPos, buttonWidth, buttonHeight};
        SDL_RenderCopy(renderer, buttonTexture, nullptr, &buttonRect);

        SDL_Point leftEdge = {xPos, yPos + buttonHeight / 2};
        SDL_Point rightEdge = {xPos + buttonWidth, yPos + buttonHeight / 2};
        buttonEdges.push_back({leftEdge, rightEdge});

        xPos += buttonWidth + buttonDistances[i];
    }

    SDL_SetRenderDrawColor(renderer, 157, 165, 189, SDL_ALPHA_OPAQUE);
    for (size_t i = 0; i < buttonEdges.size() - 1; ++i) {
        DrawThickLine(renderer, 
                      buttonEdges[i].rightEdge.x, buttonEdges[i].rightEdge.y,
                      buttonEdges[i + 1].leftEdge.x, buttonEdges[i + 1].leftEdge.y, 
                      4);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}


void RenderRoom(SDL_Renderer* renderer, const std::string& backgroundPath, SDL_Texture* shipTexture) {
    SDL_Texture* roomBackground = loadTexture(backgroundPath, renderer);
    if (!roomBackground) {
        std::cerr << "HIBA: Szoba háttérkép betöltése sikertelen!" << std::endl;
        return;
    }

    SDL_RenderCopy(renderer, roomBackground, nullptr, nullptr); // Háttér megjelenítése
    SDL_DestroyTexture(roomBackground); // Nem kell többé

    // Hajó pozíciójának és textúrájának renderelése, ha szükséges
    if (shipTexture) {
    SDL_RenderCopy(renderer, shipTexture, nullptr, &player.position); // A hajó megjelenítése a frissített pozícióban
    }
    if (currentState == ROOM2) {
        for (const auto& enemy : enemies) {
            SDL_Rect enemyRect = {static_cast<int>(enemy.x), static_cast<int>(enemy.y), enemy.width, enemy.height};
            SDL_RenderCopy(renderer, enemyTexture, nullptr, &enemyRect);
            }
    }
    if (currentState != ROOM2) {
    enemies.clear(); // Töröld az összes ellenséget, amikor nem ROOM2-ben vagy
}
}
void TestPlayerClass();

int main(int argc, char* argv[]) {
    TestPlayerClass();
    
       // SDL inicializálása
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL inicializálási hiba: " << SDL_GetError() << std::endl;
        return -1;
    }
    // SDL_image inicializálása
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image inicializálási hiba: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
  
    // Teljes képernyős ablak létrehozása
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode); // Fő monitor mérete
    SDL_Window* window = SDL_CreateWindow("Mosze2024", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          displayMode.w, displayMode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    if (!window) {
        std::cerr << "SDL_Window létrehozási hiba: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    // SDL renderelő létrehozása
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_Renderer létrehozási hiba: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }
    // ImGui inicializálása
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Opcionális: Engedélyezheted a billentyűzetes navigációt
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    io.Fonts->Build();
   

    // Textúrák betöltése
    LoadMenuTextures(renderer);
    LoadGameTextures(renderer);


    TestTexturesFunc();

    buttonDistances = GenerateRandomDistances(displayMode.w, 7, 100); // Egyszeri távolsággenerálás

    // Háttér textúra betöltése
    SDL_Texture* backgroundTexture = loadTexture("res/bg.png", renderer);
    if (!backgroundTexture) {
        std::cerr << "Háttérkép betöltési hiba!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    //*************FŐ LOGIKA**************//
    bool run = true;
    bool inMenu = true; // Állapotkezelés: térkép vagy játék
    SDL_Point mouseClick = {0, 0}; // Egérkattintás pozíciója
    bool mouseClicked = false; // Jelzi, hogy történt-e kattintás

        // Hajó textúra betöltése
    shipTexture = loadTexture("res/hajo1.png", renderer);
    if (!shipTexture) {
        std::cerr << "HIBA: hajo1.png betöltése sikertelen!" << std::endl;
        run = false; // Ha nem sikerül betölteni, a program ne fusson tovább
    }

    bool keys[4] = {false, false, false, false}; // W, A, S, D nyomásának nyomon követése

    while (run) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) run = false;
        
    // Gombkattintás kezelése, ha nem a menüben vagyunk
            // Egérkattintás kezelése
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (currentState == MAP) { // Kiemelt sor
                    SDL_Point mouseClick = {event.button.x, event.button.y};
                    if (handleButtonClick(mouseClick.x, mouseClick.y, displayMode.w, displayMode.h)) {
                        // Állapotváltás a handleButtonClick-ben történik
                    }
    }
}

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float dirX = mouseX - (player.position.x + player.position.w / 2);
                float dirY = mouseY - (player.position.y + player.position.h / 2);
                float length = sqrt(dirX * dirX + dirY * dirY);

                if (length != 0) {
                    dirX /= length; // Normalizálás
                    dirY /= length;
                    playerBullets.emplace_back(player.position.x + player.position.w / 2,
                                               player.position.y + player.position.h / 2,
                                               dirX * BULLET_SPEED, dirY * BULLET_SPEED, playerBulletTexture);
                }
}

        // Billentyűzetes események kezelése
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_w: keys[0] = true; break; // W
                case SDLK_a: keys[1] = true; break; // A
                case SDLK_s: keys[2] = true; break; // S
                case SDLK_d: keys[3] = true; break; // D
            }
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_w: keys[0] = false; break; // W
                case SDLK_a: keys[1] = false; break; // A
                case SDLK_s: keys[2] = false; break; // S
                case SDLK_d: keys[3] = false; break; // D
            }
        }
    }

    //int enemiesDefeated = 0; // Globális változó a legyőzött ellenségek számának nyilvántartására

    

    // Enemy spawn logic
if (currentState == ROOM2 && SDL_GetTicks() - lastEnemySpawnTime >= spawnInterval) {
    // Generate a random x-coordinate within the screen width
    float x = static_cast<float>(std::rand() % displayMode.w);

    // Spawn within the top 100 pixels of the screen
    float y = static_cast<float>(std::rand() % 100);

    // Add the enemy to the list
    enemies.emplace_back(x, y, 0, 0, 3, enemyTexture); // 3 is the health of the enemy
    lastEnemySpawnTime = SDL_GetTicks();
}




    if (!enemyTexture) {
    std::cerr << "HIBA: Nem sikerült betölteni az enemy textúrát!" << std::endl;
    return 1;
    }

    // Ellenségek frissítése
    if (currentState == ROOM2) {
    // Ellenségek frissítése
        for (auto& enemy : enemies) {
            enemy.update(16, displayMode); // Delta idő 16ms
            if (std::rand() % 100 < 5) { // Véletlenszerű lövés
                enemy.shoot(enemyBulletTexture);
            }
        }

    // Töröld a képernyőn kívüli ellenségeket
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                             [&displayMode](const Enemy& e) { return e.y > displayMode.h || !e.isAlive(); }),
              enemies.end());

    

    std::cout << "Ellenségek száma: " << enemies.size() << std::endl;
    for (const auto& enemy : enemies) {
        std::cout << "Enemy at position (" << enemy.x << ", " << enemy.y << ")" << std::endl;
    }
    }


    // Mozgás frissítése (ez már a while (run) belsejében, de az események után van)
   const int shipSpeed = 8;
    if (keys[0]) player.position.y -= shipSpeed; // Felfelé
    if (keys[1]) player.position.x -= shipSpeed; // Balra
    if (keys[2]) player.position.y += shipSpeed; // Lefelé
    if (keys[3]) player.position.x += shipSpeed; // Jobbra

    // Képernyőhatárok ellenőrzése
    if (player.position.x < 0) player.position.x = 0;
    if (player.position.y < 0) player.position.y = 0;
    if (player.position.x + player.position.w > displayMode.w) player.position.x = displayMode.w - player.position.w;
    if (player.position.y + player.position.h > displayMode.h) player.position.y = displayMode.h - player.position.h;

    for (auto& bullet : playerBullets) {
        bullet.update(16); // Update all player bullets
    }

    // Update enemies
    for (auto& enemy : enemies) {
        enemy.update(16, displayMode); // Update enemy movement and bullets
    }

        
     // Check collisions for player bullets and enemies
    for (auto bulletIt = playerBullets.begin(); bulletIt != playerBullets.end();) {
    bool bulletRemoved = false;

    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        SDL_Rect bulletRect = {static_cast<int>(bulletIt->x), static_cast<int>(bulletIt->y), 16, 16};
        SDL_Rect enemyRect = {static_cast<int>(enemyIt->x), static_cast<int>(enemyIt->y), enemyIt->width, enemyIt->height};

        if (checkCollision(bulletRect, enemyRect)) {
            std::cout << "Bullet hit enemy at (" << enemyIt->x << ", " << enemyIt->y << ")" << std::endl;

            enemyIt->health -= 1; // Reduce enemy health
            if (enemyIt->health <= 0) {
                std::cout << "Enemy destroyed!" << std::endl;
                enemiesDefeated++; // Increment the global counter
                std::cout << "Enemies defeated: " << enemiesDefeated << std::endl;
                enemyIt = enemies.erase(enemyIt); // Remove enemy
            } else {
                ++enemyIt;
            }

            bulletIt = playerBullets.erase(bulletIt); // Remove bullet
            bulletRemoved = true;
            break;
        } else {
            ++enemyIt;
        }
    }

    if (!bulletRemoved) {
        bulletIt->update(16); // Update bullet if not removed
        ++bulletIt;
    }
}

         // Check collisions for enemy bullets and player
    for (auto& enemy : enemies) {
        for (auto bulletIt = enemy.bullets.begin(); bulletIt != enemy.bullets.end();) {
            SDL_Rect bulletRect = {static_cast<int>(bulletIt->x), static_cast<int>(bulletIt->y), 16, 16};
            SDL_Rect playerRect = {player.position.x, player.position.y, player.position.w, player.position.h};

            if (checkCollision(bulletRect, playerRect)) {
                std::cout << "Player hit by enemy bullet!" << std::endl;
                player.health -= 10; // Reduce player's health
                bulletIt = enemy.bullets.erase(bulletIt); // Remove bullet
            } else {
                ++bulletIt;
            }
        }
    }



        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    
        SDL_RenderClear(renderer);
    
         if (currentState == MENU) {
    // Render the main menu using ImGui
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    ImGui::Begin("Main Menu");

    // Window width
    ImVec2 windowSize = ImGui::GetWindowSize();
    float windowWidth = windowSize.x;

    // Button dimensions
    float buttonWidth = 225.0f;
    float buttonHeight = 45.0f;

    // Center alignment
    float centerX = (windowWidth - buttonWidth) / 2.0f;

    // New Game button
    ImGui::SetCursorPosX(centerX);
    if (ImGui::ImageButton("NewGameButton", (ImTextureID)(intptr_t)newGameTexture, ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "New Game clicked" << std::endl;
        currentState = MAP;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Settings button
    ImGui::SetCursorPosX(centerX);
    if (ImGui::ImageButton("SettingsButton", (ImTextureID)(intptr_t)settingsTexture, ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "Settings clicked" << std::endl;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Credits button
    ImGui::SetCursorPosX(centerX);
    if (ImGui::ImageButton("CreditsButton", (ImTextureID)(intptr_t)creditsTexture, ImVec2(buttonWidth, buttonHeight))) {
        std::cout << "Credits clicked" << std::endl;
    }

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    // Exit button
    ImGui::SetCursorPosX(centerX);
    if (ImGui::ImageButton("ExitButton", (ImTextureID)(intptr_t)exitTexture, ImVec2(buttonWidth, buttonHeight))) {
        run = false;
    }

    ImGui::End();
} else if (currentState == MAP) {
    RenderGameScreen(renderer, displayMode.w, displayMode.h);

    // Check if enemies are alive
    for (auto& enemy : enemies) {
        if (!enemy.isAlive()) {
            ++enemiesDefeated;
        }
    }

    
} else if (currentState >= ROOM1 && currentState <= ROOM7) {
    RenderRoom(renderer, "res/fg_map.png", shipTexture);

    // Check if the room is cleared
    if (enemiesDefeated >= 5) {
        currentState = MAP; // Return to the map view
        enemiesDefeated = 0; // Reset the counter
        enemies.clear();     // Clear enemy list
    }

    if (player.health <=0){
        currentState = MENU; // Return to the menu view
        enemies.clear();     // Clear enemy list
    }

    // Render player bullets
    for (auto& bullet : playerBullets) {
        bullet.render(renderer);
    }

    // Render all enemies and their bullets
    for (auto& enemy : enemies) {
        enemy.render(renderer);

        // Check for collisions between enemy bullets and the player
        for (auto bulletIt = enemy.bullets.begin(); bulletIt != enemy.bullets.end();) {
            SDL_Rect bulletRect = {static_cast<int>(bulletIt->x), static_cast<int>(bulletIt->y), 16, 16};
            SDL_Rect playerRect = {player.position.x, player.position.y, player.position.w, player.position.h};

            if (checkCollision(bulletRect, playerRect)) {
                std::cout << "Player hit by enemy bullet!" << std::endl;
                player.health -= 10; // Decrease player's health
                std::cout << "player hp" << std::endl;
                bulletIt = enemy.bullets.erase(bulletIt); // Remove bullet
            } else {
                ++bulletIt;
            }
        }
    }

    // Remove off-screen player bullets
    playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
                                   [&](const Bullet& b) {
                                       return b.x < 0 || b.x > displayMode.w || b.y < 0 || b.y > displayMode.h;
                                   }),
                    playerBullets.end());
}


        //else {
        //   
//
        //    try {
        //        RenderGameScreen(renderer, displayMode.w, displayMode.h);
        //    } catch (const std::exception& e) {
        //        std::cerr << "Hiba történt a RenderGameScreen futása közben: " << e.what() << std::endl;
        //        run = false;
        //    } catch (...) {
        //        std::cerr << "Ismeretlen hiba történt a RenderGameScreen futása közben!" << std::endl;
        //        run = false;
        //    }
//
        //    if (mouseClicked) {
//
        //        if (!handleButtonClick(mouseClick.x, mouseClick.y, displayMode.w, displayMode.h)) {
        //           // std::cerr << "Hiba történt a gombkattintás kezelése közben!" << std::endl;
        //        }
//
        //        mouseClicked = false;
        //    }
        //}
    
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
}


   
    

   // TISZTÍTÁS
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyTexture(gameBackgroundTexture);
    SDL_DestroyTexture(button1Texture);
    SDL_DestroyTexture(button2Texture);
    SDL_DestroyTexture(button3Texture);
    SDL_DestroyTexture(button4Texture);
    SDL_DestroyTexture(button5Texture);
    SDL_DestroyTexture(button6Texture);
    SDL_DestroyTexture(button7Texture);
    SDL_DestroyTexture(newGameTexture);
    SDL_DestroyTexture(settingsTexture);
    SDL_DestroyTexture(creditsTexture);
    SDL_DestroyTexture(exitTexture);
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}