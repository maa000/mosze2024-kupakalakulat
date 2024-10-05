#include <SDL.h>
#include <SDL_image.h>
#include <iostream>



// Négyzet sebessége
const float SQUARE_SPEED = 0.3f;

// Textúra betöltése
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer)
{
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr)
    {
        std::cout << "Kép betöltési hiba! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    else
    {

        // Textúra létrehozása a betöltött surface-ből
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr)
        {
            std::cout << "Textúra létrehozási hiba! SDL Error: " << SDL_GetError() << std::endl;
        }

        // Felületek felszabadítása
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

// Fő program
int main(int argc, char* args[])
{
    // SDL inicializálása
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL nem tudott inicializálni! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // SDL_image inicializálása
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cout << "SDL_image nem tudott inicializálni! SDL_image Error: " << IMG_GetError() << std::endl;
        return -1;
    }

    // Képernyőméret
 SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0)
    {
        std::cout << "Nem sikerült lekérdezni a kijelző módot! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    int screenWidth = displayMode.w;
    int screenHeight = displayMode.h;

    // Ablak létrehozása keret nélküli teljes képernyős módban
    SDL_Window* window = SDL_CreateWindow("Kép mozgatása - SDL2",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

    if (window == nullptr)
    {
        std::cout << "Ablak nem jött létre! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Renderer létrehozása
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cout << "Renderer nem jött létre! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Kép betöltése és textúra létrehozása
    SDL_Texture* imageTexture = loadTexture("res/hajo1.png", renderer);
    if (imageTexture == nullptr)
    {
        std::cout << "Kép betöltési hiba!" << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Textúra méretének lekérdezése
    int imgWidth, imgHeight;
    SDL_QueryTexture(imageTexture, nullptr, nullptr, &imgWidth, &imgHeight);

    // Kép kezdőpozíciója
    SDL_Rect destRect = {displayMode.w / 2 - imgWidth / 2, displayMode.h/ 2 - imgHeight / 2, imgWidth, imgHeight};
    float velX = 0, velY = 0;

    // Program futtatása
    bool quit = false;
    SDL_Event e;

    while (!quit)
    {
        // Események kezelése
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        // Billentyűk állapota
        const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

        // Horizontális mozgás
        if (currentKeyStates[SDL_SCANCODE_A])
        {
            velX = -SQUARE_SPEED; // Balra mozgás
        }
        else if (currentKeyStates[SDL_SCANCODE_D])
        {
            velX = SQUARE_SPEED;  // Jobbra mozgás
        }
        else
        {
            velX = 0; // Nincs vízszintes mozgás
        }

        // Vertikális mozgás
        if (currentKeyStates[SDL_SCANCODE_W])
        {
            velY = -SQUARE_SPEED; // Felfelé mozgás
        }
        else if (currentKeyStates[SDL_SCANCODE_S])
        {
            velY = SQUARE_SPEED;  // Lefelé mozgás
        }
        else
        {
            velY = 0; // Nincs függőleges mozgás
        }

        // Kép pozíciójának frissítése
        destRect.x += static_cast<int>(velX * 10);
        destRect.y += static_cast<int>(velY * 10);

        // Kép pozíciójának korlátozása a képernyőn belül
        if (destRect.x < 0) destRect.x = 0;
        if (destRect.x + destRect.w > displayMode.w) destRect.x = displayMode.w - destRect.w;
        if (destRect.y < 0) destRect.y = 0;
        if (destRect.y + destRect.h > displayMode.h) destRect.y = displayMode.h - destRect.h;

        // Háttér (fekete) rajzolása
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Kép (textúra) rajzolása
        SDL_RenderCopy(renderer, imageTexture, nullptr, &destRect);

        // Képernyő frissítése
        SDL_RenderPresent(renderer);

        // Kis késleltetés a simább mozgás érdekében
        SDL_Delay(16);
    }

    // Takarítás
    SDL_DestroyTexture(imageTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
