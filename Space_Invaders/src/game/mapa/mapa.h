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

typedef enum
{
    ENEMIGO_CALAMAR_VERDE,
    ENEMIGO_CANGREJO_VERDE,
    ENEMIGO_CANGREJO_TURQUESA,
    ENEMIGO_PULPO_TURQUESA,
    ENEMIGO_PULPO_MORADO
} tTipoEnemigo;

void mapa_enemigos_iniciar(void);
void mapa_enemigos_actualizar(void);
void mapa_escudos_iniciar(void);
void mapa_escudos_actualizar(void);
void mapa_escudos_dibujar(SDL_Renderer *render);
void mapa_enemigos_dibujar(SDL_Renderer *render);
bool mapa_sprites_cargar(SDL_Renderer *render);
int mapa_enemigos_hay_activos(void);
void mapa_sprites_liberar(void);
#endif // MAPA_H_INCLUDED
