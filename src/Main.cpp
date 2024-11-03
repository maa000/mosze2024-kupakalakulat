#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stb/stb_image.h>
#include <iostream>
#include "myheaders/SaveManager.h"
#include "myheaders/Map.h"
#include "myheaders/Room.h"
#include "myheaders/Player.h"

// Textúrák globális változók
GLuint newGameTexture;
GLuint settingsTexture;
GLuint creditsTexture;
GLuint exitTexture;

//*************MENÜ**************//

// Textúrák betöltése függvény
GLuint LoadTextureFromFile(const char* filename) {
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 4);
    if (!image) {
        std::cerr << "Failed to load " << filename << ": " << stbi_failure_reason() << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Textúra paraméterek
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);

    return textureID;
}

// Textúrák inicializálása
void LoadTextures() {
    newGameTexture = LoadTextureFromFile("res/newgame.png");
    settingsTexture = LoadTextureFromFile("res/settings.png");
    creditsTexture = LoadTextureFromFile("res/credits.png");
    exitTexture = LoadTextureFromFile("res/exit.png");

    if (!newGameTexture || !settingsTexture || !creditsTexture || !exitTexture) {
        std::cerr << "One or more textures failed to load. Please check file paths and image formats." << std::endl;
    }
}
SaveManager saveManager("savefile.json");
Map currentMap;

int main(int argc, char* argv[]) {
    // SDL és OpenGL inicializálása
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    //*************FULLSCREEN ABLAK LÉTREHOZÁSA**************//
    // Teljes képernyős ablak létrehozása
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode); // Fő monitor mérete
    SDL_Window* window = SDL_CreateWindow("Fullscreen Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          displayMode.w, displayMode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Vsync engedélyezése

    // ImGui inicializálása
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Textúrák betöltése
    LoadTextures();


    //*************FŐ LOGIKA**************//
    bool run = true;
    while (run) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) run = false;
        }

        // Új frame indítása az ImGui-nál
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // Középre pozicionált menü ablak
        ImVec2 windowSize(300, 300);  // Menüablak mérete
        ImVec2 windowPos((displayMode.w - windowSize.x) / 2, (displayMode.h - windowSize.y) / 2);
        ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // New Game gomb
    if (ImGui::ImageButton("NewGameButton", (ImTextureID)(intptr_t)newGameTexture, ImVec2(250, 50))) {

        Player player;
         int roomIndex = 0;
       SaveManager saveManager("savefile.json");

        // Ha van mentés, betöltjük
    if (saveManager.loadGame(roomIndex, player)) {
        std::cout << "Game loaded from save." << std::endl;
    } else {
        // Ha nincs mentés, új játék inicializálása alapértelmezett értékekkel
        std::cout << "New game started." << std::endl;
        player = Player();  // Alapértelmezett élet és pénz
        saveManager.saveGame(roomIndex, player);
}}


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

        ImGui::End();

        // Renderelés
        ImGui::Render();
        glViewport(0, 0, displayMode.w, displayMode.h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Takarítás
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}