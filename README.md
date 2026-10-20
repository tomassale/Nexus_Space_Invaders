# Nexus — Space Invaders

Versión del clásico *Space Invaders* escrita en C con SDL2, desarrollada para la materia **Tópicos de Programación**.

El juego corre en una resolución lógica de 256×240 (la del arcade), que SDL escala al tamaño de la ventana.

## Cómo se juega

| Tecla | Acción |
|---|---|
| `Espacio` | Empezar la partida (en el menú) / disparar |
| `←` `→` | Mover la nave |
| `Esc` | Pausar / reanudar |
| `R` | Reiniciar después de una derrota o una victoria |

### Reglas

- Arrancás con **3 vidas**. Cada **1000 puntos** ganás una vida extra.
- La formación tiene **5 filas × 11 columnas**. Cada enemigo eliminado suma según su tipo:

  | Enemigo | Puntos |
  |---|---|
  | Calamar | 30 |
  | Cangrejo | 20 |
  | Pulpo | 10 |

- La formación se mueve de costado y baja al tocar un borde. Cuantos menos enemigos quedan, **más rápido se mueve**. A medida que baja cambia de color: amarillo al acercarse y rojo al llegar a los escudos.
- Solo puede haber **una bala del jugador** y **una bala enemiga** en pantalla a la vez. Si las dos chocan, se destruyen entre sí.
- Hay **4 escudos** que se rompen de a partes: cada bala rompe un pedazo y los enemigos destruyen lo que tocan al bajar.
- Si te pega una bala perdés una vida y la nave vuelve a la posición inicial. Si un enemigo llega a la nave, la partida termina.
- Se gana superando **2 rondas**.

## Requisitos

- Windows de 64 bits.
- [Code::Blocks](https://www.codeblocks.org/) con un compilador **MinGW-w64 GCC de 64 bits** (el proyecto se compila con GCC 14.2).

Las librerías vienen incluidas en el repositorio, así que no hace falta instalar nada más:

| Librería | Versión | Ubicación |
|---|---|---|
| SDL2 | 2.32.4 | `SDL_dev_lib/` |
| SDL2_image | 2.0.5 | `Space_Invaders/sdl_image/` |
| SDL2_mixer | 2.0.4 | `Space_Invaders/sdl_mixer/` |
| SDL2_ttf | 2.0.12 | `Space_Invaders/sdl_ttf/` |

## Compilar y ejecutar

1. Abrir `Space_Invaders/Space_Invaders.cbp` en Code::Blocks.
2. Elegir el target **Debug** o **Release**.
3. *Build and run* (`F9`).

El juego carga los assets con rutas relativas (`assets/...`) y necesita las DLL de SDL que están en `Space_Invaders/`, por lo que **el directorio de trabajo tiene que ser `Space_Invaders/`**. Code::Blocks lo usa por defecto. Para correr el `.exe` por fuera del IDE, ejecutalo desde esa carpeta o copiá al lado del ejecutable la carpeta `assets/` y todas las DLL.

## Estructura del proyecto

```
Nexus/
├── SDL_dev_lib/              SDL2 (headers, librerías y DLL)
└── Space_Invaders/
    ├── Space_Invaders.cbp    proyecto de Code::Blocks
    ├── main.c                ciclo principal: eventos → actualizar → dibujar
    ├── *.dll                 DLL de SDL necesarias para ejecutar
    ├── sdl_image/ sdl_mixer/ sdl_ttf/
    ├── assets/
    │   ├── audio/            efectos y música
    │   ├── fonts/            fuente del HUD
    │   └── images/           sprites y fondo
    └── src/
        ├── motor.c/h         ventana, renderizador, resolución lógica y control de FPS (60)
        ├── audio.c/h         envoltorio de SDL_mixer (sonidos y música)
        ├── fonts.c/h         envoltorio de SDL_ttf
        ├── sprites.c/h       carga y dibujado de texturas
        ├── recursos.c/h      carga y acceso a todas las imágenes, sonidos y fuentes del juego
        ├── game.c/h          estados del juego, reglas, colisiones y HUD
        ├── jugador.c/h       nave del jugador
        ├── bala.c/h          balas del jugador y de los enemigos
        ├── enemigo.c/h       formación de enemigos: movimiento, disparo y dibujado
        ├── escudo.c/h        escudos destructibles por partes
        ├── colision.h        test de superposición de rectángulos
        └── vector.c/h        TDA vector genérico usado para todas las colecciones
```

### Organización del código

- **`game`** es dueño de los datos de la partida: jugador, enemigos, balas y escudos. Los guarda en `Vector`s y se los pasa a cada módulo, que solo sabe crearlos y trabajar con ellos.
- **`escudo`** y **`enemigo`** no se conocen entre sí: cuando la formación rompe un escudo, la interacción la coordina `game`.
- **`recursos`** concentra las rutas de los assets. Los demás módulos piden el sprite, el sonido o la fuente que necesitan, sin saber de dónde se cargan.
- **`motor`**, **`audio`**, **`fonts`** y **`sprites`** son la única capa que habla directamente con SDL para inicializar subsistemas y cargar recursos.

## Limitaciones conocidas

- El HUD muestra la etiqueta `HIGH SCORE`, pero el valor es el puntaje de la partida actual: no se guarda un récord entre partidas.
