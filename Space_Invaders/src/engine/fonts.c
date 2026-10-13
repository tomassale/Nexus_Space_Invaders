#include "fonts.h"

bool fuentesCrear(void)
{
    if(TTF_Init() < 0)
    {
        SDL_Log("TTF_Init: %s", TTF_GetError());
        return false;
    }
    return true;
}

void fuentesDestruir(void)
{
    TTF_Quit();
}

tFuente* fuenteCargar(const char *ruta, int tamanio)
{
    tFuente *f = TTF_OpenFont(ruta, tamanio);

    if(f == NULL)
        SDL_Log("No se pudo cargar %s: %s", ruta, TTF_GetError());

    return f;
}

void fuenteLiberar(tFuente *f)
{
    if(f)
        TTF_CloseFont(f);
}

void fuenteDibujar(SDL_Renderer *r, tFuente *f, const char *texto,
                   int x, int y, SDL_Color color)
{
    SDL_Surface *surf;
    SDL_Texture *tex;
    SDL_Rect destino;

    if(f == NULL || texto == NULL || texto[0] == '\0')
        return;

    surf = TTF_RenderUTF8_Solid(f, texto, color);
    if(surf == NULL)
        return;

    tex = SDL_CreateTextureFromSurface(r, surf);
    if(tex)
    {
        destino.x = x;
        destino.y = y;
        destino.w = surf->w;
        destino.h = surf->h;
        SDL_RenderCopy(r, tex, NULL, &destino);
        SDL_DestroyTexture(tex);
    }

    SDL_FreeSurface(surf);
}

int fuenteAncho(tFuente *f, const char *texto)
{
    int w = 0;

    if(f && texto)
        TTF_SizeUTF8(f, texto, &w, NULL);

    return w;
}
