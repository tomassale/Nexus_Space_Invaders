#include <SDL.h>
#include "game.h"
#include "mapa.h"
#include "jugador.h"
#include "recursos.h"
#define IZQUIERDA -1
#define DERECHA 1

//ejemplo ventana 800x600
//#define PLAYER_START_X 400 //arranca al costado (cambiar)
//#define PLAYER_START_Y 550 //constante en y

void jugador_iniciar(tJugador *jugador)
{
    jugador->x = PLAYER_START_X;
    jugador->y = PLAYER_START_Y;
    jugador->vidas = 3;
    jugador->puntaje = 0;
}

void jugador_mover(tJugador *jugador, int direccion)
{
    if(direccion == IZQUIERDA)
        jugador->x--;
    if(direccion == DERECHA)
        jugador->x++;
}

void jugador_disparar(tJugador *jugador)
{
    if(!disparo->activo)
    {
        disparo->x = jugador->x;
        disparo->y = jugador->y;
        disparo->activo = 1;
    }
}




