#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "jugador.h"
#include "enemigo.h"
#include "disparo.h"
#include "mapa.h"
#include "bala.h"
#include "sprites.h"
#include "audio.h"
#include "fonts.h"
#include "vector.h"

#define CANT_PASOS 4
#define RONDAS_PARA_GANAR 2

#define RUTA_IMAGEN_MENU "assets/images/menu.png"
#define RUTA_MUSICA_MENU "assets/audio/spaceinvaders1.mpeg"

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
static bool enMenu = true;
static bool pausa = false;
static bool derrota = false;
static bool victoria = false;
static int puntaje;
static int ronda;


/* Colecciones: el juego es dueno de los datos; los modulos solo saben usarlos */
static tJugador jugador;
static Vector vBalasJugador;
static Vector vBalasEnemigo;
static Vector vEnemigos;
static bool balasJugadorCreadas = false;
static bool balasEnemigoCreadas = false;
static bool enemigosCreados = false;
static float tiempoDisparo = 0;


static tSprite sprFondo = {0};
static tSprite sprJugador = {0};
static tSprite sprMenu = {0};
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
    tEnemigo enemigo;
    Bala bala;
    int x, y;

    if(VectorCE(&vBalasEnemigo) != 0)
        return;

    if(!enemigo_obtener(&vEnemigos, &enemigo))
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

/* Test unico: se superponen dos rectangulos? (x, y, ancho, alto) */
static bool game_hay_choque(int x1, int y1, int ancho1, int alto1,
                            int x2, int y2, int ancho2, int alto2)
{
    bool separadosEnX = x1 >= x2 + ancho2 || x1 + ancho1 <= x2;
    bool separadosEnY = y1 >= y2 + alto2 || y1 + alto1 <= y2;

    return !separadosEnX && !separadosEnY;
}

/* Bala (de cualquiera de las dos colecciones) contra un escudo */
static void colisiones_bala_escudo(Vector *vBalas)
{
    unsigned i = 0;
    Bala bala;

    while(i < (unsigned)VectorCE(vBalas))
    {
        vectorGetByPos(vBalas, i, &bala);

        if(mapa_escudo_impactado(bala.x, bala.y))
            vectorRemoveByPos(vBalas, i);   /* no avanzo i: se corre la siguiente */
        else
            i++;
    }
}

/* Bala del jugador contra bala enemiga: se destruyen las dos */
static void colisiones_balas_jugador_bala_enemiga(void)
{
    unsigned i = 0, j;
    Bala balaJ, balaE;
    int eliminada;

    while(i < (unsigned)VectorCE(&vBalasJugador))
    {
        vectorGetByPos(&vBalasJugador, i, &balaJ);
        eliminada = 0;

        for(j = 0; j < (unsigned)VectorCE(&vBalasEnemigo) && !eliminada; j++)
        {
            vectorGetByPos(&vBalasEnemigo, j, &balaE);

            if(game_hay_choque(balaJ.x, balaJ.y, BALA_ANCHO, BALA_ALTO,
                               balaE.x, balaE.y, BALA_ANCHO, BALA_ALTO))
            {
                vectorRemoveByPos(&vBalasJugador, i);
                vectorRemoveByPos(&vBalasEnemigo, j);
                eliminada = 1;
            }
        }

        if(!eliminada)
            i++;
    }
}

/* Bala del jugador contra un enemigo: se borran los dos y suma puntaje */
static void colisiones_balas_jugador_enemigos(void)
{
    unsigned i = 0, j;
    Bala bala;
    tEnemigo enemigo;
    int golpeado;

    while(i < (unsigned)VectorCE(&vBalasJugador))
    {
        vectorGetByPos(&vBalasJugador, i, &bala);

        /* Busco el primer enemigo que choque con esta bala */
        golpeado = -1;

        for(j = 0; j < (unsigned)VectorCE(&vEnemigos); j++)
        {
            vectorGetByPos(&vEnemigos, j, &enemigo);

            if(game_hay_choque(bala.x, bala.y, BALA_ANCHO, BALA_ALTO,
                               enemigo.x, enemigo.y, ENEMIGO_ANCHO, ENEMIGO_ALTO))
            {
                golpeado = (int)j;
                break;
            }
        }

        if(golpeado >= 0)
        {
            /* "enemigo" quedo con el que choco (el ultimo leido) */
            enemigo_marcar_eliminado(enemigo.x, enemigo.y);
            puntaje += puntosEnemigo[enemigo.tipo];

            if(puntaje >= proximaVidaExtra)
            {
                jugador.vidas++;
                proximaVidaExtra += PUNTAJE_VIDA_EXTRA;
            }

            vectorRemoveByPos(&vEnemigos, (unsigned)golpeado);
            vectorRemoveByPos(&vBalasJugador, i);
            /* no avanzo i: se corre la siguiente bala */
        }
        else
            i++;
    }
}

