#ifndef JUGADOR_H
#define JUGADOR_H

#include <stdbool.h>
#include <SDL.h>
#include "../../engine/sprites.h"

#define JUGADOR_ANCHO 20
#define JUGADOR_ALTO  10

typedef struct
{
    int x;
    int y;
    int vidas;
    int puntaje;
    tSprite sprite;
} tJugador;

void jugador_iniciar(tJugador *jugador);
bool jugador_cargar(tJugador *jugador, SDL_Renderer *render);
void jugador_mover(tJugador *jugador, int direccion);
void jugador_dibujar(const tJugador *jugador, SDL_Renderer *render);
void jugador_destruir(tJugador *jugador);

#endif
