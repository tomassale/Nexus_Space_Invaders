#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>

typedef struct
{
    SDL_Texture *textura;
    int ancho;
    int alto;
    int frames;
    int frameActual;
    float tiempo;
    float duracionFrame;
}tSprite;

bool spriteCargar(tSprite*, SDL_Renderer*, const char*,
                  int, float);
void spriteActualizar(tSprite*, float);
void spriteDibujar(SDL_Renderer*, const tSprite*, int, int, int, int);
void spriteLiberar(tSprite*);

#endif // SPRITES_H_INCLUDED
