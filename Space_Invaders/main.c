#include "main.h"

#define WIN_W 820
#define WIN_H 920

int main(int argc, char *argv[])
{

    float delta;

    if(!motorCrear("Space Invaders - Nexus", WIN_H, WIN_W) )
        return 1;

    if(!gameCrear(motorRenderizador()))
    {
        motorDestruir();
        return 1;
    }

    while(gameEjecutando())
    {
        delta = motorIniciarFrame();

        gameProcesarEventos();
        gameActualizar(delta);

        motorLimpiar();

        gameDibujar(motorRenderizador());

        motorPresentar();
        motorEsperar();
    }

    gameDestruir();
    motorDestruir();

    return 0;
}
