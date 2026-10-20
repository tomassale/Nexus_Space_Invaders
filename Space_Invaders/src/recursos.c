#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "recursos.h"
#include "sprites.h"

/* Rutas de todas las imagenes del juego */
#define RUTA_FONDO          "assets/images/fondo.jpg"
#define RUTA_JUGADOR        "assets/images/jugador.png"
#define RUTA_ESCUDO         "assets/images/escudo.png"

#define RUTA_CALAMAR        "assets/images/calamar.png"
#define RUTA_CALAMAR_ANIM   "assets/images/calamar_anim.png"
#define RUTA_CANGREJO       "assets/images/cangrejo.png"
#define RUTA_CANGREJO_ANIM  "assets/images/cangrejo_anim.png"
#define RUTA_PULPO          "assets/images/pulpo.png"
#define RUTA_PULPO_ANIM     "assets/images/pulpo_anim.png"
#define RUTA_ELIMINADO      "assets/images/eliminado.png"

/* Rutas de los sonidos y de la fuente */
#define RUTA_DISPARO_JUGADOR "assets/audio/disparo_jugador.wav"
#define RUTA_DISPARO_ENEMIGO "assets/audio/disparo_enemigo.wav"
#define RUTA_FINAL_NIVEL     "assets/audio/final_nivel.wav"
#define RUTA_NAVE_BAJANDO    "assets/audio/fastinvader%d.wav" /* %d: paso, de 1 a CANT_PASOS */

#define RUTA_FUENTE          "assets/fonts/space_invaders.ttf"
#define TAMANIO_FUENTE       8

/* =========================================================
 * Texturas del juego
 * ========================================================= */
static tSprite sprFondo = {0};
static tSprite sprJugador = {0};
static tSprite sprEscudo = {0};

static tSprite sprCalamar[2] = {0};
static tSprite sprCangrejo[2] = {0};
static tSprite sprPulpo[2] = {0};
static tSprite sprEnemigoEliminado = {0};

/* =========================================================
 * Sonidos y fuente del juego
 * ========================================================= */
static tSonido *sndDisparoJugador = NULL;
static tSonido *sndDisparoEnemigo = NULL;
static tSonido *sndFinalNivel = NULL;
static tSonido *sndNaveBajando[CANT_PASOS] = {NULL};

static tFuente *fntJuego = NULL;

/* Carga UNA textura (siempre de un solo frame) */
static bool cargarTextura(tSprite *s, SDL_Renderer *render, const char *ruta)
{
    return spriteCargar(s, render, ruta, 1, 0);
}

static bool cargarSonidos(void)
{
    char ruta[64];
    int i;

    sndDisparoJugador = sonidoCargar(RUTA_DISPARO_JUGADOR);
    sndDisparoEnemigo = sonidoCargar(RUTA_DISPARO_ENEMIGO);
    sndFinalNivel = sonidoCargar(RUTA_FINAL_NIVEL);

    if(!sndDisparoJugador || !sndDisparoEnemigo || !sndFinalNivel)
        return false;

    for(i = 0; i < CANT_PASOS; i++)
    {
        snprintf(ruta, sizeof(ruta), RUTA_NAVE_BAJANDO, i + 1);
        sndNaveBajando[i] = sonidoCargar(ruta);

        if(!sndNaveBajando[i])
            return false;
    }

    return true;
}

/* Libera UN sonido y deja el puntero en NULL para que liberar dos veces sea seguro */
static void liberarSonido(tSonido **s)
{
    sonidoLiberar(*s);
    *s = NULL;
}

bool recursos_cargar(SDL_Renderer *render)
{
    if(!cargarTextura(&sprFondo, render, RUTA_FONDO) ||
       !cargarTextura(&sprJugador, render, RUTA_JUGADOR) ||
       !cargarTextura(&sprEscudo, render, RUTA_ESCUDO) ||
       !cargarTextura(&sprCalamar[0], render, RUTA_CALAMAR) ||
       !cargarTextura(&sprCalamar[1], render, RUTA_CALAMAR_ANIM) ||
       !cargarTextura(&sprCangrejo[0], render, RUTA_CANGREJO) ||
       !cargarTextura(&sprCangrejo[1], render, RUTA_CANGREJO_ANIM) ||
       !cargarTextura(&sprPulpo[0], render, RUTA_PULPO) ||
       !cargarTextura(&sprPulpo[1], render, RUTA_PULPO_ANIM) ||
       !cargarTextura(&sprEnemigoEliminado, render, RUTA_ELIMINADO))
        return false;

    if(!cargarSonidos())
        return false;

    fntJuego = fuenteCargar(RUTA_FUENTE, TAMANIO_FUENTE);
    if(!fntJuego)
        return false;

    return true;
}

void recursos_liberar(void)
{
    int i;

    spriteLiberar(&sprFondo);
    spriteLiberar(&sprJugador);
    spriteLiberar(&sprEscudo);

    spriteLiberar(&sprCalamar[0]);
    spriteLiberar(&sprCalamar[1]);
    spriteLiberar(&sprCangrejo[0]);
    spriteLiberar(&sprCangrejo[1]);
    spriteLiberar(&sprPulpo[0]);
    spriteLiberar(&sprPulpo[1]);
    spriteLiberar(&sprEnemigoEliminado);

    liberarSonido(&sndDisparoJugador);
    liberarSonido(&sndDisparoEnemigo);
    liberarSonido(&sndFinalNivel);
    for(i = 0; i < CANT_PASOS; i++)
        liberarSonido(&sndNaveBajando[i]);

    fuenteLiberar(fntJuego);
    fntJuego = NULL;
}

/* =========================================================
 * Acceso a los sprites
 * ========================================================= */
const tSprite *recursos_fondo(void)
{
    return &sprFondo;
}

const tSprite *recursos_jugador(void)
{
    return &sprJugador;
}


const tSprite *recursos_escudo(void)
{
    return &sprEscudo;
}

const tSprite *recursos_calamar(int frame)
{
    return &sprCalamar[frame % 2];
}

const tSprite *recursos_cangrejo(int frame)
{
    return &sprCangrejo[frame % 2];
}

const tSprite *recursos_pulpo(int frame)
{
    return &sprPulpo[frame % 2];
}

const tSprite *recursos_enemigo_eliminado(void)
{
    return &sprEnemigoEliminado;
}


/* =========================================================
 * Acceso a los sonidos y a la fuente
 * ========================================================= */
tSonido *recursos_disparo_jugador(void)
{
    return sndDisparoJugador;
}

tSonido *recursos_disparo_enemigo(void)
{
    return sndDisparoEnemigo;
}

tSonido *recursos_final_nivel(void)
{
    return sndFinalNivel;
}

tSonido *recursos_nave_bajando(int paso)
{
    return sndNaveBajando[paso % CANT_PASOS];
}

tFuente *recursos_fuente(void)
{
    return fntJuego;
}
