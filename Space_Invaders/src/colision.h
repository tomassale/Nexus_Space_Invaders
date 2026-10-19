#ifndef COLISION_H_INCLUDED
#define COLISION_H_INCLUDED

#include <stdbool.h>

/* Test unico de colision: se superponen dos rectangulos? (x, y, ancho, alto)
   Se define en el header (static inline) para poder usarlo desde varios
   modulos sin duplicar la cuenta ni agregar otro .c al build. */
static inline bool hay_choque(int x1, int y1, int ancho1, int alto1,
                              int x2, int y2, int ancho2, int alto2)
{
    return x1 < x2 + ancho2 &&
           x1 + ancho1 > x2 &&
           y1 < y2 + alto2 &&
           y1 + alto1 > y2;
}

#endif // COLISION_H_INCLUDED
