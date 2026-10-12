#include <SDL.h>
#include <stdbool.h>

#include "game.h"
#include "jugador.h"
#include "disparo.h"

#define ANCHO_JUGADOR 40
#define ALTO_JUGADOR 10

#define ANCHO_DISPARO 4
#define ALTO_DISPARO 10

#define DESPLAZAMIENTO_CANNON_X 20

#define VELOCIDAD_DISPARO_JUGADOR 8
#define VELOCIDAD_DISPARO_ENEMIGO 5

/* Resolucion logica del juego */
#define VENTANA_X 256
#define VENTANA_Y 240

/*** VARIABLES PRIVADAS ***/

static bool ejecutando = false;

static tJugador jugador;
static tDisparo disparoJugador;
static tDisparo disparoEnemigo;

/*** FUNCIONES PRIVADAS ***/

static void disparo_mover(tDisparo *disparo, int direccion, int velocidad)
{
    disparo->y += direccion * velocidad;
}

static int disparo_colision(tDisparo *j, tDisparo *e)
{
    int colision = 0;

    if(j->x < e->x + ANCHO_DISPARO && j->x + ANCHO_DISPARO > e->x && j->y < e->y + ALTO_DISPARO && j->y + ALTO_DISPARO > e->y)
    {
        colision = 1;
    }

    return colision;
}

static void game_disparar_jugador(void)
{
    if(!disparoJugador.activo)
    {
        disparoJugador.x =
            jugador.x + DESPLAZAMIENTO_CANNON_X;

        disparoJugador.y =
            jugador.y - ALTO_DISPARO;

        disparoJugador.activo = 1;
    }
}

static void game_actualizar_disparos(void)
{
    if(disparoJugador.activo)
    {
        disparo_mover(&disparoJugador,-1,VELOCIDAD_DISPARO_JUGADOR);

        if(disparoJugador.y + ALTO_DISPARO < 0)
            disparoJugador.activo = 0;
    }

    if(disparoEnemigo.activo)
    {
        disparo_mover(&disparoEnemigo,1,VELOCIDAD_DISPARO_ENEMIGO);

        if(disparoEnemigo.y > VENTANA_Y)
            disparoEnemigo.activo = 0;
    }

    if(disparoJugador.activo && disparoEnemigo.activo)
    {
        if(disparo_colision(&disparoJugador, &disparoEnemigo))
        {
            disparoJugador.activo = 0;
            disparoEnemigo.activo = 0;
        }
    }
}

/*** FUNCIONES PUBLICAS ***/

bool gameCrear(SDL_Renderer *render)
{
    (void)render;

    jugador_iniciar(&jugador);

    disparoJugador.activo = 0;
    disparoEnemigo.activo = 0;

    ejecutando = true;

    return true;
}

void gameProcesarEventos(void)
{
    SDL_Event evento;

    while(SDL_PollEvent(&evento) != 0)
    {
        if(evento.type == SDL_QUIT)
            ejecutando = false;

        if(evento.type == SDL_KEYDOWN &&
           evento.key.repeat == 0)
        {
            if(evento.key.keysym.sym == SDLK_ESCAPE)
                ejecutando = false;

            if(evento.key.keysym.sym == SDLK_SPACE)
                game_disparar_jugador();
        }
    }
}

void gameActualizar(float delta)
{
    const Uint8 *teclado = SDL_GetKeyboardState(NULL);

    (void)delta;

    if(teclado[SDL_SCANCODE_LEFT])
        jugador_mover(&jugador, -1);

    if(teclado[SDL_SCANCODE_RIGHT])
        jugador_mover(&jugador, 1);

    game_actualizar_disparos();
}

void gameDibujar(SDL_Renderer *render)
{
    SDL_Rect rect;

    /* Dibujar jugador */
    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);

    rect.x = jugador.x;
    rect.y = jugador.y;
    rect.w = ANCHO_JUGADOR;
    rect.h = ALTO_JUGADOR;

    SDL_RenderFillRect(render, &rect);

    /* Dibujar disparo del jugador */
    if(disparoJugador.activo)
    {
        SDL_SetRenderDrawColor(render, 255, 255, 255, 255);

        rect.x = disparoJugador.x;
        rect.y = disparoJugador.y;
        rect.w = ANCHO_DISPARO;
        rect.h = ALTO_DISPARO;

        SDL_RenderFillRect(render, &rect);
    }

    /* Dibujar disparo enemigo */
    if(disparoEnemigo.activo)
    {
        SDL_SetRenderDrawColor(render, 255, 0, 0, 255);

        rect.x = disparoEnemigo.x;
        rect.y = disparoEnemigo.y;
        rect.w = ANCHO_DISPARO;
        rect.h = ALTO_DISPARO;

        SDL_RenderFillRect(render, &rect);
    }
}

bool gameEjecutando(void)
{
    return ejecutando;
}

void gameDestruir(void)
{
    ejecutando = false;
}
