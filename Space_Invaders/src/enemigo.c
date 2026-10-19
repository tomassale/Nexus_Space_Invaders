#include <SDL.h>
#include "enemigo.h"
#include "bala.h"      /* BALA_ANCHO / BALA_ALTO: los usa la colision */
#include "jugador.h"   /* ANCHO_JUGADOR / ALTO_JUGADOR: los usa la colision */
#include "sprites.h"
#include "vector.h"
#include <stdbool.h>

/* =========================================================
 * Formacion clasica del Space Invaders (5 filas x 11 columnas).
 * Son datos privados: nadie afuera necesita saber como esta armada.
 * ========================================================= */
#define FILAS_ENEMIGOS 5
#define COLUMNAS_ENEMIGOS 11
#define INICIO_X 20
#define INICIO_Y 30
#define SEPARACION_X 20
#define SEPARACION_Y 15

#define VENTANA_X 256
#define VELOCIDAD_ENEMIGOS 2
#define DESCENSO_ENEMIGOS 10
#define ALTURA_AMARILLA 150

#define DURACION_ELIMINADO 250

/* =========================================================
 * Estado del modulo: la coleccion de enemigos vive aca adentro.
 * El TDA Vector se encarga de crecer y de compactar la memoria.
 * ========================================================= */
static Vector vEnemigos;

static tSprite sprCalamar[2] = {0};
static tSprite sprCangrejo[2] = {0};
static tSprite sprPulpo[2] = {0};
static tSprite sprEliminado = {0};

static int frameAnimacion = 0;   /* alterna el sprite normal y el animado */
static int direccion = 1;        /* sentido horizontal de la formacion (+1 / -1) */

/* Feedback de muerte: se dibuja el sprite "eliminado" unos ms donde murio */
static Uint32 tiempoEliminado;
static int eliminadoX;
static int eliminadoY;
static int mostrarEliminado = 0;

/* Que tipo de enemigo va en cada fila */
static const int tiposNivel1[FILAS_ENEMIGOS] =
{
    ENEMIGO_CALAMAR_VERDE,
    ENEMIGO_CANGREJO_VERDE,
    ENEMIGO_CANGREJO_TURQUESA,
    ENEMIGO_PULPO_TURQUESA,
    ENEMIGO_PULPO_MORADO
};

/* =========================================================
 * Ciclo de vida
 * ========================================================= */
bool enemigo_cargar(SDL_Renderer *render)
{
    /* Cada bicho tiene su sprite normal y su sprite animado */
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

    return true;
}

