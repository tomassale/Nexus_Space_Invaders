#include <SDL.h>
#include <stdbool.h>

#include "recursos.h"
#include "sprites.h"

/* Rutas de todas las imagenes del juego */
#define RUTA_FONDO          "assets/images/fondo.jpg"
#define RUTA_JUGADOR        "assets/images/jugador.png"
#define RUTA_MENU           "assets/images/menu.png"
#define RUTA_ESCUDO         "assets/images/escudo.png"

#define RUTA_CALAMAR        "assets/images/calamar.png"
#define RUTA_CALAMAR_ANIM   "assets/images/calamar_anim.png"
#define RUTA_CANGREJO       "assets/images/cangrejo.png"
#define RUTA_CANGREJO_ANIM  "assets/images/cangrejo_anim.png"
#define RUTA_PULPO          "assets/images/pulpo.png"
#define RUTA_PULPO_ANIM     "assets/images/pulpo_anim.png"
#define RUTA_ELIMINADO      "assets/images/eliminado.png"

/* =========================================================
 * Texturas del juego
 * ========================================================= */
static tSprite sprFondo = {0};
static tSprite sprJugador = {0};
static tSprite sprMenu = {0};
static tSprite sprEscudo = {0};

static tSprite sprCalamar[2] = {0};
static tSprite sprCangrejo[2] = {0};
static tSprite sprPulpo[2] = {0};
static tSprite sprEnemigoEliminado = {0};

/* Carga UNA textura (siempre de un solo frame) */
static bool cargarTextura(tSprite *s, SDL_Renderer *render, const char *ruta)
{
    return spriteCargar(s, render, ruta, 1, 0);
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

    /* La portada es opcional: si no esta, el menu usa el fondo */
    cargarTextura(&sprMenu, render, RUTA_MENU);

    return true;
}

void recursos_liberar(void)
{
    spriteLiberar(&sprFondo);
    spriteLiberar(&sprJugador);
    spriteLiberar(&sprMenu);
    spriteLiberar(&sprEscudo);

    spriteLiberar(&sprCalamar[0]);
    spriteLiberar(&sprCalamar[1]);
    spriteLiberar(&sprCangrejo[0]);
    spriteLiberar(&sprCangrejo[1]);
    spriteLiberar(&sprPulpo[0]);
    spriteLiberar(&sprPulpo[1]);
    spriteLiberar(&sprEnemigoEliminado);
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

const tSprite *recursos_menu(void)
{
    return &sprMenu;
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
