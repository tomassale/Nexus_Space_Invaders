#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdbool.h>

typedef Mix_Chunk tSonido;

bool audioCrear(void);
void audioDestruir(void);

tSonido* sonidoCargar(const char *ruta);
void sonidoReproducir(tSonido *s);
void sonidoLiberar(tSonido *s);

bool musicaReproducir(const char *ruta, bool bucle);
void musicaDetener(void);
void audioVolumen(int volumen); //0...128

#endif // AUDIO_H_INCLUDED
