#include <SDL.h>
#include <stdbool.h>

#include "escudo.h"
#include "colision.h"
#include "recursos.h"
#include "vector.h"

/* Posicion y separacion de los escudos en pantalla */
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

int escudo_iniciar(Vector *vEscudos, Vector *vEscudosPartes)
{
    int i, fila, columna;
    tEscudo escudo;
    tParteEscudo parte;

    if(!vectorCrear(vEscudos, CANT_ESCUDOS, sizeof(tEscudo)))
        return 0;

    if(!vectorCrear(vEscudosPartes, PARTES_ESCUDOS_TOTAL, sizeof(tParteEscudo)))
    {
        vectorDestruir(vEscudos);
        return 0;
    }

    for(i = 0; i < CANT_ESCUDOS; i++)
    {
        escudo.x = ESCUDOS_INICIO_X + i * ESCUDOS_SEPARACION_X;
        escudo.y = ESCUDOS_INICIO_Y;
        escudo.activo = 1;

        if(vectorInsertarAlFinal(vEscudos, &escudo) != TODO_OK)
        {
            escudo_destruir(vEscudos, vEscudosPartes);
            return 0;
        }

        /* Cada escudo se divide en una grilla de partes chicas */
        for(fila = 0; fila < FILAS_ESCUDO; fila++)
        {
            for(columna = 0; columna < COLUMNAS_ESCUDO; columna++)
            {
                parte.x = escudo.x + columna * PARTE_ESCUDO_ANCHO;
                parte.y = escudo.y + fila * PARTE_ESCUDO_ALTO;
                parte.activo = 1;

                if(vectorInsertarAlFinal(vEscudosPartes, &parte) != TODO_OK)
                {
                    escudo_destruir(vEscudos, vEscudosPartes);
                    return 0;
                }
            }
        }
    }

    return 1;
}

/* Apaga UNA parte del escudo */
static void destruirParteEscudo(Vector *vEscudosPartes, unsigned indiceParte)
{
    tParteEscudo parte;

    if(vectorGetByPos(vEscudosPartes, indiceParte, &parte))
    {
        parte.activo = 0;

        vectorUpdateByPos(vEscudosPartes, indiceParte, &parte);
    }
}

/* Apaga el escudo entero recien cuando no le queda ninguna parte viva */
static void actualizarEstadoEscudo(Vector *vEscudos, Vector *vEscudosPartes,
                                   unsigned indiceEscudo)
{
    unsigned i = indiceEscudo * PARTES_POR_ESCUDO;
    unsigned fin = i + PARTES_POR_ESCUDO;
    int hayPartesActivas = 0;
    tParteEscudo parte;
    tEscudo escudo;

    while(i < fin && !hayPartesActivas)
    {
        if(vectorGetByPos(vEscudosPartes, i, &parte) &&
           parte.activo)
            hayPartesActivas = 1;

        i++;
    }

    if(!hayPartesActivas &&
       vectorGetByPos(vEscudos, indiceEscudo, &escudo))
    {
        escudo.activo = 0;

        vectorUpdateByPos(vEscudos, indiceEscudo, &escudo);
    }
}

/* Indice de la primera parte viva que choca con el rectangulo, o -1 */
static int buscarParteGolpeada(Vector *vEscudosPartes,
                               int x, int y, int ancho, int alto)
{
    unsigned i;
    unsigned cantidad = (unsigned)VectorCE(vEscudosPartes);
    tParteEscudo parte;

    for(i = 0; i < cantidad; i++)
    {
        if(vectorGetByPos(vEscudosPartes, i, &parte) &&
           parte.activo &&
           hay_choque(x, y, ancho, alto,
                      parte.x, parte.y, PARTE_ESCUDO_ANCHO, PARTE_ESCUDO_ALTO))
            return (int)i;
    }

    return -1;
}

/* Una bala: rompe solo la primera parte que toca */
int escudo_impactado(Vector *vEscudos, Vector *vEscudosPartes,
                     int x, int y, int ancho, int alto)
{
    int indice = buscarParteGolpeada(vEscudosPartes, x, y, ancho, alto);

    if(indice < 0)
        return 0;

    destruirParteEscudo(vEscudosPartes, (unsigned)indice);

    /* A que escudo pertenece la parte golpeada */
    actualizarEstadoEscudo(vEscudos, vEscudosPartes,
                           (unsigned)indice / PARTES_POR_ESCUDO);

    return 1;
}

/* Un enemigo: rompe todas las partes que toca de una sola vez */
void escudo_romper_rect(Vector *vEscudos, Vector *vEscudosPartes,
                        int x, int y, int ancho, int alto)
{
    unsigned i;
    unsigned cantidad = (unsigned)VectorCE(vEscudosPartes);
    tParteEscudo parte;

    for(i = 0; i < cantidad; i++)
    {
        if(vectorGetByPos(vEscudosPartes, i, &parte) &&
           parte.activo &&
           hay_choque(x, y, ancho, alto,
                      parte.x, parte.y, PARTE_ESCUDO_ANCHO, PARTE_ESCUDO_ALTO))
        {
            destruirParteEscudo(vEscudosPartes, i);
            actualizarEstadoEscudo(vEscudos, vEscudosPartes,
                                   i / PARTES_POR_ESCUDO);
        }
    }
}

/* Dibuja solo las partes vivas, recortando cada una del sprite completo */
void escudo_dibujar(Vector *vEscudos, Vector *vEscudosPartes, SDL_Renderer *render)
{
    unsigned i = 0;
    unsigned cantidad = (unsigned)VectorCE(vEscudosPartes);
    unsigned indiceEscudo;
    unsigned indiceLocal;
    unsigned fila;
    unsigned columna;

    tParteEscudo parte;
    tEscudo escudo;

    SDL_Rect origen;
    SDL_Rect destino;

    while(i < cantidad)
    {
        if(vectorGetByPos(vEscudosPartes, i, &parte) &&
           parte.activo)
        {
            indiceEscudo = i / PARTES_POR_ESCUDO;
            indiceLocal = i % PARTES_POR_ESCUDO;

            if(vectorGetByPos(vEscudos, indiceEscudo, &escudo) &&
               escudo.activo)
            {
                fila = indiceLocal / COLUMNAS_ESCUDO;
                columna = indiceLocal % COLUMNAS_ESCUDO;

                origen.x = columna * PARTE_ESCUDO_ANCHO;
                origen.y = fila * PARTE_ESCUDO_ALTO;
                origen.w = PARTE_ESCUDO_ANCHO;
                origen.h = PARTE_ESCUDO_ALTO;

                destino.x = parte.x;
                destino.y = parte.y;
                destino.w = PARTE_ESCUDO_ANCHO;
                destino.h = PARTE_ESCUDO_ALTO;

                SDL_RenderCopy(render,
                               recursos_escudo()->textura,
                               &origen,
                               &destino);
            }
        }

        i++;
    }
}

void escudo_destruir(Vector *vEscudos, Vector *vEscudosPartes)
{
    vectorDestruir(vEscudos);
    vectorDestruir(vEscudosPartes);
}
