#include <stdbool.h>

#include "game.h"
#include "jugador/jugador.h"
#include "jugador/disparo.h"
#include "mapa/mapa.h"
#include "../engine/sprites.h"
#include "../engine/audio.h"
#include "../engine/fonts.h"

#include "jugador/bala.h"
#include "../engine/vector.h"

#define ANCHO_CARACTER 6
#define ALTO_CARACTER 8

#define ANCHO_JUGADOR 20
#define ALTO_JUGADOR 10

//#define JUGADOR_ANCHO 20
//#define JUGADOR_ALTO 10


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
static bool pausa = false;
static int puntaje;
static int ronda;


static tJugador jugador;
static Vector vBalasJugador;
static tDisparo disparoEnemigo;


static tSprite sprFondo = {0};
static tSprite sprJugador = {0};
//static tSprite sprEscudos;
//static tSprite sprEnemigos;

static tSonido *sndDisparoJugador;
//static tSonido *sndExplosion;
//static tSonido *sndDisparoEnemigo;
//static tSonido *sndFinalNivel;
//static tSonido *sndNaveBajando;

static tFuente *fntPuntaje;

/*** FUNCIONES PRIVADAS ***/

//Jugador
static void disparo_mover(tDisparo *disparo, int direccion, int velocidad)
{
    disparo->y += direccion * velocidad;
}


static void game_disparar_jugador(void)
{
    if(VectorCE(&vBalasJugador) != 0)
        return;

    Bala bala;
    int x = jugador.x + (ANCHO_JUGADOR / 2) - (BALA_ANCHO / 2);
    int y = jugador.y - BALA_ALTO;

    bala_crear(&bala, x, y);
    vectorInsertarAlFinal(&vBalasJugador, &bala);

    sonidoReproducir(sndDisparoJugador);
}

static void game_actualizar_disparos(void)
{
    vectorMap(&vBalasJugador, bala_mover_jugador, NULL);
    vectorFilter(&vBalasJugador, bala_en_pantalla, NULL);

    if(disparoEnemigo.activo)
    {
        disparo_mover(&disparoEnemigo,1,VELOCIDAD_DISPARO_ENEMIGO);

        if(disparoEnemigo.y > VENTANA_Y)
            disparoEnemigo.activo = 0;
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
    audioVolumen(30);
    sndDisparoJugador = sonidoCargar("assets/audio/disparo_jugador.wav");
    fntPuntaje = fuenteCargar("assets/fonts/space_invaders.ttf", 8);

    if(!spriteCargar(&sprJugador, render, "assets/images/jugador.png", 1, 0) ||
            !spriteCargar(&sprFondo, render, "assets/images/fondo.jpg", 1, 0))
    {
        gameDestruir();
        return false;
    }

    if(!sndDisparoJugador)
    {
        gameDestruir();
        return false;
    }

    if(!fntPuntaje)
    {
        gameDestruir();
        return false;
    }

    jugador_iniciar(&jugador);
    disparoEnemigo.activo = 0;
    puntaje = 0;
    ronda = 1;

    if(!vectorCrear(&vBalasJugador, 8, sizeof(Bala)))
    {
        gameDestruir();
        return false;
    }

    if(!mapa_sprites_cargar(render))
    {
        gameDestruir();
        return false;
    }

    mapa_enemigos_iniciar();
    mapa_escudos_iniciar();
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
                pausa = !pausa;

            if(evento.key.keysym.sym == SDLK_SPACE && !pausa)
                game_disparar_jugador();
        }

    }
}

void gameActualizar(float delta)
{
    const Uint8 *teclado;

    (void)delta;

    if(pausa)
        return;

    teclado = SDL_GetKeyboardState(NULL);

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

    /*Dibujar puntaje*/
    SDL_Color blanco = {255, 255, 255, 255};
    char buf[32];

    snprintf(buf, sizeof(buf), "HIGH SCORE %05d", puntaje);
    fuenteDibujar(render, fntPuntaje, buf, 8, 4, blanco);

    snprintf(buf, sizeof(buf), "1UP ROUND %02d", ronda);
    fuenteDibujar(render, fntPuntaje, buf, 8, 17, blanco);

    /* Dibujar jugador */
    spriteDibujar(render, &sprJugador, jugador.x, jugador.y, ANCHO_JUGADOR, ALTO_JUGADOR);

    rect.x = jugador.x;
    rect.y = jugador.y;
    rect.w = ANCHO_JUGADOR;
    rect.h = ALTO_JUGADOR;

    /* Dibujar enemigos */
    mapa_enemigos_dibujar(render);

    /* Dibujar escudos */
    mapa_escudos_dibujar(render);

    /* Dibujar disparo del jugador */
    vectorMap(&vBalasJugador, bala_dibujar, render);

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

    /*Dibujar pausa*/
    if(pausa)
    {
        SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 160);

        rect.x = 0;
        rect.y = 0;
        rect.w = VENTANA_X;
        rect.h = VENTANA_Y;
        SDL_RenderFillRect(render, &rect);

        fuenteDibujar(render, fntPuntaje, "PAUSA", (VENTANA_X - (ANCHO_CARACTER * 5)) / 2, (VENTANA_Y - ALTO_CARACTER)/2, blanco);
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

    mapa_sprites_liberar();

    sonidoLiberar(sndDisparoJugador);
    //sonidoLiberar(sndDisparoEnemigo);
    //sonidoLiberar(sndFinalNivel);
    //sonidoLiberar(sndNaveBajando);

    sndDisparoJugador = NULL;
    //sndDisparoEnemigo = NULL;
    //sndFinalNivel = NULL;
    //sndNaveBajando = NULL;

     vectorDestruir(&vBalasJugador);

    fuenteLiberar(fntPuntaje);
    fntPuntaje = NULL;

    ejecutando = false;
}
