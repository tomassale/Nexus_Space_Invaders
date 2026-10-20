#ifndef RECURSOS_H_INCLUDED
#define RECURSOS_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>
#include "sprites.h"
#include "audio.h"
#include "fonts.h"

/* Cantidad de sonidos de la marcha de la formacion (uno por paso) */
#define CANT_PASOS 4

/* Carga y liberacion de todas las imagenes, sonidos y fuentes del juego.
   Las rutas y los recursos viven en recursos.c: los modulos solo piden lo que usan. */
bool recursos_cargar(SDL_Renderer *render);
void recursos_liberar(void);

/* Sprites del juego */
const tSprite *recursos_fondo(void);
const tSprite *recursos_jugador(void);
const tSprite *recursos_escudo(void);

/* Cada enemigo tiene su sprite normal y su sprite animado */
const tSprite *recursos_calamar(int frame);
const tSprite *recursos_cangrejo(int frame);
const tSprite *recursos_pulpo(int frame);
const tSprite *recursos_enemigo_eliminado(void);

/* Sonidos del juego */
tSonido *recursos_disparo_jugador(void);
tSonido *recursos_disparo_enemigo(void);
tSonido *recursos_final_nivel(void);
tSonido *recursos_nave_bajando(int paso);

/* Fuente del puntaje y de los textos en pantalla */
tFuente *recursos_fuente(void);

#endif // RECURSOS_H_INCLUDED
