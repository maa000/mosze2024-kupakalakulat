#include <SDL.h>
#include <SDL_image.h>
#include <iostream>



// Négyzet sebessége
const float SQUARE_SPEED = 0.3f;

// Gombok mérete
const int BUTTON_WIDTH = 449;
const int BUTTON_HEIGHT = 206;

// Gombok pozíciója a menüben (középen)
SDL_Rect playButton = { 449, 206, BUTTON_WIDTH, BUTTON_HEIGHT };
SDL_Rect quitButton = { 449, 206, BUTTON_WIDTH, BUTTON_HEIGHT };

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

// Menü megjelenítése
bool showMenu(SDL_Renderer* renderer, SDL_Texture* playTexture, SDL_Texture* quitTexture)
{
    bool quitMenu = false;
    SDL_Event e;

    while (!quitMenu)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                return false;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // "Play" gomb
                if (mouseX > playButton.x && mouseX < playButton.x + playButton.w &&
                    mouseY > playButton.y && mouseY < playButton.y + playButton.h)
                {
                    return true; // Induljon a játék
                }

                // "Quit" gomb
                if (mouseX > quitButton.x && mouseX < quitButton.x + quitButton.w &&
                    mouseY > quitButton.y && mouseY < quitButton.y + quitButton.h)
                {
                    return false; // Kilépés a programból
                }
            }
        }

    // Menü megjelenítése
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // "Play" gomb képének kirajzolása
        SDL_RenderCopy(renderer, playTexture, nullptr, &playButton);

        // "Quit" gomb képének kirajzolása
        SDL_RenderCopy(renderer, quitTexture, nullptr, &quitButton);

        SDL_RenderPresent(renderer);
    }

    return false;
}

// Játék megjelenítése
void showGame(SDL_Renderer* renderer, SDL_DisplayMode displayMode)
{
    SDL_Texture* imageTexture = loadTexture("res/hajo1.png", renderer);
    if (imageTexture == nullptr)
    {
        std::cout << "Kép betöltési hiba!" << std::endl;
        return;
    }

    int imgWidth, imgHeight;
    SDL_QueryTexture(imageTexture, nullptr, nullptr, &imgWidth, &imgHeight);

    SDL_Rect destRect = { displayMode.w / 2 - imgWidth / 2, displayMode.h / 2 - imgHeight / 2, imgWidth, imgHeight };
    float velX = 0, velY = 0;

    bool quitGame = false;
    SDL_Event e;

    while (!quitGame)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quitGame = true;
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

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyTexture(imageTexture);
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

    // Ablak létrehozása
    SDL_Window* window = SDL_CreateWindow("Menu és Játék - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          displayMode.w, displayMode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

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
     // Textúrák betöltése a "Play" és "Quit" gombokhoz
    SDL_Texture* playTexture = loadTexture("res/play_button.png", renderer);
    SDL_Texture* quitTexture = loadTexture("res/quit_button.png", renderer);

    // Gombok pozicionálása a képernyő közepére
    playButton.x = (displayMode.w - BUTTON_WIDTH) / 2; // "Play" gomb X pozíciója
    playButton.y = (displayMode.h - BUTTON_HEIGHT) / 2 - 120; // "Play" gomb Y pozíciója

    quitButton.x = (displayMode.w - BUTTON_WIDTH) / 2; // "Quit" gomb X pozíciója
    quitButton.y = (displayMode.h - BUTTON_HEIGHT) / 2 + 120; // "Quit" gomb Y pozíciója

    // Menü megjelenítése
     bool playGame = showMenu(renderer, playTexture, quitTexture);

    // Ha a játék indult
    if (playGame)
    {
        showGame(renderer, displayMode);
    }

    // Takarítás
    SDL_DestroyTexture(playTexture);
    SDL_DestroyTexture(quitTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}