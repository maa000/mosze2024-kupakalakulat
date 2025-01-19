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
#include <SDL_image.h>


Player player(100, 0, 0); // Player objektum létrehozása
SDL_Texture* shipTexture = nullptr; // Hajó textúra
// Globális textúra változók
SDL_Texture* newGameTexture;
SDL_Texture* settingsTexture;
SDL_Texture* creditsTexture;
SDL_Texture* exitTexture;
SDL_Texture* gameBackgroundTexture;

std::vector<int> buttonDistances;

// Gomb textúrák
SDL_Texture* button1Texture;//room0
SDL_Texture* button2Texture;//room1
SDL_Texture* button3Texture;//stb
SDL_Texture* button4Texture;
SDL_Texture* button5Texture;
SDL_Texture* button6Texture;
SDL_Texture* button7Texture;


SDL_Texture* bulletTexture = nullptr; // Lövedék textúra


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

    bulletTexture = loadTexture("res/loves_te.png", renderer); // Lövedék sprite
    if (!bulletTexture) {
        std::cerr << "HIBA: Lövedék textúra betöltése sikertelen!" << std::endl;
    }

    if (!gameBackgroundTexture || !button1Texture || !button2Texture || !button3Texture ||
        !button4Texture || !button5Texture || !button6Texture || !button7Texture) {
        std::cerr << "Egy vagy több játék textúra betöltése sikertelen!" << std::endl;
    }
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


class Bullet {
public:
    float x, y;
    float vx, vy;
    SDL_Texture* texture;

    Bullet(float x, float y, float vx, float vy, SDL_Texture* texture)
        : x(x), y(y), vx(vx), vy(vy), texture(texture) {}

    void update(float deltaTime) {
        x += vx * deltaTime;
        y += vy * deltaTime;
    }

    void render(SDL_Renderer* renderer) {
        SDL_Rect destRect = { static_cast<int>(x), static_cast<int>(y), 16, 16 };
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    }
};



std::vector<Bullet> playerBullets;          // Lövedékek tárolása
const float BULLET_SPEED = 0.5f;      // Lövedék sebessége

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

GameState currentState = MENU;


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
                                               dirX * BULLET_SPEED, dirY * BULLET_SPEED, bulletTexture);
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

    // Mozgás frissítése (ez már a while (run) belsejében, de az események után van)
   const int shipSpeed = 5;
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
    bullet.update(16); // Frissítés minden frame-ben
}



        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    
        SDL_RenderClear(renderer);
    
         if (currentState == MENU) {
            // Menü renderelése ImGui-val
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            ImGui::Begin("Main Menu");

            // Az ablak szélessége
            ImVec2 windowSize = ImGui::GetWindowSize();
            float windowWidth = windowSize.x;

            // Gomb szélessége
            float buttonWidth = 225.0f;
            float buttonHeight = 45.0f;

            // Középre igazítás
            float centerX = (windowWidth - buttonWidth) / 2.0f;

            // New Game gomb
            ImGui::SetCursorPosX(centerX); // Vízszintes középre igazítás
            if (ImGui::ImageButton("NewGameButton", (ImTextureID)(intptr_t)newGameTexture, ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "New Menu clicked" << std::endl;
                currentState = MAP;
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            // Settings gomb
            ImGui::SetCursorPosX(centerX);
            if (ImGui::ImageButton("SettingsButton", (ImTextureID)(intptr_t)settingsTexture, ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Settings clicked" << std::endl;
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            // Credits gomb
            ImGui::SetCursorPosX(centerX);
            if (ImGui::ImageButton("CreditsButton", (ImTextureID)(intptr_t)creditsTexture, ImVec2(buttonWidth, buttonHeight))) {
                std::cout << "Credits clicked" << std::endl;
            }

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            // Exit gomb
            ImGui::SetCursorPosX(centerX);
            if (ImGui::ImageButton("ExitButton", (ImTextureID)(intptr_t)exitTexture, ImVec2(buttonWidth, buttonHeight))) {
                run = false;
            }

            ImGui::End();
        }
        
        else if (currentState == MAP) { // Kiemelt sor
            RenderGameScreen(renderer, displayMode.w, displayMode.h);
        } 

        else if (currentState >= ROOM1 && currentState <= ROOM7) {
            RenderRoom(renderer, "res/room1.png", shipTexture); 
                                                                                        //  + std::to_string(currentState - ROOM1 + 1) + ".png"

            // Lövedékek kirajzolása
            for (auto& bullet : playerBullets) {
                bullet.render(renderer);
            }
        }
                                                                                playerBullets.erase(std::remove_if(playerBullets.begin(), playerBullets.end(),
                                       [&](const Bullet& b) {
                                           return b.x < 0 || b.x > displayMode.w || b.y < 0 || b.y > displayMode.h;
                                       }),
                        playerBullets.end());


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