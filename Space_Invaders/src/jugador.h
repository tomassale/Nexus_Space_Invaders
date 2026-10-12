#ifndef JUGADOR_H
#define JUGADOR_H

typedef struct
{
    int x;
    int y;
    int vidas;
    int puntaje;
} tJugador;

void jugador_iniciar(tJugador *jugador);
void jugador_mover(tJugador *jugador, int direccion);

#endif
