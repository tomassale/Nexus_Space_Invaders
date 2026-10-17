#include "jugador.h"

#define IZQUIERDA -1
#define DERECHA 1

#define VELOCIDAD_JUGADOR 3

/* Valores provisionales */
#define VENTANA_X 256
#define PLAYER_START_X ((VENTANA_X)/2) - 100
#define PLAYER_START_Y 220

void jugador_iniciar(tJugador *jugador)
{
    jugador->x = PLAYER_START_X;
    jugador->y = PLAYER_START_Y;
    jugador->vidas = 3;
    jugador->puntaje = 0;
}

bool jugador_cargar(tJugador *jugador, SDL_Renderer *render)
{
    return spriteCargar(&jugador->sprite, render, "assets/images/jugador.png", 1, 0);
}

void jugador_mover(tJugador *jugador, int direccion)
{
    int resultante;

    if(direccion == IZQUIERDA || direccion == DERECHA)
    {
        resultante = jugador->x + direccion * VELOCIDAD_JUGADOR;

        if(resultante >= 0 && resultante <= VENTANA_X - JUGADOR_ANCHO)
        {
            jugador->x = resultante;
        }
    }
}

void jugador_dibujar(const tJugador *jugador, SDL_Renderer *render)
{
    spriteDibujar(render, &jugador->sprite, jugador->x, jugador->y, JUGADOR_ANCHO, JUGADOR_ALTO);
}

void jugador_destruir(tJugador *jugador)
{
    spriteLiberar(&jugador->sprite);
}
