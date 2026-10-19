#include <SDL.h>
#include <stdlib.h>
#include "mapa.h"
#include "game.h"
#include "jugador.h"
#include "recursos.h"
#include "sprites.h"
#include <stdbool.h>

#define CANT_ESCUDOS 4
#define ESCUDOS_ANCHO 35
#define ESCUDOS_ALTO 35
#define ESCUDOS_INICIO_X 40
#define ESCUDOS_INICIO_Y 170
#define ESCUDOS_SEPARACION_X 50

tEscudos escudos [CANT_ESCUDOS];
static tSprite sprEscudo = {0};

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

bool mapa_sprites_cargar(SDL_Renderer *render)
{
    if(!spriteCargar(&sprEscudo, render,
                     "assets/images/escudo.png", 1, 0))
        return false;

    return true;
}

void mapa_sprites_liberar(void)
{
    spriteLiberar(&sprEscudo);
}
