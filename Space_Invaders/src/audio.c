#include "audio.h"

static Mix_Music *musica = NULL;

bool audioCrear(void)
{
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        return false;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        SDL_Log("Mix_OpenAudio: %s", Mix_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    Mix_AllocateChannels(16);
    return true;
}

void audioDestruir(void)
{
    musicaDetener();
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

tSonido* sonidoCargar(const char *ruta)
{
    tSonido *s = Mix_LoadWAV(ruta);

    if(s == NULL)
        SDL_Log("No se pudo cargar %s: %s", ruta, Mix_GetError());

    return s;
}

void sonidoReproducir(tSonido *s)
{
    if(s)
        Mix_PlayChannel(-1, s, 0); //-1 = primer canal libre
}

void sonidoLiberar(tSonido *s)
{
    if(s)
        Mix_FreeChunk(s);
}

bool musicaReproducir(const char *ruta, bool bucle)
{
    musicaDetener();

    musica = Mix_LoadMUS(ruta);
    if(musica == NULL)
    {
        SDL_Log("No se pudo cargar %s: %s", ruta, Mix_GetError());
        return false;
    }

    return Mix_PlayMusic(musica, bucle ? -1 : 1) == 0;
}

void musicaDetener(void)
{
    if(musica)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(musica);
        musica = NULL;
    }
}

void audioVolumen(int volumen)
{
    Mix_Volume(-1, volumen);
    Mix_VolumeMusic(volumen);
}
