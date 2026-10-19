#ifndef MAPA_H_INCLUDED
#define MAPA_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>

/* Escudos: cada escudo esta armado con partes chicas que se destruyen de a una */
typedef struct
{
    int x;
    int y;
    int activo;
}tEscudos;

typedef struct
{
    int x;
    int y;
    int activo;
} tParteEscudos;

#define CANT_ESCUDOS 4
#define ESCUDOS_ANCHO 35
#define ESCUDOS_ALTO 35

#define PARTE_ESCUDO_ANCHO 5
#define PARTE_ESCUDO_ALTO 5

#define COLUMNAS_ESCUDO (ESCUDOS_ANCHO / PARTE_ESCUDO_ANCHO)
#define FILAS_ESCUDO (ESCUDOS_ALTO / PARTE_ESCUDO_ALTO)

#define PARTES_POR_ESCUDO (COLUMNAS_ESCUDO * FILAS_ESCUDO)
#define PARTES_ESCUDOS_TOTAL (CANT_ESCUDOS * PARTES_POR_ESCUDO)

int mapa_escudos_iniciar(void);
int mapa_escudo_impactado(int x, int y);
void mapa_destruir_parte_escudo(unsigned indiceParte);
void mapa_actualizar_estado_escudo(unsigned indiceEscudo);
void mapa_escudos_dibujar(SDL_Renderer *render);
void mapa_escudos_destruir(void);

/* Sprites del mapa (hoy: solo el escudo) */
bool mapa_sprites_cargar(SDL_Renderer *render);
void mapa_sprites_liberar(void);

#endif // MAPA_H_INCLUDED
