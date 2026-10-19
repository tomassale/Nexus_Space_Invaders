#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "jugador.h"
#include "disparo.h"
#include "mapa.h"
#include "bala.h"
#include "sprites.h"
#include "audio.h"
#include "fonts.h"
#include "vector.h"

#define CANT_PASOS 4
#define RONDAS_PARA_GANAR 1

#define ALTO_CARACTER 8


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
static bool derrota = false;
static bool victoria = false;
static int puntaje;
static int ronda;


static tJugador jugador;
static Vector vBalasJugador;
static Vector vBalasEnemigo;
static bool balasJugadorCreadas = false;
static bool balasEnemigoCreadas = false;
static float tiempoDisparo = 0;


static tSprite sprFondo = {0};
static tSprite sprJugador = {0};
//static tSprite sprEscudos;
//static tSprite sprEnemigos;

static tSonido *sndDisparoJugador;
static tSonido *sndExplosion;
static tSonido *sndDisparoEnemigo;
static tSonido *sndFinalNivel;
static tSonido *sndNaveBajando[CANT_PASOS];

static int canalFinalNivel = -1;
static int pasoActual = 0;
static bool transicionRonda = false;

static tFuente *fntPuntaje;
static const int puntosEnemigo[] = {
    30, /* ENEMIGO_CALAMAR_VERDE      (0) */
    20, /* ENEMIGO_CANGREJO_VERDE     (1) */
    20, /* ENEMIGO_CANGREJO_TURQUESA  (2) */
    10, /* ENEMIGO_PULPO_TURQUESA    (3) */
    10  /* ENEMIGO_PULPO_MORADO      (4) */
};
#define PUNTAJE_VIDA_EXTRA 1000
static int proximaVidaExtra = PUNTAJE_VIDA_EXTRA;

/*** FUNCIONES PRIVADAS ***/

//Jugador

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

static void game_disparar_enemigo(void)
{
    tEnemigos enemigo;
    Bala bala;
    int x, y;

    if(VectorCE(&vBalasEnemigo) != 0)
        return;

    if(!mapa_enemigos_obtener(&enemigo))
        return;

    x = enemigo.x + (ENEMIGO_ANCHO / 2) - (BALA_ANCHO / 2);
    y = enemigo.y + ENEMIGO_ALTO;

    bala_crear(&bala, x, y);
    if(vectorInsertarAlFinal(&vBalasEnemigo, &bala) == TODO_OK)
        sonidoReproducir(sndDisparoEnemigo);
}

static void game_cooldown_enemigo(float delta)
{
    tiempoDisparo += delta;

    if(tiempoDisparo >= 0.1f)
    {
        tiempoDisparo -= 0.1f;

        if(rand() % 100 < 50)
            game_disparar_enemigo();
    }
}

static int hayColisionBalas(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + BALA_ANCHO &&
    x1 + BALA_ANCHO > x2 &&
    y1 < y2 + BALA_ALTO &&
    y1 + BALA_ALTO > y2;
}

static int hayColisionBalaAJugador(int x1, int y1, int x2, int y2)
{
    return x1 < x2 + ANCHO_JUGADOR &&
           x1 + BALA_ANCHO > x2 &&
           y1 < y2 + ALTO_JUGADOR &&
           y1 + BALA_ALTO > y2;
}

static void game_colisiones(void)
{
    unsigned i = 0;
    unsigned j;
    Bala balaJ, balaE;
    int balaJEliminada, tipoEliminado;

    /* Colisiones entre balas del jugador, balas enemigas,
       escudos y enemigos */
    while(i < (unsigned)VectorCE(&vBalasJugador))
    {
        vectorGetByPos(&vBalasJugador, i, &balaJ);
        balaJEliminada = 0;

        /* Colision de la bala del jugador contra un escudo */
        if(mapa_escudo_impactado(balaJ.x, balaJ.y))
        {
            vectorRemoveByPos(&vBalasJugador, i);
            balaJEliminada = 1;
        }

        /* Colision entre balas del jugador y enemigas */
        j = 0;

        while(j < (unsigned)VectorCE(&vBalasEnemigo) &&
              !balaJEliminada)
        {
            vectorGetByPos(&vBalasEnemigo, j, &balaE);

            if(hayColisionBalas(balaJ.x, balaJ.y,
                                balaE.x, balaE.y))
            {
                vectorRemoveByPos(&vBalasJugador, i);
                vectorRemoveByPos(&vBalasEnemigo, j);
                balaJEliminada = 1;
            }
            else
                j++;
        }

        /* Colision de la bala del jugador contra un enemigo */
        if(!balaJEliminada)
        {
            if(mapa_enemigo_impactado(balaJ.x, balaJ.y,
                                      &tipoEliminado))
            {
                vectorRemoveByPos(&vBalasJugador, i);
                balaJEliminada = 1;

                puntaje += puntosEnemigo[tipoEliminado];

                if(puntaje >= proximaVidaExtra)
                {
                    jugador.vidas++;
                    proximaVidaExtra += PUNTAJE_VIDA_EXTRA;
                }
            }
        }

        if(!balaJEliminada)
            i++;
    }

    /* Colisiones entre balas enemigas, escudos y jugador */
    i = 0;

    while(i < (unsigned)VectorCE(&vBalasEnemigo))
    {
        vectorGetByPos(&vBalasEnemigo, i, &balaE);

        /* Colision de la bala enemiga contra un escudo */
        if(mapa_escudo_impactado(balaE.x, balaE.y))
        {
            vectorRemoveByPos(&vBalasEnemigo, i);
        }
        /* Colision de la bala enemiga contra el jugador */
        else if(hayColisionBalaAJugador(balaE.x, balaE.y,
                                        jugador.x, jugador.y))
        {
            jugador.vidas--;
            vectorRemoveByPos(&vBalasEnemigo, i);

            jugador.x = PLAYER_START_X;
            jugador.y = PLAYER_START_Y;
        }
        else
            i++;
    }

    /* Colision entre enemigos y jugador */
    if(mapa_enemigo_toca_jugador(jugador.x, jugador.y))
        jugador.vidas = 0;
}

