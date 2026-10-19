#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "jugador.h"
#include "enemigo.h"
#include "mapa.h"
#include "sprites.h"
#include "audio.h"
#include "fonts.h"
#include "bala.h"
#include "vector.h"

#define ANCHO_CARACTER 6
#define ALTO_CARACTER 8

#define MAX_BALAS_ENEMIGAS 1
#define INTERVALO_DISPARO_ENEMIGO 1.0f

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
static Vector vBalasEnemigo;


static tSprite sprFondo = {0};
//static tSprite sprEscudos;
//static tSprite sprEnemigos;

static tSonido *sndDisparoJugador;
static tSonido *sndDisparoEnemigo;
//static tSonido *sndExplosion;
//static tSonido *sndFinalNivel;
//static tSonido *sndNaveBajando;

static tFuente *fntPuntaje;

/*** FUNCIONES PRIVADAS ***/

static void game_disparar_jugador(void)
{
    if(VectorCE(&vBalasJugador) != 0)
        return;

    Bala bala;
    int x = jugador.x + (JUGADOR_ANCHO / 2) - (BALA_ANCHO / 2);
    int y = jugador.y - BALA_ALTO;

    bala_crear(&bala, x, y);
    vectorInsertarAlFinal(&vBalasJugador, &bala);

    sonidoReproducir(sndDisparoJugador);
}

static void game_disparar_enemigo(void)
{
    Bala bala;
    int xCentro, yBase;

    if(VectorCE(&vBalasEnemigo) >= MAX_BALAS_ENEMIGAS)
        return;

    if(!enemigo_disparador(&xCentro, &yBase))
        return;

    bala_crear(&bala, xCentro - (BALA_ANCHO / 2), yBase);
    vectorInsertarAlFinal(&vBalasEnemigo, &bala);

    sonidoReproducir(sndDisparoEnemigo);
}

static void game_actualizar_disparos(void)
{
    vectorMap(&vBalasJugador, bala_mover_jugador, NULL);
    vectorFilter(&vBalasJugador, bala_en_pantalla, NULL);

    vectorMap(&vBalasEnemigo, bala_mover_enemigo, NULL);
    vectorFilter(&vBalasEnemigo, bala_en_pantalla, NULL);
}


/*** FUNCIONES PUBLICAS ***/

bool gameCrear(SDL_Renderer *render)
{
    srand((unsigned)time(NULL));

    audioVolumen(30);
    sndDisparoJugador = sonidoCargar("assets/audio/disparo_jugador.wav");
    sndDisparoEnemigo = sonidoCargar("assets/audio/disparo_enemigo.wav");
    fntPuntaje = fuenteCargar("assets/fonts/space_invaders.ttf", 8);

    if(!jugador_cargar(&jugador, render) ||
            !spriteCargar(&sprFondo, render, "assets/images/fondo.jpg", 1, 0))
    {
        gameDestruir();
        return false;
    }

    if(!sndDisparoJugador || !sndDisparoEnemigo)
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
    puntaje = 0;
    ronda = 1;

    if(!vectorCrear(&vBalasJugador, 8, sizeof(Bala)))
    {
        gameDestruir();
        return false;
    }

    if(!vectorCrear(&vBalasEnemigo, 8, sizeof(Bala)))
    {
        gameDestruir();
        return false;
    }

    if(!enemigo_cargar(render) || !mapa_sprites_cargar(render))
    {
        gameDestruir();
        return false;
    }

    enemigo_iniciar();
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
    static float tiempoMovimientoEnemigos = 0.0f;
    static float tiempoDisparoEnemigo = 0.0f;

    if(pausa)
        return;

    teclado = SDL_GetKeyboardState(NULL);

    if(teclado[SDL_SCANCODE_LEFT])
        jugador_mover(&jugador, -1);

    if(teclado[SDL_SCANCODE_RIGHT])
        jugador_mover(&jugador, 1);

    game_actualizar_disparos();

    tiempoDisparoEnemigo += delta;

    if(tiempoDisparoEnemigo >= INTERVALO_DISPARO_ENEMIGO)
    {
        game_disparar_enemigo();
        tiempoDisparoEnemigo = 0.0f;
    }

    tiempoMovimientoEnemigos += delta;

    if(tiempoMovimientoEnemigos >= 0.5f)
    {
        enemigo_mover();
        tiempoMovimientoEnemigos -= 0.5f;
    }

    if(!enemigo_hay_activos())
    {
        /* Ac� ten�s que gestionar la victoria */
    }
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
    jugador_dibujar(&jugador, render);

    /* Dibujar enemigos */
    enemigo_dibujar(render);

    /* Dibujar escudos */
    mapa_escudos_dibujar(render);

    /* Dibujar disparo del jugador */
    vectorMap(&vBalasJugador, bala_dibujar, render);

    /* Dibujar disparo enemigo */
    vectorMap(&vBalasEnemigo, bala_dibujar, render);

    /* Dibujar pausa */
    if(pausa)
    {
        SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 160);

        rect.x = 0;
        rect.y = 0;
        rect.w = VENTANA_X;
        rect.h = VENTANA_Y;
        SDL_RenderFillRect(render, &rect);

        fuenteDibujar(render, fntPuntaje, "PAUSA", (VENTANA_X - (ANCHO_CARACTER * 5)) / 2, (VENTANA_Y - ALTO_CARACTER) / 2, blanco);
    }
}


bool gameEjecutando(void)
{
    return ejecutando;
}

void gameDestruir(void)
{
    spriteLiberar(&sprFondo);
    jugador_destruir(&jugador);
    //spriteLiberar(&sprEnemigos);
    //spriteLiberar(&sprEscudos);

    enemigo_liberar();
    mapa_sprites_liberar();

    sonidoLiberar(sndDisparoJugador);
    sonidoLiberar(sndDisparoEnemigo);
    //sonidoLiberar(sndFinalNivel);
    //sonidoLiberar(sndNaveBajando);

    sndDisparoJugador = NULL;
    sndDisparoEnemigo = NULL;
    //sndFinalNivel = NULL;
    //sndNaveBajando = NULL;

    vectorDestruir(&vBalasJugador);
    vectorDestruir(&vBalasEnemigo);

    fuenteLiberar(fntPuntaje);
    fntPuntaje = NULL;

    ejecutando = false;
}
