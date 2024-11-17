#define SDL_MAIN_HANDLED
#include <SDL.h>
//#include <SDL_opengl.h>
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stb/stb_image.h>
#include <iostream>
#include <vector>
#include <cstdlib> // Véletlen számokhoz
#include <ctime>
#include "myheaders/SaveManager.h"
#include "myheaders/Map.h"
#include "myheaders/Room.h"
#include "myheaders/Player.h"
#include <SDL_image.h>

// Globális textúra változók
SDL_Texture* newGameTexture;
SDL_Texture* settingsTexture;
SDL_Texture* creditsTexture;
SDL_Texture* exitTexture;
SDL_Texture* gameBackgroundTexture;

// Gomb textúrák
SDL_Texture* button1Texture;//room0
SDL_Texture* button2Texture;//room1
SDL_Texture* button3Texture;//stb
SDL_Texture* button4Texture;
SDL_Texture* button5Texture;
SDL_Texture* button6Texture;
SDL_Texture* button7Texture;

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
    gameBackgroundTexture = loadTexture("res/game_background.png", renderer);
    button1Texture = loadTexture("res/button1.png", renderer);
    button2Texture = loadTexture("res/button2.png", renderer);
    button3Texture = loadTexture("res/button3.png", renderer);
    button4Texture = loadTexture("res/button4.png", renderer);
    button5Texture = loadTexture("res/button1.png", renderer);
    button6Texture = loadTexture("res/button2.png", renderer);
    button7Texture = loadTexture("res/button3.png", renderer);

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
void RenderGameScreen(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    SDL_RenderCopy(renderer, gameBackgroundTexture, nullptr, nullptr); // Háttér kirajzolása

    int buttonWidth = 100;
    int buttonHeight = 50;
    auto distances = GenerateRandomDistances(screenWidth, 7, buttonWidth);

    int xPos = 0;
    for (int i = 0; i < 7; ++i) {
        xPos += distances[i];
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
        ImGui::SetCursorPos(ImVec2(xPos, screenHeight / 2 - buttonHeight / 2));
        if (ImGui::ImageButton(("Button" + std::to_string(i + 1)).c_str(), (ImTextureID)(intptr_t)buttonTexture, ImVec2(buttonWidth, buttonHeight))) {
            std::cout << "Button " << i + 1 << " clicked!" << std::endl;
        }
        xPos += buttonWidth;
    }
}

int main(int argc, char* argv[]) {
    
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


    // Háttér textúra betöltése
    SDL_Texture* backgroundTexture = loadTexture("res/background.png", renderer);
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

    while (run) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) run = false;
        }

        // Új frame indítása az ImGui-nál
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
         // Háttér kirajzolása
        SDL_RenderClear(renderer);
        //SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
     
     if (inMenu) {
               SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            ImGui::Begin("Main Menu");
            if (ImGui::ImageButton("NewGameButton", (ImTextureID)(intptr_t)newGameTexture, ImVec2(250, 50))) {
                inMenu = false; // Játék képernyőre váltás
            }
        ImGui::Dummy(ImVec2(0.0f, 10.0f));  // Kis távolság a gombok között

        // Settings gomb
        if (ImGui::ImageButton("SettingsButton", (ImTextureID)(intptr_t)settingsTexture, ImVec2(250, 50))) {
            std::cout << "Settings clicked" << std::endl;
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Credits gomb
        if (ImGui::ImageButton("CreditsButton", (ImTextureID)(intptr_t)creditsTexture, ImVec2(250, 50))) {
            std::cout << "Credits clicked" << std::endl;
        }

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // Exit gomb
        if (ImGui::ImageButton("ExitButton", (ImTextureID)(intptr_t)exitTexture, ImVec2(250, 50))) {
            run = false;  // Kilépés a programból
        }
   // Helyes lezárás
    ImGui::End();
    } else {
            // Játék képernyő renderelése
            RenderGameScreen(renderer, displayMode.w, displayMode.h);
        }
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),renderer);

        // SDL ablak frissítése
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}