static void game_actualizar_disparos(void)
{
    vectorMap(&vBalasJugador, bala_mover_jugador, NULL);
    vectorFilter(&vBalasJugador, bala_en_pantalla, NULL);

    vectorMap(&vBalasEnemigo, bala_mover_enemigo, NULL);
    vectorFilter(&vBalasEnemigo, bala_en_pantalla, NULL);
}

static void game_dibujar_texto_centrado(SDL_Renderer *render, const char *texto,
                                        int y, SDL_Color color)
{
    int x = (VENTANA_X - fuenteAncho(fntPuntaje, texto)) / 2;

    fuenteDibujar(render, fntPuntaje, texto, x, y, color);
}

static void game_dibujar_transicion(SDL_Renderer *render)
{
    SDL_Color blanco = {255, 255, 255, 255};
    char texto[32];

    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render);

    snprintf(texto, sizeof(texto), "RONDA %d", ronda + 1);
    game_dibujar_texto_centrado(render, texto, (VENTANA_Y - ALTO_CARACTER) / 2, blanco);
}


static void game_iniciar_ronda(int numero)
{
    ronda = numero;
    pasoActual = 0;

    jugador.x = PLAYER_START_X;
    jugador.y = PLAYER_START_Y;

    vectorDestruir(&vBalasJugador);
    vectorCrear(&vBalasJugador, 8, sizeof(Bala));
    vectorDestruir(&vBalasEnemigo);
    vectorCrear(&vBalasEnemigo, 8, sizeof(Bala));

    mapa_enemigos_destruir();
    mapa_enemigos_iniciar();
    mapa_escudos_destruir();
    mapa_escudos_iniciar();
}

/*** FUNCIONES PUBLICAS ***/

bool gameCrear(SDL_Renderer *render)
{
    char ruta[64];
    int i;

    audioVolumen(30);
    sndDisparoJugador = sonidoCargar("assets/audio/disparo_jugador.wav");
    sndDisparoEnemigo = sonidoCargar("assets/audio/disparo_enemigo.wav");
    sndFinalNivel = sonidoCargar("assets/audio/final_nivel.wav");

    fntPuntaje = fuenteCargar("assets/fonts/space_invaders.ttf", 8);

    for(i = 0; i < CANT_PASOS; i++)
    {
        snprintf(ruta, sizeof(ruta), "assets/audio/fastinvader%d.wav", i + 1);
        sndNaveBajando[i] = sonidoCargar(ruta);
    }

    if(!spriteCargar(&sprJugador, render, "assets/images/jugador.png", 1, 0) ||
            !spriteCargar(&sprFondo, render, "assets/images/fondo.jpg", 1, 0))
    {
        gameDestruir();
        return false;
    }

    if(!sndDisparoJugador || !sndDisparoEnemigo || !sndFinalNivel ||
       !sndNaveBajando[0] || !sndNaveBajando[1] || !sndNaveBajando[2] || !sndNaveBajando[3])
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
    srand((unsigned int)time(NULL));
    puntaje = 0;
    ronda = 1;

    if(!vectorCrear(&vBalasJugador, 8, sizeof(Bala)))
    {
        gameDestruir();
        return false;
    }
    balasJugadorCreadas = true;

    if(!vectorCrear(&vBalasEnemigo, 8, sizeof(Bala)))
    {
        gameDestruir();
        return false;
    }
    balasEnemigoCreadas = true;

    if(!mapa_sprites_cargar(render))
    {
        gameDestruir();
        return false;
    }

    if(!mapa_enemigos_iniciar())
    {
        gameDestruir();
        return false;
    }

    if(!mapa_escudos_iniciar())
    {
        gameDestruir();
        return false;
    }
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
            if(evento.key.keysym.sym == SDLK_ESCAPE && !transicionRonda)
                pausa = !pausa;

            if(evento.key.keysym.sym == SDLK_SPACE && !pausa && !derrota && !transicionRonda)
                game_disparar_jugador();

            if(evento.key.keysym.sym == SDLK_r && (derrota || victoria))
            {
                derrota = false;
                victoria=false;
                puntaje = 0;
                proximaVidaExtra = PUNTAJE_VIDA_EXTRA;
                jugador_iniciar(&jugador);
                game_iniciar_ronda(1);
            }
        }

    }
}

