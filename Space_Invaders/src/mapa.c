#include <SDL.h>
#include "mapa.h"
#include "game.h"
#include "bala.h"
#include "jugador.h"
#include "recursos.h"
#include "sprites.h"
#include "vector.h"
#include "audio.h"
#include <stdbool.h>

#define FILAS_ENEMIGOS 5
#define COLUMNAS_ENEMIGOS 11
#define INICIO_X 20
#define INICIO_Y 30
#define SEPARACION_X 20
#define SEPARACION_Y 15
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

#define VENTANA_X 256
#define VENTANA_Y 240
#define VELOCIDAD_ENEMIGOS 2
#define TOTAL_ENEMIGOS (FILAS_ENEMIGOS * COLUMNAS_ENEMIGOS)
#define INTERVALO_MOVIMIENTO_MAX 0.5f
#define INTERVALO_MOVIMIENTO_MIN 0.05f
#define DESCENSO_ENEMIGOS 10
#define ALTURA_AMARILLA 150

static Uint32 tiempoEliminado;
#define DURACION_ELIMINADO 250
//static Uint8 eliminadoR;
//static Uint8 eliminadoG;
//static Uint8 eliminadoB;

static Vector vEnemigos;
static Vector vEscudos;
static Vector vEscudosPartes;
static int escudosCreados = 0;

static tSprite sprCalamar[2] = {0};
static tSprite sprCangrejo[2] = {0};
static tSprite sprPulpo[2] = {0};
static tSprite sprEscudo = {0};
static tSprite sprEliminado = {0};
static int frameAnimacion = 0;
static int eliminadoX;
static int eliminadoY;
static int mostrarEliminado = 0;
//static SDL_Surface *superficiesEscudo[CANT_ESCUDOS] = {0};
//static SDL_Texture *texturasEscudo[CANT_ESCUDOS] = {0};

static int direccion = 1;

