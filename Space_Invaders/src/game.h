#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <SDL.h>

bool gameCrear(SDL_Renderer *render);
void gameProcesarEventos(void);
void gameActualizar(float delta);
void gameDibujar(SDL_Renderer *render);
bool gameEjecutando(void);
void gameDestruir(void);

#endif
