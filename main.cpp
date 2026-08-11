#include <SDL2/SDL.h>
#include <iostream>
#include "globe.h"

std::string outputPath = "D:/ProgrammingProjects/Geoscape/Globe.obj";

int main(int argc, char* argv[]) {
    int w = 900;
    int h = 900;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    Icosphere icosphere = Icosphere(1);

    icosphere.debugPrintFaces();
    icosphere.debugPrintVertices();
    icosphere.exportAsOBJ(outputPath);

    return 0;
};