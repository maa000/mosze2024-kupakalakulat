#ifndef UI_H
#define UI_H

#include <SDL.h>

void renderHealthIndicator(SDL_Renderer* renderer);
void renderNumber(SDL_Renderer* renderer, int number, int x, int y);

#endif // UI_H
