#include "motor.h"
#define FPS 60
#define FRAME_TIME (1000 / FPS)
#define POS_X SDL_WINDOWPOS_CENTERED
#define POS_Y SDL_WINDOWPOS_CENTERED

static SDL_Window* ventana = NULL;
static SDL_Renderer* renderizador = NULL;
static Uint32 tiempo_anterior = 0;
static Uint32 inicio_frame = 0;

static int ancho = 0;
static int alto = 0;

static bool inicializarSDL(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        return false;
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Quit();
        return false;
    }

    return true;
}

static void cerrarSDL(void)
{
    IMG_Quit();
    SDL_Quit();
}

static void crearIcono(const char *ruta)
{
    SDL_Surface *icono = IMG_Load(ruta);

    if(icono)
    {
        SDL_SetWindowIcon(ventana, icono);
        SDL_FreeSurface(icono);
    }
}

static bool crearVentana(const char* titulo)
{
    ventana = SDL_CreateWindow(titulo, POS_X, POS_Y, ancho, alto, SDL_WINDOW_SHOWN);

    if(ventana == NULL)
        return false;

    crearIcono("assets/images/icon.png");

    return true;
}

static bool crearRenderizador(void)
{
    renderizador = SDL_CreateRenderer(ventana,-1,SDL_RENDERER_ACCELERATED);

    if(renderizador == NULL)
        return false;

    SDL_RenderSetLogicalSize(renderizador, VENTANA_X, VENTANA_Y);

    return true;
}

static void liberarRecursos(void)
{
    if(renderizador)
    {
        SDL_DestroyRenderer(renderizador);
        renderizador = NULL;
    }

    if(ventana)
    {
        SDL_DestroyWindow(ventana);
        ventana = NULL;
    }

    audioDestruir();
    fuentesDestruir();
    cerrarSDL();
}

bool motorCrear(const char* titulo, int an, int al)
{
    ventana = NULL;
    renderizador = NULL;
    alto =  al;
    ancho = an;

    if(!inicializarSDL())
    {
        return false;
    }

    if(!crearVentana(titulo))
    {
        cerrarSDL();
        return false;
    }

    if(!crearRenderizador())
    {
        liberarRecursos();
        return false;
    }

    if(!audioCrear())
        SDL_Log("Sin audio, el juego sigue sin sonido");

    if(!fuentesCrear())
        SDL_Log("Sin fuentes");

    tiempo_anterior=SDL_GetTicks();
    return true;
}

void motorLimpiar(void)
{
    SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
    SDL_RenderClear(renderizador);
}

void motorPresentar(void)
{
    SDL_RenderPresent(renderizador);
}

void motorDestruir(void)
{
    liberarRecursos();
}

SDL_Renderer* motorRenderizador()
{
    return renderizador;
}

int motorAlto(void)
{
    return alto;
}

int motorAncho(void)
{
    return ancho;
}

int motorAltoLogico(void)
{
    return VENTANA_Y;
}

int motorAnchoLogico(void)
{
    return VENTANA_X;
}

float motorIniciarFrame(void)
{
    Uint32 ahora = SDL_GetTicks();

    float delta = (ahora - tiempo_anterior) / 1000.0f;

    tiempo_anterior = ahora;
    inicio_frame = ahora;

    return delta;
}

float motorDelta(void)
{
    Uint32 tiempo_actual = SDL_GetTicks();

    float delta = (tiempo_actual - tiempo_anterior) / 1000.0f;

    tiempo_anterior = tiempo_actual;

    return delta;
}

void motorEsperar()
{
    Uint32 tiempo_frame = SDL_GetTicks() - inicio_frame;

    if(tiempo_frame < FRAME_TIME)
    {
        SDL_Delay(FRAME_TIME - tiempo_frame);
    }
}
