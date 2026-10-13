#ifndef MOTOR_H_INCLUDED
#define MOTOR_H_INCLUDED

#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "audio.h"
#include "fonts.h"

bool motorCrear(const char* titulo, int an, int al);
void motorLimpiar(void);
void motorPresentar(void);
void motorDestruir(void);
SDL_Renderer* motorRenderizador(void);
int motorAlto(void);
int motorAncho(void);
float motorIniciarFrame(void);
float motorDelta(void);
void motorEsperar(void);

#endif // MOTOR_H_INCLUDED
