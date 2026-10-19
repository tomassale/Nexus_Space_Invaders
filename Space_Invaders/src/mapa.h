#ifndef MAPA_H_INCLUDED
#define MAPA_H_INCLUDED

#include <stdbool.h>

typedef struct
{
    int x;
    int y;
    int activo;
    int tipo;
}tEnemigos;

typedef struct
{
    int x;
    int y;
    int activo;
}tEscudos;

#define ENEMIGO_CALAMAR_VERDE       0
#define ENEMIGO_CANGREJO_VERDE      1
#define ENEMIGO_CANGREJO_TURQUESA   2
#define ENEMIGO_PULPO_TURQUESA      3
#define ENEMIGO_PULPO_MORADO        4

void mapa_enemigos_iniciar(void);
void mapa_enemigos_mover(void);
void mapa_escudos_iniciar(void);
void mapa_escudos_dibujar(SDL_Renderer *render);
void mapa_enemigos_dibujar(SDL_Renderer *render);
bool mapa_sprites_cargar(SDL_Renderer *render);
int mapa_enemigos_hay_activos(void);
bool mapa_enemigo_disparador(int *xCentro, int *yBase);
void mapa_sprites_liberar(void);
#endif // MAPA_H_INCLUDED
