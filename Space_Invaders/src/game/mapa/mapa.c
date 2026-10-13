#include <SDL.h>
#include "mapa.h"
#include "../game.h"
#include "../jugador/jugador.h"
#include "../recursos/recursos.h"
#include "../../engine/sprites.h"

#define FILAS_ENEMIGOS 5
#define COLUMNAS_ENEMIGOS 11
#define INICIO_X 20
#define INICIO_Y 30
#define SEPARACION_X 20
#define SEPARACION_Y 25
#define ENEMIGO_ANCHO 12
#define ENEMIGO_ALTO 8
#define CANT_ESCUDOS 4
#define ESCUDOS_ANCHO 35
#define ESCUDOS_ALTO 35
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

tEnemigos enemigos [FILAS_ENEMIGOS][COLUMNAS_ENEMIGOS];
tEscudos escudos [CANT_ESCUDOS];
static tSprite sprCalamarVerde = {0};
static tSprite sprCangrejoVerde = {0};
static tSprite sprCangrejoTurquesa = {0};
static tSprite sprPulpoTurquesa = {0};
static tSprite sprPulpoMorado = {0};
static tSprite sprEscudo = {0};

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
    if(!spriteCargar(&sprCalamarVerde, render,
                     "assets/images/calamar.png", 1, 0))
        return false;

    if(!spriteCargar(&sprCangrejoVerde, render,
                     "assets/images/cangrejo.png", 1, 0))
        return false;

    if(!spriteCargar(&sprCangrejoTurquesa, render,
                     "assets/images/cangrejo.png", 1, 0))
        return false;

    if(!spriteCargar(&sprPulpoTurquesa, render,
                     "assets/images/pulpo.png", 1, 0))
        return false;

    if(!spriteCargar(&sprPulpoMorado, render,
                     "assets/images/pulpo.png", 1, 0))
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
                        //SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
                        sprite = &sprCalamarVerde;
                        SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
                        break;
                    case ENEMIGO_CANGREJO_VERDE:
                        //SDL_SetRenderDrawColor(render, 0, 180, 0, 255);
                        sprite = &sprCangrejoVerde;
                        SDL_SetTextureColorMod(sprite->textura, 0, 255, 0);
                        break;
                    case ENEMIGO_CANGREJO_TURQUESA:
                        //SDL_SetRenderDrawColor(render, 0, 255, 200, 255);
                        sprite = &sprCangrejoTurquesa;
                        SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
                        break;
                    case ENEMIGO_PULPO_TURQUESA:
                        //SDL_SetRenderDrawColor(render, 0, 255, 200, 255);
                        sprite = &sprPulpoTurquesa;
                        SDL_SetTextureColorMod(sprite->textura, 0, 255, 200);
                        break;
                    case ENEMIGO_PULPO_MORADO:
                        //SDL_SetRenderDrawColor(render, 200, 102, 238, 255);
                        sprite = &sprPulpoMorado;
                        SDL_SetTextureColorMod(sprite->textura, 200, 102, 238);
                        break;
                    default:
                        //SDL_SetRenderDrawColor(render, 128, 128, 128, 255);
                        break;
                }

                if(sprite != NULL)
                {
                    spriteDibujar(render, sprite,enemigos[fila][columna].x,enemigos[fila][columna].y,ENEMIGO_ANCHO, ENEMIGO_ALTO);
                }
            }
        }
    }
}
void mapa_enemigos_actualizar(void);
int mapa_enemigos_hay_activos(void);
void mapa_sprites_liberar(void)
{
    spriteLiberar(&sprCalamarVerde);
    spriteLiberar(&sprCangrejoVerde);
    spriteLiberar(&sprCangrejoTurquesa);
    spriteLiberar(&sprPulpoTurquesa);
    spriteLiberar(&sprPulpoMorado);
    spriteLiberar(&sprEscudo);
}
