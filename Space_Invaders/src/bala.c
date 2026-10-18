#include "bala.h"
#include "motor.h"

void bala_crear(Bala *bala, int x, int y)
{
    bala->x = x;
    bala->y = y;
}

int bala_mover_jugador(void *elem, void *ctx)
{
    Bala *bala = (Bala *)elem;

    bala->y -= BALA_VEL_JUGADOR;

    return 1;
}

int bala_mover_enemigo(void *elem, void *ctx)
{
    Bala *bala = (Bala *)elem;
    bala->y += BALA_VEL_ENEMIGO;

    return 1;
}

int bala_en_pantalla(const void *elem, void *contexto)
{
    const Bala *bala = (const Bala *)elem;

    if(bala->y + BALA_ALTO <= 0)
        return 0;

    if(bala->y >= motorAlto())
        return 0;

    return 1;
}

int bala_dibujar(void *elem, void *ctx)
{
    const Bala *bala = (const Bala*)elem;
    SDL_Renderer *renderizador = (SDL_Renderer*)ctx;
    if(!bala || !renderizador)
        return 0;

    SDL_Rect rect = {bala->x, bala->y, BALA_ANCHO, BALA_ALTO};

    SDL_SetRenderDrawColor(renderizador, 255, 255, 0, 255);
    SDL_RenderFillRect(renderizador, &rect);

    return 1;
}