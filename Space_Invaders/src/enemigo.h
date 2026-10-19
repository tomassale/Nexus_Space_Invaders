#ifndef ENEMIGO_H_INCLUDED
#define ENEMIGO_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>

/* Tamano en pantalla de cada enemigo */
#define ENEMIGO_ANCHO 12
#define ENEMIGO_ALTO  8

/* Tipo de enemigo: define que sprite usa y cuanto puntaje otorga */
#define ENEMIGO_CALAMAR_VERDE       0
#define ENEMIGO_CANGREJO_VERDE      1
#define ENEMIGO_CANGREJO_TURQUESA   2
#define ENEMIGO_PULPO_TURQUESA      3
#define ENEMIGO_PULPO_MORADO        4

/* Un enemigo de la formacion.
   La coleccion completa se guarda en un Vector privado del modulo, asi que
   desde afuera solo se pide "un enemigo" cuando hace falta. */
typedef struct
{
    int x;
    int y;
    int activo;
    int tipo;
}tEnemigo;

/* Ciclo de vida */
bool enemigo_cargar(SDL_Renderer *render);   /* carga los sprites */
int  enemigo_iniciar(void);                  /* crea la coleccion y arma la formacion */
void enemigo_destruir(void);                 /* libera la coleccion */
void enemigo_liberar(void);                  /* libera los sprites */

/* Operaciones sobre la coleccion */
void enemigo_mover(void);
void enemigo_dibujar(SDL_Renderer *render);
int  enemigo_hay_activos(void);

/* Consultas que usa la logica del juego */
int  enemigo_obtener(tEnemigo *enemigo);                  /* elige uno para disparar */
int  enemigo_impactado(int x, int y, int *tipoEliminado); /* colision con la bala del jugador */
int  enemigo_toca_jugador(int x, int y);                  /* colision con el jugador */

#endif // ENEMIGO_H_INCLUDED
