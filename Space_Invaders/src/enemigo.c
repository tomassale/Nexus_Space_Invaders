#include <SDL.h>
#include <stdbool.h>

#include "vector.h"

#include "enemigo.h"
#include "motor.h"
#include "recursos.h"
#include "sprites.h"

/* Formacion clasica del Space Invaders (5 filas x 11 columnas) */
#define FILAS_ENEMIGOS 5
#define COLUMNAS_ENEMIGOS 11
#define INICIO_X 20
#define INICIO_Y 30
#define SEPARACION_X 20
#define SEPARACION_Y 15

#define VELOCIDAD_ENEMIGOS 2
#define DESCENSO_ENEMIGOS 10
#define ALTURA_AMARILLA 150
#define TOTAL_ENEMIGOS (FILAS_ENEMIGOS * COLUMNAS_ENEMIGOS)
#define INTERVALO_MOVIMIENTO_MAX 0.5f
#define INTERVALO_MOVIMIENTO_MIN 0.05f

#define DURACION_ELIMINADO 250

/* La coleccion de enemigos (vEnemigos) es del juego: llega por parametro. */

/* =========================================================
 * Estado de la formacion
 * ========================================================= */

static int frameAnimacion = 0;   /* alterna el sprite normal y el animado */
static int direccion = 1;        /* sentido horizontal de la formacion (+1/-1) */

/* Feedback de muerte: se dibuja el "eliminado" unos ms donde murio */
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
 * Formacion
 * ========================================================= */

/* Crea la coleccion en el Vector que le pasa el juego y la llena con la formacion */
int enemigo_iniciar(Vector *vEnemigos)
{
    int fila, columna;
    tEnemigo enemigo;

    /* Capacidad inicial = la formacion entera */
    if(!vectorCrear(vEnemigos,
                    TOTAL_ENEMIGOS,
                    sizeof(tEnemigo)))
        return 0;

    /* Se cargan todos los enemigos de una: un tipo por fila */
    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            enemigo.x = INICIO_X + columna * SEPARACION_X;
            enemigo.y = INICIO_Y + fila * SEPARACION_Y;
            enemigo.activo = 1;
            enemigo.tipo = tiposNivel1[fila];

            if(vectorInsertarAlFinal(vEnemigos, &enemigo) != TODO_OK)
            {
                vectorDestruir(vEnemigos);
                return 0;
            }
        }
    }

    return 1;
}

/* =========================================================
 * Dibujado
 * ========================================================= */

/* Accion del TDA: dibuja UN enemigo; el render llega en "datos" */
static int dibujarEnemigo(void *elem, void *datos)
{
    tEnemigo *enemigo = elem;
    SDL_Renderer *render = datos;
    const tSprite *sprite = NULL;

    if(!enemigo->activo)
        return 1;

    /* El tipo decide el sprite y su color */
    switch(enemigo->tipo)
    {
        case ENEMIGO_CALAMAR_VERDE:
            sprite = recursos_calamar(frameAnimacion);
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
            break;

        case ENEMIGO_CANGREJO_VERDE:
            sprite = recursos_cangrejo(frameAnimacion);
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
            break;

        case ENEMIGO_CANGREJO_TURQUESA:
            sprite = recursos_cangrejo(frameAnimacion);
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
            break;

        case ENEMIGO_PULPO_TURQUESA:
            sprite = recursos_pulpo(frameAnimacion);
            SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
            break;

        case ENEMIGO_PULPO_MORADO:
            sprite = recursos_pulpo(frameAnimacion);
            SDL_SetTextureColorMod(sprite->textura, 200, 102, 238);
            break;

        default:
            return 1;
    }

    /* Los que ya bajaron demasiado se avisan en amarillo */
    if(enemigo->y >= ALTURA_AMARILLA)
        SDL_SetTextureColorMod(sprite->textura, 255, 255, 0);
    if(enemigo->y >= ENEMIGO_ALTURA_ROJA)
        SDL_SetTextureColorMod(sprite->textura, 255, 0, 0);

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
            spriteDibujar(render, recursos_enemigo_eliminado(),
                          eliminadoX, eliminadoY,
                          ENEMIGO_ANCHO, ENEMIGO_ALTO);
    }
}

void enemigo_dibujar(Vector *vEnemigos, SDL_Renderer *render)
{
    /* vectorMap recorre la coleccion y llama a dibujarEnemigo por cada uno */
    vectorMap(vEnemigos, dibujarEnemigo, render);
    dibujarEliminado(render);
}

/* =========================================================
 * Movimiento
 * ========================================================= */

/* Accion del TDA: avisa si algun enemigo toco un borde de la pantalla */
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

/* Accion del TDA: mueve un enemigo al costado y, si se invirtio, lo baja */
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

void enemigo_mover(Vector *vEnemigos)
{
    int llegoAlBorde = 0;

    frameAnimacion = 1 - frameAnimacion;

    /* Pasada 1: preguntar si alguien toco el borde */
    vectorMap(vEnemigos, detectarBorde, &llegoAlBorde);

    if(llegoAlBorde)
        direccion *= -1;

    /* Pasada 2: mover toda la formacion junta */
    vectorMap(vEnemigos, moverEnemigo, &llegoAlBorde);
}

/* Cuantos menos enemigos quedan, menos espera entre pasos: la oleada acelera. */
float enemigo_intervalo_movimiento(Vector *vEnemigos)
{
    float proporcionVivos = (float)VectorCE(vEnemigos) / TOTAL_ENEMIGOS;

    return INTERVALO_MOVIMIENTO_MIN +
           (INTERVALO_MOVIMIENTO_MAX - INTERVALO_MOVIMIENTO_MIN) * proporcionVivos;
}

int enemigo_hay_activos(Vector *vEnemigos)
{
    unsigned i;
    tEnemigo enemigo;

    for(i = 0; i < (unsigned)VectorCE(vEnemigos); i++)
    {
        if(vectorGetByPos(vEnemigos, i, &enemigo) &&
           enemigo.activo)
            return 1;
    }

    return 0;
}

/* =========================================================
 * Consultas para la logica del juego
 * ========================================================= */

/* Devuelve un enemigo al azar para que dispare */
int enemigo_obtener(Vector *vEnemigos, tEnemigo *enemigo)
{
    unsigned pos;
    unsigned cantidad;
    tEnemigo actual;
    int intentos = 0;

    cantidad = (unsigned)VectorCE(vEnemigos);

    if(cantidad == 0 || enemigo == NULL)
        return 0;

    while(intentos < 10)
    {
        pos = (unsigned)(rand() % cantidad);

        if(vectorGetByPos(vEnemigos, pos, &actual))
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

/* Marca donde murio un enemigo para dibujar el "eliminado" unos ms */
void enemigo_marcar_eliminado(int x, int y)
{
    eliminadoX = x;
    eliminadoY = y;
    mostrarEliminado = 1;
    tiempoEliminado = SDL_GetTicks();
}
