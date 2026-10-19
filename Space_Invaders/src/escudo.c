#include <SDL.h>
#include "escudo.h"
#include "bala.h"      /* BALA_ANCHO / BALA_ALTO: los usa la colision */
#include "sprites.h"
#include "vector.h"
#include <stdbool.h>

/* Posicion y separacion de los escudos en pantalla */
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

/* =========================================================
 * Estado del modulo: los escudos y sus partes viven aca adentro.
 * ========================================================= */
static Vector vEscudos;
static Vector vEscudosPartes;
static int escudosCreados = 0;

static tSprite sprEscudo = {0};

int escudo_iniciar(void)
{
    int i, fila, columna;
    tEscudo escudo;
    tParteEscudo parte;

    if(!vectorCrear(&vEscudos, CANT_ESCUDOS, sizeof(tEscudo)))
        return 0;

    if(!vectorCrear(&vEscudosPartes, PARTES_ESCUDOS_TOTAL, sizeof(tParteEscudo)))
    {
        vectorDestruir(&vEscudos);
        return 0;
    }

    escudosCreados = 1;

    for(i = 0; i < CANT_ESCUDOS; i++)
    {
        escudo.x = ESCUDOS_INICIO_X + i * ESCUDOS_SEPARACION_X;
        escudo.y = ESCUDOS_INICIO_Y;
        escudo.activo = 1;

        if(vectorInsertarAlFinal(&vEscudos, &escudo) != TODO_OK)
        {
            escudo_destruir();
            return 0;
        }

        /* Cada escudo se divide en una grilla de partes chicas */
        for(fila = 0; fila < FILAS_ESCUDO; fila++)
        {
            for(columna = 0; columna < COLUMNAS_ESCUDO; columna++)
            {
                parte.x = escudo.x + columna * 5;
                parte.y = escudo.y + fila * 5;
                parte.activo = 1;

                if(vectorInsertarAlFinal(&vEscudosPartes, &parte) != TODO_OK)
                {
                    escudo_destruir();
                    return 0;
                }
            }
        }
    }

    return 1;
}

/* Colision rectangular entre la bala y una parte del escudo */
static int hayColisionBalaAEscudo(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + PARTE_ESCUDO_ANCHO &&
    x1 + BALA_ANCHO > x2 &&
    y1 < y2 + PARTE_ESCUDO_ALTO &&
    y1 + BALA_ALTO > y2;
}

/* Apaga UNA parte del escudo */
static void destruirParteEscudo(unsigned indiceParte)
{
    tParteEscudo parte;

    if(vectorGetByPos(&vEscudosPartes, indiceParte, &parte))
    {
        parte.activo = 0;

        vectorUpdateByPos(&vEscudosPartes, indiceParte, &parte);
    }
}

/* Apaga el escudo entero recien cuando no le queda ninguna parte viva */
static void actualizarEstadoEscudo(unsigned indiceEscudo)
{
    unsigned i = indiceEscudo * PARTES_POR_ESCUDO;
    unsigned fin = i + PARTES_POR_ESCUDO;
    int hayPartesActivas = 0;
    tParteEscudo parte;
    tEscudo escudo;

    while(i < fin && !hayPartesActivas)
    {
        if(vectorGetByPos(&vEscudosPartes, i, &parte) &&
           parte.activo)
            hayPartesActivas = 1;

        i++;
    }

    if(!hayPartesActivas &&
       vectorGetByPos(&vEscudos, indiceEscudo, &escudo))
    {
        escudo.activo = 0;
        vectorUpdateByPos(&vEscudos, indiceEscudo, &escudo);
    }
}

/* Busca que parte del escudo golpeo la bala y la destruye */
int escudo_impactado(int x, int y)
{
    unsigned i = 0;
    unsigned cantidad = VectorCE(&vEscudosPartes);
    tParteEscudo parte;
    unsigned indiceEscudo;

    while(i < cantidad)
    {
        vectorGetByPos(&vEscudosPartes, i, &parte);

        if(parte.activo && hayColisionBalaAEscudo(x, y, parte.x, parte.y))
        {
            destruirParteEscudo(i);

            /* A que escudo pertenece la parte golpeada */
            indiceEscudo = i / PARTES_POR_ESCUDO;
            actualizarEstadoEscudo(indiceEscudo);
            return 1;
        }

        i++;
    }

    return 0;
}

/* Accion que el TDA aplica a cada escudo; el render llega en "datos" */
static int dibujarEscudo(void *e, void *datos)
{
    tEscudo *escudo = e;
    SDL_Renderer *render = datos;

    if(escudo->activo)
    {
        spriteDibujar(render, &sprEscudo,
                      escudo->x, escudo->y,
                      ESCUDOS_ANCHO, ESCUDOS_ALTO);
    }

    return 1;
}

void escudo_dibujar(SDL_Renderer *render)
{
    vectorMap(&vEscudos, dibujarEscudo, render);
}

void escudo_destruir(void)
{
    if(escudosCreados)
    {
        vectorDestruir(&vEscudos);
        vectorDestruir(&vEscudosPartes);
        escudosCreados = 0;
    }
}

/* =========================================================
 * Sprites
 * ========================================================= */
bool escudo_cargar(SDL_Renderer *render)
{
    if(!spriteCargar(&sprEscudo, render,
                     "assets/images/escudo.png", 1, 0))
        return false;

    return true;
}

void escudo_liberar(void)
{
    spriteLiberar(&sprEscudo);
}
