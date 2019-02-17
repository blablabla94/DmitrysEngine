# Dmitry's Engine

Experimental C99 cross-platform header-only 3D game engine with absolute minimum of external dependencies.

Please note, that this engine is at very early development stage and some features maybe very brittle. In the `./test` folder you can find the game that I writing using this engine.

## Features
* Header only - no need to build as separate library, just `#include "de_main.h"` so extremely fast compilation.
* C99 with full compatibility with C++.
* FBX support - both ASCII and binary
* Modern rendering techniques, renderer based on OpenGL 3.3 Core.
    * Deferred shading
    * Normal mapping
* TTF fonts support (compound characters support is not implemented yet)
* GUI (inspired by WPF)
* TGA image loading
* Position-based physics
* Skeletal animation
* Built-in save/load functionality via object visitor.
* Documentation - almost every function of the engine has description in Doxygen format.

## Planned features (by priorities)
- Sound 
- Materials (probably PBR)
- More GUI widgets (there are still no text input nodes)
- Stability
- Particle systems
- Support more 3D formats (3ds for example)
- Compound character support in fonts.

## Dependencies (header only)
- miniz_tinfl - to decompress FBX compressed data.

## Code statistics
- ~17000 lines of code
- Pure C99
- Header-only

## Supported compilers
### Compiling as C
If compiling as C, you will need C99-compatible compiler:
- Windows:  Visual Studio 2013 or higher, mingw 4.8.1 or higher
- Linux: GCC 4.5 or higher

If you using lower version of Visual Studio, then you should compile engine as C++, because lower versions supports only C89.
### Compiling as C++
If compling as C++, you will need C++98 or higher compiler.

## Why engine called in that way?
At very beginning engine was called Dark Engine, but then I found that there is already one engine with that name (Thief game series using it). Then I started to looking at some fancy adjectives that could fit into `de` prefix that I was already using for my functions, and I haven't found anything suitable. And then I said - "fuck it, let it be like Dmitry's Engine, I'm too tired of fancy names".

## How to build?
### Windows
Required packages: None.
Options for linker: `opengl32; dsound; gdi32; dxguid`

### Linux
Required packages: `libx11-dev, mesa-common-dev, libgl1-mesa-dev, libxrandr-dev.`
Options for linker: `-lGL -lpthread -lasound -lX11 -lXrandr`

## Why C?
![Why](pics/why.png?raw=true "Why")