/* Bala enemiga contra el jugador: pierde una vida y vuelve al inicio */
static void colisiones_bala_enemiga_jugador(void)
{
    unsigned i = 0;
    Bala bala;

    while(i < (unsigned)VectorCE(&vBalasEnemigo))
    {
        vectorGetByPos(&vBalasEnemigo, i, &bala);

        if(game_hay_choque(bala.x, bala.y, BALA_ANCHO, BALA_ALTO,
                           jugador.x, jugador.y, ANCHO_JUGADOR, ALTO_JUGADOR))
        {
            jugador.vidas--;
            jugador.x = PLAYER_START_X;
            jugador.y = PLAYER_START_Y;

            vectorRemoveByPos(&vBalasEnemigo, i);
        }
        else
            i++;
    }
}

/* La formacion llego al jugador: se termina la partida */
static void colisiones_enemigos_jugador(void)
{
    unsigned i = 0;
    tEnemigo enemigo;

    while(i < (unsigned)VectorCE(&vEnemigos))
    {
        vectorGetByPos(&vEnemigos, i, &enemigo);

        if(game_hay_choque(enemigo.x, enemigo.y, ENEMIGO_ANCHO, ENEMIGO_ALTO,
                           jugador.x, jugador.y, ANCHO_JUGADOR, ALTO_JUGADOR))
        {
            jugador.vidas = 0;
            return;
        }

        i++;
    }
}

/* Deteccion de colisiones (paso del ciclo del juego).
   El ORDEN importa: una bala puede morir una sola vez. */
static void game_colisiones(void)
{
    colisiones_bala_escudo(&vBalasJugador);          /* bala jugador vs escudo */
    colisiones_balas_jugador_bala_enemiga();         /* bala jugador vs bala enemiga */
    colisiones_balas_jugador_enemigos();             /* bala jugador vs enemigo */
    colisiones_bala_escudo(&vBalasEnemigo);          /* bala enemiga vs escudo */
    colisiones_bala_enemiga_jugador();               /* bala enemiga vs jugador */
    colisiones_enemigos_jugador();                   /* enemigo vs jugador */
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

static void game_dibujar_menu(SDL_Renderer *render)
{
    SDL_Color blanco = {255, 255, 255, 255};

    /* Sin imagen de portada, se usa el fondo del juego con el titulo en texto */
    if(sprMenu.textura)
    {
        spriteDibujar(render, &sprMenu, 0, 0, VENTANA_X, VENTANA_Y);
    }
    else
    {
        spriteDibujar(render, &sprFondo, 0, 0, VENTANA_X, VENTANA_Y);
        game_dibujar_texto_centrado(render, "SPACE INVADERS", VENTANA_Y / 3, blanco);
    }

    /* Parpadea cada medio segundo, como en los arcades */
    if((SDL_GetTicks() / 500) % 2 == 0)
        game_dibujar_texto_centrado(render, "PRESIONA ESPACIO PARA JUGAR",
                                    VENTANA_Y - 40, blanco);
}

static void game_comenzar_partida(void)
{
    enMenu = false;
    musicaDetener();
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

    vectorDestruir(&vEnemigos);
    enemigo_iniciar(&vEnemigos);
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

    if(!enemigo_cargar(render) || !mapa_sprites_cargar(render))
    {
        gameDestruir();
        return false;
    }

    if(!enemigo_iniciar(&vEnemigos))
    {
        gameDestruir();
        return false;
    }
    enemigosCreados = true;

    if(!mapa_escudos_iniciar())
    {
        gameDestruir();
        return false;
    }

    /* La portada y la musica son opcionales: si faltan, el juego arranca igual */
    spriteCargar(&sprMenu, render, RUTA_IMAGEN_MENU, 1, 0);
    musicaReproducir(RUTA_MUSICA_MENU, true);

    enMenu = true;
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
                evento.key.repeat == 0 && enMenu)
        {
            if(evento.key.keysym.sym == SDLK_SPACE)
                game_comenzar_partida();
        }
        else if(evento.type == SDL_KEYDOWN &&
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

    if(enMenu || pausa || derrota || victoria)
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
    intervaloMovimiento = enemigo_intervalo_movimiento(&vEnemigos);

    if(tiempoMovimientoEnemigos >= intervaloMovimiento)
    {
        enemigo_mover(&vEnemigos);
        sonidoReproducir(sndNaveBajando[pasoActual]);
        pasoActual = (pasoActual + 1) % CANT_PASOS;
        tiempoMovimientoEnemigos -= intervaloMovimiento;
    }

    if(!enemigo_hay_activos(&vEnemigos))
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

    if(enMenu)
    {
        game_dibujar_menu(render);
        return;
    }

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
    enemigo_dibujar(&vEnemigos, render);

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
    spriteLiberar(&sprMenu);
    musicaDetener();
    //spriteLiberar(&sprEnemigos);
    //spriteLiberar(&sprEscudos);

    mapa_escudos_destruir();
    enemigo_liberar();
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

    if(enemigosCreados)
    {
        vectorDestruir(&vEnemigos);
        enemigosCreados = false;
    }

    fuenteLiberar(fntPuntaje);
    fntPuntaje = NULL;

    ejecutando = false;
}
