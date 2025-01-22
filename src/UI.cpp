#include "UI.h"
#include "Globals.h"
#include <string>

void renderHealthIndicator(SDL_Renderer* renderer) {
    if (!healthIconTexture || !healthFont) return;

    // Ikon kirajzolása
    SDL_Rect iconRect = {20, 600, 32, 32}; // Bal alsó sarok
    SDL_RenderCopy(renderer, healthIconTexture, nullptr, &iconRect);

    // Szám generálása
    std::string healthText = std::to_string(player.getHealth());
    SDL_Color textColor = {255, 255, 255, 255}; // Fehér szín
    SDL_Surface* textSurface = TTF_RenderText_Solid(healthFont, healthText.c_str(), textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {60, 600, textSurface->w, textSurface->h}; // Szám pozíciója az ikon mellett
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}