#ifndef BALA_H_INCLUDED
#define BALA_H_INCLUDED

#include <SDL.h>

#define BALA_ANCHO 4
#define BALA_ALTO 12
#define BALA_VEL_JUGADOR  7
#define BALA_VEL_ENEMIGO  4

typedef struct
{
    int x;
    int y;
} Bala;

void bala_crear(Bala *bala, int x, int y);
int bala_mover_jugador(void *elem, void *ctx);
int bala_mover_enemigo(void *elem, void *ctx);
int bala_en_pantalla(const void *elem, void *ctx);
int bala_dibujar(void *elem, void *ctx);

#endif // BALA_H_INCLUDED