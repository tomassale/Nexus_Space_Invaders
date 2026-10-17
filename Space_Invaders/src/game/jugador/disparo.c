#include <stdlib.h>
#include "disparos.h"
#define MINTAM 10

static void disparo_mover(tDisparo *disparo, int direccion, int velocidad)
{
    disparo->y += direccion * velocidad;
}


