#include <SDL.h>
#include "mapa.h"
#include "game.h"
#include "jugador.h"
#include "recursos.h"
#include "sprites.h"
#include <stdbool.h>

#define FILAS_ENEMIGOS 5
#define COLUMNAS_ENEMIGOS 11
#define INICIO_X 20
#define INICIO_Y 30
#define SEPARACION_X 20
#define SEPARACION_Y 15
#define ENEMIGO_ANCHO 12
#define ENEMIGO_ALTO 8
#define CANT_ESCUDOS 4
#define ESCUDOS_ANCHO 35
#define ESCUDOS_ALTO 35
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

#define VENTANA_X 256
#define VENTANA_Y 240
#define VELOCIDAD_ENEMIGOS 2
#define DESCENSO_ENEMIGOS 10
#define ALTURA_AMARILLA 150

tEnemigos enemigos [FILAS_ENEMIGOS][COLUMNAS_ENEMIGOS];
tEscudos escudos [CANT_ESCUDOS];
static tSprite sprCalamar[2] = {0};
static tSprite sprCangrejo[2] = {0};
static tSprite sprPulpo[2] = {0};
static tSprite sprEscudo = {0};
static tSprite sprEliminado = {0};
static int frameAnimacion = 0;

static int direccion = 1;

static const int tiposNivel1[FILAS_ENEMIGOS] =
{
    ENEMIGO_CALAMAR_VERDE,
    ENEMIGO_CANGREJO_VERDE,
    ENEMIGO_CANGREJO_TURQUESA,
    ENEMIGO_PULPO_TURQUESA,
    ENEMIGO_PULPO_MORADO
};

void mapa_escudos_iniciar(void)
{
    int i;

    for(i=0;i<CANT_ESCUDOS;i++)
    {
        escudos[i].x=ESCUDOS_INICIO_X + i * ESCUDOS_SEPARACION_X;
        escudos[i].y=ESCUDOS_INICIO_Y;
        escudos[i].activo=1;
    }
}

void mapa_escudos_dibujar(SDL_Renderer *render)
{
    int i;
    tSprite *sprite = NULL;

    for(i=0;i<CANT_ESCUDOS;i++)
    {
        if(escudos[i].activo)
        {
            sprite = &sprEscudo;

            spriteDibujar(render, sprite,escudos[i].x,escudos[i].y,ESCUDOS_ANCHO,ESCUDOS_ALTO);
        }
    }

}

void mapa_enemigos_iniciar(void)
{
    int fila, columna;

    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            enemigos[fila][columna].x=INICIO_X + columna * SEPARACION_X;
            enemigos[fila][columna].y=INICIO_Y + fila * SEPARACION_Y;
            enemigos[fila][columna].activo=1;
            enemigos[fila][columna].tipo=tiposNivel1[fila];
        }
    }
}

bool mapa_sprites_cargar(SDL_Renderer *render)
{
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

    if(!spriteCargar(&sprEscudo, render,
                     "assets/images/escudo.png", 1, 0))
        return false;

    return true;
}

void mapa_enemigos_dibujar(SDL_Renderer *render)
{
    int fila, columna;
    tSprite *sprite = NULL;

    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            if(enemigos[fila][columna].activo)
            {
                switch(enemigos[fila][columna].tipo)
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
                        sprite = NULL;
                        break;
                }

                if(sprite != NULL)
                {
                    spriteDibujar(render, sprite,enemigos[fila][columna].x,enemigos[fila][columna].y,ENEMIGO_ANCHO, ENEMIGO_ALTO);

                    if(enemigos[fila][columna].y >= ALTURA_AMARILLA)
                    {
                        SDL_SetTextureColorMod(sprite->textura, 255, 255, 0);
                    }

                    spriteDibujar(render, sprite,
                    enemigos[fila][columna].x,
                    enemigos[fila][columna].y,
                    ENEMIGO_ANCHO, ENEMIGO_ALTO);
                }
            }
        }
    }
}

void mapa_enemigos_mover(void)
{
    int fila, columna;
    bool llegoAlBorde = false;

    frameAnimacion = 1 - frameAnimacion;

    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            if(enemigos[fila][columna].activo)
            {
                if(enemigos[fila][columna].x + ENEMIGO_ANCHO >= VENTANA_X ||
                   enemigos[fila][columna].x <= 0)
                {
                    llegoAlBorde = true;
                }
            }
        }
    }

    if(llegoAlBorde)
    {
        direccion *= -1;
    }

    for(fila = 0; fila < FILAS_ENEMIGOS; fila++)
    {
        for(columna = 0; columna < COLUMNAS_ENEMIGOS; columna++)
        {
            if(enemigos[fila][columna].activo)
            {
                enemigos[fila][columna].x += VELOCIDAD_ENEMIGOS * direccion;

                if(llegoAlBorde)
                {
                    enemigos[fila][columna].y += DESCENSO_ENEMIGOS;
                }
            }
        }
    }
}

int mapa_enemigos_hay_activos(void)
{
    int fila, columna;
    int hay=0;

    for(fila=0;fila<FILAS_ENEMIGOS;fila++)
    {
        for(columna=0;columna<COLUMNAS_ENEMIGOS;columna++)
        {
            if(enemigos[fila][columna].activo)
                hay=1;
        }
    }

    return hay;
}

void mapa_sprites_liberar(void)
{
    spriteLiberar(&sprCalamar[0]);
    spriteLiberar(&sprCangrejo[0]);
    spriteLiberar(&sprPulpo[0]);
    spriteLiberar(&sprCalamar[1]);
    spriteLiberar(&sprCangrejo[1]);
    spriteLiberar(&sprPulpo[1]);
    spriteLiberar(&sprEliminado);
    spriteLiberar(&sprEscudo);
}
