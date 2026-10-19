#ifndef ENEMIGO_H_INCLUDED
#define ENEMIGO_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>

typedef struct
{
    int x;
    int y;
    int activo;
    int tipo;
}tEnemigo;

#define ENEMIGO_CALAMAR_VERDE       0
#define ENEMIGO_CANGREJO_VERDE      1
#define ENEMIGO_CANGREJO_TURQUESA   2
#define ENEMIGO_PULPO_TURQUESA      3
#define ENEMIGO_PULPO_MORADO        4

bool enemigo_cargar(SDL_Renderer *render);
void enemigo_iniciar(void);
void enemigo_mover(void);
void enemigo_dibujar(SDL_Renderer *render);
int enemigo_hay_activos(void);
bool enemigo_disparador(int *xCentro, int *yBase);
void enemigo_liberar(void);

#endif // ENEMIGO_H_INCLUDED
