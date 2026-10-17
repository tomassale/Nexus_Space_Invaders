#include <SDL_image.h>
#include "sprites.h"

bool spriteCargar(tSprite *s, SDL_Renderer *r, const char *ruta,
                  int frames, float duracionFrame)
{
    int w, h;

    if(frames < 1)
        frames = 1;

    s->textura = IMG_LoadTexture(r, ruta);
    if(s->textura == NULL)
    {
        SDL_Log("No se pudo cargar %s: %s", ruta, IMG_GetError());
        return false;
    }

    SDL_QueryTexture(s->textura, NULL, NULL, &w, &h);

    s->ancho = w / frames;
    s->alto = h;
    s->frames = frames;
    s->frameActual = 0;
    s->tiempo = 0.0f;
    s->duracionFrame = duracionFrame;

    return true;
}

void spriteActualizar(tSprite *s, float delta)
{
    if(s->frames <= 1 || s->duracionFrame <= 0.0f)
        return;

    s->tiempo += delta;

    while(s->tiempo >= s->duracionFrame)
    {
        s->tiempo -= s->duracionFrame;
        s->frameActual = (s->frameActual + 1) % s->frames;
    }
}

void spriteDibujar(SDL_Renderer *r, const tSprite *s, int x, int y, int w, int h)
{
    SDL_Rect origen = { s->frameActual * s->ancho, 0, s->ancho, s->alto };
    SDL_Rect destino = { x, y, w, h };

    SDL_RenderCopy(r, s->textura, &origen, &destino);
}

void spriteLiberar(tSprite *s)
{
    if(s->textura)
    {
        SDL_DestroyTexture(s->textura);
        s->textura = NULL;
    }
}
