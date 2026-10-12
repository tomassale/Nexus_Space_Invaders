#include <SDL.h>
#include <stdbool.h>

#include "game.h"
#include "jugador/jugador.h"
#include "jugador/disparo.h"
#include "../engine/sprites.h"

#define ANCHO_JUGADOR 20
#define ALTO_JUGADOR 10

#define ANCHO_DISPARO 2
#define ALTO_DISPARO 10

#define DESPLAZAMIENTO_CANNON_X ((ANCHO_JUGADOR)/2)-((ANCHO_DISPARO)/2)

#define VELOCIDAD_DISPARO_JUGADOR 8
#define VELOCIDAD_DISPARO_ENEMIGO 5

/* Resolucion logica del juego */
#define VENTANA_X 256
#define VENTANA_Y 240

/*** VARIABLES PRIVADAS ***/

static bool ejecutando = false;

static tSprite sprFondo;
static tJugador jugador;
static tDisparo disparoJugador;
static tDisparo disparoEnemigo;
static tSprite sprJugador;
//static tSprite sprEscudos;
//static tSprite sprEnemigos;

/*** FUNCIONES PRIVADAS ***/

//Jugador
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
/*
//Mapa
static void gameMapa(){}

//Recursos
static void gameEscudos(){}
static void gameScore(){}
static void gamePause(){}
*/

/*** FUNCIONES PUBLICAS ***/

bool gameCrear(SDL_Renderer *render)
{
    (void)render;



    if(!spriteCargar(&sprJugador, render, "assets/jugador.png", 1, 0) ||
       !spriteCargar(&sprFondo, render, "assets/fondo.jpg", 1, 0))
    {
        gameDestruir();
        return false;
    }

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

    /*Dibujar fondo*/
    spriteDibujar(render, &sprFondo, 0, 0, VENTANA_X, VENTANA_Y);

    /* Dibujar jugador */
    spriteDibujar(render, &sprJugador, jugador.x, jugador.y, ANCHO_JUGADOR, ALTO_JUGADOR);

    rect.x = jugador.x;
    rect.y = jugador.y;
    rect.w = ANCHO_JUGADOR;
    rect.h = ALTO_JUGADOR;

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
    spriteLiberar(&sprFondo);
    spriteLiberar(&sprJugador);
    //spriteLiberar(&sprEnemigos);
    //spriteLiberar(&sprEscudos);
    ejecutando = false;
}
