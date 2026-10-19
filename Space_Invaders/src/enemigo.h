#ifndef ENEMIGO_H_INCLUDED
#define ENEMIGO_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>
#include "vector.h"

// Tamano en pantalla de cada enemigo
#define ENEMIGO_ANCHO 12
#define ENEMIGO_ALTO  8
#define ENEMIGO_VELOCIDAD 1
#define ENEMIGO_DESCENSO 16

// Tipo de enemigo: define que sprite usa y cuanto puntaje otorga
#define ENEMIGO_CALAMAR_VERDE       0
#define ENEMIGO_CANGREJO_VERDE      1
#define ENEMIGO_CANGREJO_TURQUESA   2
#define ENEMIGO_PULPO_TURQUESA      3
#define ENEMIGO_PULPO_MORADO        4

#define ESCUDOS_INICIO_Y 170
#define ALTURA_ROJA (ESCUDOS_INICIO_Y - ENEMIGO_ALTO)

// Estructura de UN enemigo de la formacion
typedef struct
{
    int x;
    int y;
    int activo;
    int tipo;
}tEnemigo;

/* La coleccion de enemigos (un Vector) la declara y la guarda el juego.
   El modulo solo sabe como crearla y como trabajar con ella. */

/* Sprites */
bool enemigo_cargar(SDL_Renderer *render);
void enemigo_liberar(void);

/* Formacion: crea la coleccion y le carga los enemigos */
int  enemigo_iniciar(Vector *vEnemigos);

/* Formacion: mover y dibujar todos juntos */
void enemigo_mover(Vector *vEnemigos);
float enemigo_intervalo_movimiento(Vector *vEnemigos); /* segundos entre pasos */
void enemigo_dibujar(Vector *vEnemigos, SDL_Renderer *render);
int  enemigo_hay_activos(Vector *vEnemigos);

/* Consultas que usa la logica del juego */
int  enemigo_obtener(Vector *vEnemigos, tEnemigo *enemigo); /* elige uno para disparar */
void enemigo_marcar_eliminado(int x, int y);                /* feedback de muerte (250 ms) */

#endif // ENEMIGO_H_INCLUDED
