#ifndef JUGADOR_H
#define JUGADOR_H

#include "motor.h"

typedef struct
{
    int x;
    int y;
    int vidas;
    int puntaje;
} tJugador;

#define PLAYER_START_X ((VENTANA_X)/2) - 100
#define PLAYER_START_Y 220

#define ANCHO_JUGADOR 20
#define ALTO_JUGADOR 10

void jugador_iniciar(tJugador *jugador);
void jugador_mover(tJugador *jugador, int direccion);

#endif
