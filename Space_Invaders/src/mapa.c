#include <SDL.h>
#include "mapa.h"
#include "bala.h"      /* BALA_ANCHO / BALA_ALTO: los usa la colision */
#include "sprites.h"
#include "enemigo.h"
#include "vector.h"
#include "audio.h"
#include <stdbool.h>

/* Posicion y separacion de los escudos en pantalla */
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

#define VENTANA_X 256
#define VENTANA_Y 240


static Vector vEscudos;
static Vector vEscudosPartes;
static int escudosCreados = 0;

static tSprite sprEscudo = {0};

int mapa_escudos_iniciar(void)
{
    int i, fila, columna;
    tEscudos escudo;
    tParteEscudos parte;

    if(!vectorCrear(&vEscudos, CANT_ESCUDOS, sizeof(tEscudos)))
        return 0;

    if(!vectorCrear(&vEscudosPartes, PARTES_ESCUDOS_TOTAL, sizeof(tParteEscudos)))
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
            mapa_escudos_destruir();
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

                if(vectorInsertarAlFinal(&vEscudosPartes, &parte) != TODO_OK)
                {
                    mapa_escudos_destruir();
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

/* Busca que parte del escudo golpeo la bala y la destruye */
int mapa_escudo_impactado(int x, int y)
{
    unsigned i = 0;
    unsigned cantidad = (unsigned)VectorCE(&vEscudosPartes);
    unsigned indiceEscudo;
    tParteEscudos parte;

    while(i < cantidad)
    {
        if(vectorGetByPos(&vEscudosPartes, i, &parte) &&
           parte.activo &&
           hayColisionBalaAEscudo(x, y, parte.x, parte.y))
        {
            mapa_destruir_parte_escudo(i);

            indiceEscudo = i / PARTES_POR_ESCUDO;
            mapa_actualizar_estado_escudo(indiceEscudo);

            return 1;
        }

        i++;
    }

    return 0;
}

void mapa_destruir_parte_escudo(unsigned indiceParte)
{
    tParteEscudos parte;

    if(vectorGetByPos(&vEscudosPartes, indiceParte, &parte))
    {
        parte.activo = 0;

        vectorUpdateByPos(&vEscudosPartes, indiceParte, &parte);
    }
}

void mapa_actualizar_estado_escudo(unsigned indiceEscudo)
{
    unsigned i = indiceEscudo * PARTES_POR_ESCUDO;
    unsigned fin = i + PARTES_POR_ESCUDO;
    int hayPartesActivas = 0;
    tParteEscudos parte;
    tEscudos escudo;

    while(i < fin && !hayPartesActivas)
    {
        if(vectorGetByPos(&vEscudosPartes, i, &parte) &&
           parte.activo)
        {
            hayPartesActivas = 1;
        }

        i++;
    }

    if(!hayPartesActivas &&
       vectorGetByPos(&vEscudos, indiceEscudo, &escudo))
    {
        escudo.activo = 0;

        vectorUpdateByPos(&vEscudos, indiceEscudo, &escudo);
    }
}

/* Colision entre un enemigo y una parte del escudo */
static int hayColisionEnemigoAEscudo(int xE, int yE,
                                     int xP, int yP)
{
    return xE < xP + PARTE_ESCUDO_ANCHO &&
           xE + ENEMIGO_ANCHO > xP &&
           yE < yP + PARTE_ESCUDO_ALTO &&
           yE + ENEMIGO_ALTO > yP;
}

/* Destruye las partes del escudo que toca la formacion */
void mapa_enemigos_rompen_escudos(Vector *vEnemigos)
{
    unsigned i, j;
    unsigned cantEnemigos = (unsigned)VectorCE(vEnemigos);
    unsigned cantPartes = (unsigned)VectorCE(&vEscudosPartes);
    tEnemigo enemigo;
    tParteEscudos parte;

    for(i = 0; i < cantEnemigos; i++)
    {
        if(vectorGetByPos(vEnemigos, i, &enemigo) &&
           enemigo.activo)
        {
            for(j = 0; j < cantPartes; j++)
            {
                if(vectorGetByPos(&vEscudosPartes, j, &parte) &&
                   parte.activo &&
                   hayColisionEnemigoAEscudo(
                       enemigo.x, enemigo.y, parte.x, parte.y))
                {
                    mapa_destruir_parte_escudo(j);
                    mapa_actualizar_estado_escudo(
                        j / PARTES_POR_ESCUDO);
                }
            }
        }
    }
}

void mapa_escudos_dibujar(SDL_Renderer *render)
{
    unsigned i = 0;
    unsigned cantidad = (unsigned)VectorCE(&vEscudosPartes);
    unsigned indiceEscudo;
    unsigned indiceLocal;
    unsigned fila;
    unsigned columna;

    tParteEscudos parte;
    tEscudos escudo;

    SDL_Rect origen;
    SDL_Rect destino;

    while(i < cantidad)
    {
        if(vectorGetByPos(&vEscudosPartes, i, &parte) &&
           parte.activo)
        {
            indiceEscudo = i / PARTES_POR_ESCUDO;
            indiceLocal = i % PARTES_POR_ESCUDO;

            if(vectorGetByPos(&vEscudos, indiceEscudo, &escudo) &&
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
                               sprEscudo.textura,
                               &origen,
                               &destino);
            }
        }

        i++;
    }
}

void mapa_escudos_destruir(void)
{
    if(escudosCreados)
    {
        vectorDestruir(&vEscudos);
        vectorDestruir(&vEscudosPartes);
        escudosCreados = 0;
    }
}

bool mapa_sprites_cargar(SDL_Renderer *render)
{
    if(!spriteCargar(&sprEscudo, render,
                     "assets/images/escudo.png", 1, 0))
        return false;

    return true;
}

void mapa_sprites_liberar(void)
{
    spriteLiberar(&sprEscudo);
}
