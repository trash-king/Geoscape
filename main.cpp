#include <SDL2/SDL.h>
#include "globe.h"
#include "icosphere.h"
#include "vector3.h"

int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    
}