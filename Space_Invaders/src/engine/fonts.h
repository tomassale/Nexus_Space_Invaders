#ifndef FONTS_H_INCLUDED
#define FONTS_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

typedef TTF_Font tFuente;

bool fuentesCrear(void);
void fuentesDestruir(void);

tFuente* fuenteCargar(const char *ruta, int tamanio);
void fuenteLiberar(tFuente *f);

void fuenteDibujar(SDL_Renderer *r, tFuente *f, const char *texto,
                   int x, int y, SDL_Color color);
int fuenteAncho(tFuente *f, const char *texto);

#endif // FONTS_H_INCLUDED
