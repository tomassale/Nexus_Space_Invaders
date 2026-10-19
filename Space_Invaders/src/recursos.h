#ifndef RECURSOS_H_INCLUDED
#define RECURSOS_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>
#include "sprites.h"

/* Carga y liberacion de todas las imagenes del juego.
   Las rutas y las texturas viven en recursos.c: los modulos solo piden el sprite. */
bool recursos_cargar(SDL_Renderer *render);
void recursos_liberar(void);

/* Sprites del juego */
const tSprite *recursos_fondo(void);
const tSprite *recursos_jugador(void);
const tSprite *recursos_menu(void);
const tSprite *recursos_escudo(void);

/* Cada enemigo tiene su sprite normal y su sprite animado */
const tSprite *recursos_calamar(int frame);
const tSprite *recursos_cangrejo(int frame);
const tSprite *recursos_pulpo(int frame);
const tSprite *recursos_enemigo_eliminado(void);

#endif // RECURSOS_H_INCLUDED