int enemigo_iniciar(void)
{
    int fila, columna;
    tEnemigo enemigo;

    /* La coleccion es un Vector: capacidad inicial = la formacion entera */
    if(!vectorCrear(&vEnemigos,
                    FILAS_ENEMIGOS * COLUMNAS_ENEMIGOS,
                    sizeof(tEnemigo)))
        return 0;

    /* Se cargan los 55 enemigos de una: un tipo por fila */
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

void enemigo_destruir(void)
{
    vectorDestruir(&vEnemigos);
}

void enemigo_liberar(void)
{
    spriteLiberar(&sprCalamar[0]);
    spriteLiberar(&sprCangrejo[0]);
    spriteLiberar(&sprPulpo[0]);
    spriteLiberar(&sprCalamar[1]);
    spriteLiberar(&sprCangrejo[1]);
    spriteLiberar(&sprPulpo[1]);
    spriteLiberar(&sprEliminado);
}

/* =========================================================
 * Dibujado
 * ========================================================= */

/* Accion que el TDA aplica a CADA enemigo; el render llega en "datos" */
static int dibujarEnemigo(void *elem, void *datos)
{
    tEnemigo *enemigo = elem;
    SDL_Renderer *render = datos;
    tSprite *sprite = NULL;

    if(!enemigo->activo)
        return 1;

    /* El tipo decide el sprite y su color */
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

    /* Los que ya bajaron demasiado se avisan en amarillo */
    if(enemigo->y >= ALTURA_AMARILLA)
        SDL_SetTextureColorMod(sprite->textura, 255, 255, 0);

    spriteDibujar(render, sprite,
                  enemigo->x, enemigo->y,
                  ENEMIGO_ANCHO, ENEMIGO_ALTO);

    return 1;
}

/* Dibuja el "eliminado" solo durante DURACION_ELIMINADO ms */
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

void enemigo_dibujar(SDL_Renderer *render)
{
    /* vectorMap recorre la coleccion y llama a dibujarEnemigo por cada uno */
    vectorMap(&vEnemigos, dibujarEnemigo, render);
    dibujarEliminado(render);
}

/* =========================================================
 * Movimiento
 * ========================================================= */

/* Accion: avisa si algun enemigo toco un borde de la pantalla */
static int detectarBorde(void *elem, void *datos)
{
    tEnemigo *enemigo = elem;
    int *llegoAlBorde = datos;

    if(enemigo->activo &&
       (enemigo->x + ENEMIGO_ANCHO >= VENTANA_X ||
        enemigo->x <= 0))
    {
        *llegoAlBorde = 1;
    }

    return 1;
}

/* Accion: mueve un enemigo al costado y, si se invirtio el sentido, lo baja */
static int moverEnemigo(void *elem, void *datos)
{
    tEnemigo *enemigo = elem;
    int *llegoAlBorde = datos;

    if(enemigo->activo)
    {
        enemigo->x += VELOCIDAD_ENEMIGOS * direccion;

        if(*llegoAlBorde)
            enemigo->y += DESCENSO_ENEMIGOS;
    }

    return 1;
}

void enemigo_mover(void)
{
    int llegoAlBorde = 0;

    frameAnimacion = 1 - frameAnimacion;

    /* Pasada 1: preguntar si alguien toco el borde */
    vectorMap(&vEnemigos, detectarBorde, &llegoAlBorde);

    if(llegoAlBorde)
        direccion *= -1;

    /* Pasada 2: mover toda la formacion junta */
    vectorMap(&vEnemigos, moverEnemigo, &llegoAlBorde);
}

int enemigo_hay_activos(void)
{
    unsigned i;
    tEnemigo enemigo;

    for(i = 0; i < (unsigned)VectorCE(&vEnemigos); i++)
    {
        if(vectorGetByPos(&vEnemigos, i, &enemigo) &&
           enemigo.activo)
            return 1;
    }

    return 0;
}

/* =========================================================
 * Consultas para la logica del juego
 * ========================================================= */

/* Devuelve un enemigo al azar para que dispare */
int enemigo_obtener(tEnemigo *enemigo)
{
    unsigned pos;
    unsigned cantidad;
    tEnemigo actual;
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

/* Colision rectangular entre la bala del jugador y un enemigo */
static int hayColisionBalaAEnemigo(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + ENEMIGO_ANCHO &&
    x1 + BALA_ANCHO > x2 &&
    y1 < y2 + ENEMIGO_ALTO &&
    y1 + BALA_ALTO > y2;
}

/* Busca que enemigo golpeo la bala; lo borra de la coleccion y avisa el tipo */
int enemigo_impactado(int x, int y, int* tipoEliminado)
{
    unsigned i = 0;
    unsigned cantidad = VectorCE(&vEnemigos);
    tEnemigo enemigo;

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

/* Colision rectangular entre un enemigo y el jugador */
static int hayToqueEnemigoAJugador(int xE, int yE, int xJ, int yJ)
{
    return xE < xJ + ANCHO_JUGADOR &&
    xE + ENEMIGO_ANCHO > xJ &&
    yE < yJ + ALTO_JUGADOR &&
    yE + ENEMIGO_ALTO > yJ;
}

int enemigo_toca_jugador(int x, int y)
{
    unsigned i = 0;
    tEnemigo enemigo;

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
