#ifndef MAPA_H_INCLUDED
#define MAPA_H_INCLUDED

#include <stdbool.h>

typedef struct
{
    int x;
    int y;
    int activo;
}tEscudos;

void mapa_escudos_iniciar(void);
void mapa_escudos_dibujar(SDL_Renderer *render);
bool mapa_sprites_cargar(SDL_Renderer *render);
void mapa_sprites_liberar(void);
#endif // MAPA_H_INCLUDED
