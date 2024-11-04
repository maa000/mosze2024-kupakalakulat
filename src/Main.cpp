#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>

// Négyzet sebessége
const float SQUARE_SPEED = 0.3f;
const float BULLET_SPEED = 0.5f;

// Gombok mérete
const int BUTTON_WIDTH = 449;
const int BUTTON_HEIGHT = 206;

// Gombok pozíciója a menüben (középen)
SDL_Rect playButton = { 449, 206, BUTTON_WIDTH, BUTTON_HEIGHT };
SDL_Rect quitButton = { 449, 206, BUTTON_WIDTH, BUTTON_HEIGHT };

// Lövedék osztály
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

// Textúra betöltése
SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cout << "Kép betöltési hiba! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == nullptr) {
            std::cout << "Textúra létrehozási hiba! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

// Menü megjelenítése
bool showMenu(SDL_Renderer* renderer, SDL_Texture* playTexture, SDL_Texture* quitTexture) {
    bool quitMenu = false;
    SDL_Event e;

    while (!quitMenu) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                return false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (mouseX > playButton.x && mouseX < playButton.x + playButton.w &&
                    mouseY > playButton.y && mouseY < playButton.y + playButton.h) {
                    return true; // Induljon a játék
                }
                if (mouseX > quitButton.x && mouseX < quitButton.x + quitButton.w &&
                    mouseY > quitButton.y && mouseY < quitButton.y + quitButton.h) {
                    return false; // Kilépés a programból
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, playTexture, nullptr, &playButton);
        SDL_RenderCopy(renderer, quitTexture, nullptr, &quitButton);

        SDL_RenderPresent(renderer);
    }
    return false;
}

// Játék megjelenítése
void showGame(SDL_Renderer* renderer, SDL_DisplayMode displayMode) {
    SDL_Texture* imageTexture = loadTexture("res/hajo1.png", renderer);
    SDL_Texture* bulletTexture = loadTexture("res/loves_te.png", renderer); // Töltsd be a lövedék sprite-ot
    if (imageTexture == nullptr || bulletTexture == nullptr) {
        std::cout << "Kép betöltési hiba!" << std::endl;
        return;
    }

    int imgWidth, imgHeight;
    SDL_QueryTexture(imageTexture, nullptr, nullptr, &imgWidth, &imgHeight);

    SDL_Rect destRect = { displayMode.w / 2 - imgWidth / 2, displayMode.h / 2 - imgHeight / 2, imgWidth, imgHeight };
    float velX = 0, velY = 0;
    bool quitGame = false;
    SDL_Event e;
    std::vector<Bullet> bullets;

    while (!quitGame) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quitGame = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                float dirX = mouseX - (destRect.x + imgWidth / 2);
                float dirY = mouseY - (destRect.y + imgHeight / 2);
                float length = sqrt(dirX * dirX + dirY * dirY);
                dirX /= length;
                dirY /= length;

                bullets.emplace_back(destRect.x + imgWidth / 2, destRect.y + imgHeight / 2,
                                     dirX * BULLET_SPEED, dirY * BULLET_SPEED, bulletTexture);
            }
        }

        const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

        if (currentKeyStates[SDL_SCANCODE_A]) velX = -SQUARE_SPEED;
        else if (currentKeyStates[SDL_SCANCODE_D]) velX = SQUARE_SPEED;
        else velX = 0;

        if (currentKeyStates[SDL_SCANCODE_W]) velY = -SQUARE_SPEED;
        else if (currentKeyStates[SDL_SCANCODE_S]) velY = SQUARE_SPEED;
        else velY = 0;

        destRect.x += static_cast<int>(velX * 10);
        destRect.y += static_cast<int>(velY * 10);

        if (destRect.x < 0) destRect.x = 0;
        if (destRect.x + destRect.w > displayMode.w) destRect.x = displayMode.w - destRect.w;
        if (destRect.y < 0) destRect.y = 0;
        if (destRect.y + destRect.h > displayMode.h) destRect.y = displayMode.h - destRect.h;

        for (auto& bullet : bullets) bullet.update(16);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, imageTexture, nullptr, &destRect);
        for (auto& bullet : bullets) bullet.render(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(imageTexture);
    SDL_DestroyTexture(bulletTexture);
}

// Fő program
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL nem tudott inicializálni! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image nem tudott inicializálni! SDL_image Error: " << IMG_GetError() << std::endl;
        return -1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
        std::cout << "Nem sikerült lekérdezni a kijelző módot! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Menu és Játék - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          displayMode.w, displayMode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);

    if (window == nullptr) {
        std::cout << "Ablak nem jött létre! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cout << "Renderer nem jött létre! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Textúrák betöltése a "Play" és "Quit" gombokhoz
    SDL_Texture* playTexture = loadTexture("res/play_button.png", renderer);
    SDL_Texture* quitTexture = loadTexture("res/quit_button.png", renderer);

    playButton.x = (displayMode.w - BUTTON_WIDTH) / 2;
    playButton.y = (displayMode.h - BUTTON_HEIGHT) / 2 - 120;

    quitButton.x = (displayMode.w - BUTTON_WIDTH) / 2;
    quitButton.y = (displayMode.h - BUTTON_HEIGHT) / 2 + 120;

    bool playGame = showMenu(renderer, playTexture, quitTexture);

    if (playGame) {
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