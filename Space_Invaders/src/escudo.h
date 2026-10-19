#ifndef ESCUDO_H_INCLUDED
#define ESCUDO_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>

/* Escudos: cada escudo esta armado con partes chicas que se destruyen de a una */
typedef struct
{
    int x;
    int y;
    int activo;
}tEscudo;

typedef struct
{
    int x;
    int y;
    int activo;
}tParteEscudo;

#define CANT_ESCUDOS 4
#define ESCUDOS_ANCHO 35
#define ESCUDOS_ALTO 35

#define PARTE_ESCUDO_ANCHO 5
#define PARTE_ESCUDO_ALTO 5

#define COLUMNAS_ESCUDO (ESCUDOS_ANCHO / PARTE_ESCUDO_ANCHO)
#define FILAS_ESCUDO (ESCUDOS_ALTO / PARTE_ESCUDO_ALTO)

#define PARTES_POR_ESCUDO (COLUMNAS_ESCUDO * FILAS_ESCUDO)
#define PARTES_ESCUDOS_TOTAL (CANT_ESCUDOS * PARTES_POR_ESCUDO)

/* Sprites */
bool escudo_cargar(SDL_Renderer *render);
void escudo_liberar(void);

/* Formacion */
int  escudo_iniciar(void);
void escudo_dibujar(SDL_Renderer *render);
void escudo_destruir(void);

/* Danio: el juego decide quien golpea y el escudo resuelve que partes se rompen.
   - Una bala rompe UNA parte (la primera que toca).
   - Un enemigo (rectangulo mas grande) rompe TODAS las que toca. */
int  escudo_impactado(int x, int y, int ancho, int alto);
void escudo_romper_rect(int x, int y, int ancho, int alto);

#endif // ESCUDO_H_INCLUDED