static const int tiposNivel1[FILAS_ENEMIGOS] =
{
    ENEMIGO_CALAMAR_VERDE,
    ENEMIGO_CANGREJO_VERDE,
    ENEMIGO_CANGREJO_TURQUESA,
    ENEMIGO_PULPO_TURQUESA,
    ENEMIGO_PULPO_MORADO
};

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

        for(fila = 0; fila < FILAS_ESCUDO; fila++)
        {
            for(columna = 0; columna < COLUMNAS_ESCUDO; columna++)
            {
                parte.x = escudo.x + columna * 5;
                parte.y = escudo.y + fila * 5;
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

static int hayColisionBalaAEscudo(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + PARTE_ESCUDO_ANCHO &&
    x1 + BALA_ANCHO > x2 &&
    y1 < y2 + PARTE_ESCUDO_ALTO &&
    y1 + BALA_ALTO > y2;
}

int mapa_escudo_impactado(int x, int y)
{
    unsigned i = 0;
    unsigned cantidad = VectorCE(&vEscudosPartes);
    tParteEscudos parte;
    unsigned indiceEscudo;

    while(i < cantidad)
    {
        vectorGetByPos(&vEscudosPartes, i, &parte);

        if(parte.activo && hayColisionBalaAEscudo(x,y,parte.x,parte.y))
        {
            eliminadoX=parte.x;
            eliminadoY=parte.y;

            parte.activo=0;
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

        vectorUpdateByPos(&vEscudosPartes,indiceParte,&parte);
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

static int dibujarEscudo(void *e, void *datos)
{
    tEscudos *escudo = e;
    SDL_Renderer *render = datos;

    if(escudo->activo)
    {
        spriteDibujar(render, &sprEscudo,
                      escudo->x, escudo->y,
                      ESCUDOS_ANCHO, ESCUDOS_ALTO);
    }

    return 1;
}

void mapa_escudos_dibujar(SDL_Renderer *render)
{
    vectorMap(&vEscudos, dibujarEscudo, render);
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

int mapa_enemigos_iniciar(void)
{
    int fila, columna;
    tEnemigos enemigo;

    if(!vectorCrear(&vEnemigos,
                    TOTAL_ENEMIGOS,
                    sizeof(tEnemigos)))
        return 0;

    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            enemigo.x = INICIO_X + columna * SEPARACION_X;
            enemigo.y = INICIO_Y + fila * SEPARACION_Y;
            enemigo.activo = 1;
            enemigo.tipo = tiposNivel1[fila];

            if(vectorInsertarAlFinal(&vEnemigos, &enemigo) != TODO_OK)
            {
                vectorDestruir(&vEnemigos);
                return 0;
            }
        }
    }

    return 1;
}

void mapa_enemigos_destruir(void)
{
    vectorDestruir(&vEnemigos);
}

int mapa_enemigos_obtener(tEnemigos *enemigo)
{
    unsigned pos;
    unsigned cantidad;
    tEnemigos actual;
    int intentos = 0;

    cantidad = (unsigned)VectorCE(&vEnemigos);

    if(cantidad == 0 || enemigo == NULL)
        return 0;

    while(intentos < 10)
    {
        pos = (unsigned)(rand() % cantidad);

        if(vectorGetByPos(&vEnemigos, pos, &actual))
        {
            if(actual.activo)
            {
                *enemigo = actual;
                return 1;
            }
        }

        intentos++;
    }

    return 0;
}

bool mapa_sprites_cargar(SDL_Renderer *render)
{
    if(!spriteCargar(&sprCalamar[0], render,
                     "assets/images/calamar.png", 1, 0))
        return false;

    if(!spriteCargar(&sprCalamar[1], render,
                     "assets/images/calamar_anim.png", 1, 0))
        return false;

    if(!spriteCargar(&sprCangrejo[0], render,
                     "assets/images/cangrejo.png", 1, 0))
        return false;

    if(!spriteCargar(&sprCangrejo[1], render,
                     "assets/images/cangrejo_anim.png", 1, 0))
        return false;

    if(!spriteCargar(&sprPulpo[0], render,
                     "assets/images/pulpo.png", 1, 0))
        return false;

    if(!spriteCargar(&sprPulpo[1], render,
                     "assets/images/pulpo_anim.png", 1, 0))
        return false;

    if(!spriteCargar(&sprEliminado, render,
                     "assets/images/eliminado.png", 1, 0))
        return false;

    if(!spriteCargar(&sprEscudo, render,
                     "assets/images/escudo.png", 1, 0))
        return false;

    return true;
}

static int dibujarEnemigo(void *elem, void *datos)
{
    tEnemigos *enemigo = elem;
    SDL_Renderer *render = datos;
    tSprite *sprite = NULL;

    if(!enemigo->activo)
        return 1;

    switch(enemigo->tipo)
    {
        case ENEMIGO_CALAMAR_VERDE:
            sprite = &sprCalamar[frameAnimacion];
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
            break;

        case ENEMIGO_CANGREJO_VERDE:
            sprite = &sprCangrejo[frameAnimacion];
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
            break;

        case ENEMIGO_CANGREJO_TURQUESA:
            sprite = &sprCangrejo[frameAnimacion];
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
            break;

        case ENEMIGO_PULPO_TURQUESA:
            sprite = &sprPulpo[frameAnimacion];
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
            break;

        case ENEMIGO_PULPO_MORADO:
            sprite = &sprPulpo[frameAnimacion];
            SDL_SetTextureColorMod(sprite->textura, 200, 102, 238);
            break;

        default:
            return 1;
    }

    if(enemigo->y >= ALTURA_AMARILLA)
        SDL_SetTextureColorMod(sprite->textura, 255, 255, 0);

    spriteDibujar(render, sprite,
                  enemigo->x, enemigo->y,
                  ENEMIGO_ANCHO, ENEMIGO_ALTO);

    return 1;
}

static void dibujarEliminado(SDL_Renderer *render)
{
    if(mostrarEliminado)
    {
        if(SDL_GetTicks() - tiempoEliminado >= DURACION_ELIMINADO)
            mostrarEliminado = 0;
        else
            spriteDibujar(render, &sprEliminado,
                          eliminadoX, eliminadoY,
                          ENEMIGO_ANCHO, ENEMIGO_ALTO);
    }
}

void mapa_enemigos_dibujar(SDL_Renderer *render)
{
    vectorMap(&vEnemigos, dibujarEnemigo, render);
    dibujarEliminado(render);
}

static int detectarBorde(void *elem, void *datos)
{
    tEnemigos *enemigo = elem;
    int *llegoAlBorde = datos;

    if(enemigo->activo &&
       (enemigo->x + ENEMIGO_ANCHO >= VENTANA_X ||
        enemigo->x <= 0))
    {
        *llegoAlBorde = 1;
    }

    return 1;
}

static int moverEnemigo(void *elem, void *datos)
{
    tEnemigos *enemigo = elem;
    int *llegoAlBorde = datos;

    if(enemigo->activo)
    {
        enemigo->x += VELOCIDAD_ENEMIGOS * direccion;

        if(*llegoAlBorde)
            enemigo->y += DESCENSO_ENEMIGOS;
    }

    return 1;
}

void mapa_enemigos_mover(void)
{
    int llegoAlBorde = 0;

    frameAnimacion = 1 - frameAnimacion;

    vectorMap(&vEnemigos, detectarBorde, &llegoAlBorde);

    if(llegoAlBorde)
        direccion *= -1;

    vectorMap(&vEnemigos, moverEnemigo, &llegoAlBorde);
}

/* Cuantos menos enemigos quedan, menos espera entre pasos: la oleada acelera. */
float mapa_enemigos_intervalo_movimiento(void)
{
    float proporcionVivos = (float)VectorCE(&vEnemigos) / TOTAL_ENEMIGOS;

    return INTERVALO_MOVIMIENTO_MIN +
           (INTERVALO_MOVIMIENTO_MAX - INTERVALO_MOVIMIENTO_MIN) * proporcionVivos;
}

static int hayColisionBalaAEnemigo(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + ENEMIGO_ANCHO &&
    x1 + BALA_ANCHO > x2 &&
    y1 < y2 + ENEMIGO_ALTO &&
    y1 + BALA_ALTO > y2;
}

int mapa_enemigo_impactado(int x, int y, int* tipoEliminado)
{
    unsigned i = 0;
    unsigned cantidad = VectorCE(&vEnemigos);
    tEnemigos enemigo;

    while(i < cantidad)
    {
        vectorGetByPos(&vEnemigos, i, &enemigo);

        if(hayColisionBalaAEnemigo(x,y,enemigo.x,enemigo.y))
        {
            eliminadoX=enemigo.x;
            eliminadoY=enemigo.y;
            *tipoEliminado=enemigo.tipo;
            mostrarEliminado = 1;
            tiempoEliminado = SDL_GetTicks();

            vectorRemoveByPos(&vEnemigos,i);
            return 1;
        }

        i++;
    }

    return 0;
}

int mapa_enemigos_hay_activos(void)
{
    unsigned i;
    tEnemigos enemigo;

    for(i = 0; i < (unsigned)VectorCE(&vEnemigos); i++)
    {
        if(vectorGetByPos(&vEnemigos, i, &enemigo) &&
           enemigo.activo)
            return 1;
    }

    return 0;
}

static int hayToqueEnemigoAJugador(int xE, int yE, int xJ, int yJ)
{
    return xE < xJ + ANCHO_JUGADOR &&
    xE + ENEMIGO_ANCHO > xJ &&
    yE < yJ + ALTO_JUGADOR &&
    yE + ENEMIGO_ALTO > yJ;
}

int mapa_enemigo_toca_jugador(int x, int y)
{
    unsigned i = 0;
    tEnemigos enemigo;

    while(i < (unsigned)VectorCE(&vEnemigos))
    {
        vectorGetByPos(&vEnemigos, i, &enemigo);

        if(enemigo.activo &&
           hayToqueEnemigoAJugador(enemigo.x, enemigo.y, x, y))
            return 1;

        i++;
    }

    return 0;
}

void mapa_sprites_liberar(void)
{
    spriteLiberar(&sprCalamar[0]);
    spriteLiberar(&sprCangrejo[0]);
    spriteLiberar(&sprPulpo[0]);
    spriteLiberar(&sprCalamar[1]);
    spriteLiberar(&sprCangrejo[1]);
    spriteLiberar(&sprPulpo[1]);
    spriteLiberar(&sprEliminado);
    spriteLiberar(&sprEscudo);
}