void gameActualizar(float delta)
{
    const Uint8 *teclado;
    static float tiempoMovimientoEnemigos = 0.0f;
    float intervaloMovimiento;

    (void)delta;

    if(pausa || derrota || victoria)
        return;

    if(transicionRonda)
    {
        if(!sonidoSonando(canalFinalNivel))
        {
            transicionRonda = false;
            game_iniciar_ronda(ronda + 1);
        }
        return;
    }

    teclado = SDL_GetKeyboardState(NULL);

    if(teclado[SDL_SCANCODE_LEFT])
        jugador_mover(&jugador, -1);

    if(teclado[SDL_SCANCODE_RIGHT])
        jugador_mover(&jugador, 1);

    game_actualizar_disparos();
    game_colisiones();
    if(jugador.vidas<=0)
    {
        derrota=true;
        return;
    }
    game_cooldown_enemigo(delta);

    tiempoMovimientoEnemigos += delta;
    intervaloMovimiento = mapa_enemigos_intervalo_movimiento();

    if(tiempoMovimientoEnemigos >= intervaloMovimiento)
    {
        mapa_enemigos_mover();
        sonidoReproducir(sndNaveBajando[pasoActual]);
        pasoActual = (pasoActual + 1) % CANT_PASOS;
        tiempoMovimientoEnemigos -= intervaloMovimiento;
    }

    if(!mapa_enemigos_hay_activos())
    {
        if(ronda < RONDAS_PARA_GANAR)
        {
            transicionRonda = true;
            canalFinalNivel = sonidoReproducir(sndFinalNivel);
        }else{
            victoria= true;
        }

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

    snprintf(buf, sizeof(buf), "1UP ROUND %02d  LIVES %02d", ronda, jugador.vidas);
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
    vectorMap(&vBalasEnemigo, bala_dibujar, render);

    /*Dibujar pausa/derrota/victoria*/
    if(pausa || derrota || victoria)
    {
        SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(render, 0, 0, 0, 160);

        rect.x = 0;
        rect.y = 0;
        rect.w = VENTANA_X;
        rect.h = VENTANA_Y;
        SDL_RenderFillRect(render, &rect);
        if(derrota)
        {
            game_dibujar_texto_centrado(render, "DERROTA", (VENTANA_Y - ALTO_CARACTER)/2, blanco);
            game_dibujar_texto_centrado(render, "PRESIONA R PARA REINICIAR", ((VENTANA_Y - ALTO_CARACTER)/2) + 16, blanco);
        }
        else if(victoria)
        {
            game_dibujar_texto_centrado(render, "VICTORIA", ((VENTANA_Y - ALTO_CARACTER)/2) - 8, blanco);
            game_dibujar_texto_centrado(render, "PRESIONA R PARA REINICIAR", ((VENTANA_Y - ALTO_CARACTER)/2) + 8, blanco);
        }else{
            game_dibujar_texto_centrado(render, "PAUSA", (VENTANA_Y - ALTO_CARACTER)/2, blanco);
        }
    }

    if(transicionRonda)
    {
        game_dibujar_transicion(render);
        return;
    }
}

bool gameEjecutando(void)
{
    return ejecutando;
}

void gameDestruir(void)
{

    int i;

    spriteLiberar(&sprFondo);
    spriteLiberar(&sprJugador);
    //spriteLiberar(&sprEnemigos);
    //spriteLiberar(&sprEscudos);

    mapa_escudos_destruir();
    mapa_enemigos_destruir();
    mapa_sprites_liberar();

    sonidoLiberar(sndDisparoJugador);
    sonidoLiberar(sndDisparoEnemigo);
    sonidoLiberar(sndExplosion);
    sonidoLiberar(sndFinalNivel);
    for(i = 0; i < CANT_PASOS; i++)
        sonidoLiberar(sndNaveBajando[i]);



    if(balasJugadorCreadas)
    {
        vectorDestruir(&vBalasJugador);
        balasJugadorCreadas = false;
    }

    if(balasEnemigoCreadas)
    {
        vectorDestruir(&vBalasEnemigo);
        balasEnemigoCreadas = false;
    }

    fuenteLiberar(fntPuntaje);
    fntPuntaje = NULL;

    ejecutando = false;
}
