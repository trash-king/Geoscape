#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>
#include "globe.h"
#include "target.h"

std::string outputPath = "D:/ProgrammingProjects/Geoscape/Globe.obj";
std::string debugpath = "D:/ProgrammingProjects/Geoscape/uvcoordsdebug.txt";
const char * background = "D:/ProgrammingProjects/Geoscape/background.bmp";

#if DISPLAY_MODE    == MODE_GEOSCAPE
    const char * worldmap = "D:/ProgrammingProjects/Geoscape/worldmap_flipped.bmp";
#elif DISPLAY_MODE  == MODE_EARTH
    const char * worldmap = "D:/ProgrammingProjects/Geoscape/earth_worldmap_flipped.bmp";
#elif DISPLAY_MODE  == MODE_DEBUG
    const char * worldmap = "D:/ProgrammingProjects/Geoscape/worldmap_flipped.bmp";
#endif

int main(int argc, char* argv[]) {
    int w = 900;
    int h = 900;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Geoscape", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL_Surface* bkg = SDL_LoadBMP(background);   
    if(bkg == nullptr)
    {
        printf("SDL Background Initialization Error: %s\n", SDL_GetError());
        return 1;
    }
    if(bkg->w == w && bkg->h == h)
    {
        printf("Background Size Matches\n");
    }else printf("Size does not match W/H: %d %d \n",bkg->w,bkg->h);
    
    SDL_Surface* world = SDL_LoadBMP(worldmap);
    if(worldmap == nullptr)
    {
        printf("SDL World Map Initialization Error: %s\n", SDL_GetError());
        return 1;
    }

    Icosphere icosphere = Icosphere(3);
    Globe globe;
    vector3 sun_direction = vector3(1.0, 0.25, 0.3).normalized();
    

    double yaw = 0.0, pitch = 0.15;
    bool dragging = false;
    bool hasMoved = false;
    int drag       = 3;
    int lastMouseX = 0;
    int lastMouseY = 0;
    int mouseDownX = 0;
    int mouseDownY = 0;

    frame_buffer fb(w, h);
    fb.background(bkg,w,h);
    globe.setTexture(world);
    bool running = true;
    Uint32 lastTick = SDL_GetTicks();

    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) running = false;
            else if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            {
                dragging = true;
                mouseDownX = e.button.x;
                mouseDownY = e.button.y;
                lastMouseX = e.button.x;
                lastMouseY = e.button.y;
                hasMoved = false;
            }
            else if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
            {
                dragging = false;
            }
            else if(e.type == SDL_MOUSEMOTION && dragging)
            {
                int dx = e.motion.x - lastMouseX;
                int dy = e.motion.y - lastMouseY;

                yaw += dx * 0.01;
                pitch += dy * 0.01;
                pitch = g_clamp(pitch, -1.4, 1.4);
                lastMouseX = e.motion.x;
                lastMouseY = e.motion.y;

                int totalDX = dx - mouseDownX;
                int totalDY = dy - mouseDownY;

                if(totalDX * totalDX + totalDY * totalDY > drag * drag)
                {
                    hasMoved = true;
                }
            }
            else if(e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
            {
                dragging = false;
                if(!hasMoved)
                {
                    int dx = e.button.x;
                    int dy = e.button.y;
                    yaw += dx * 0.01;
                    pitch += dy * 0.01;
                    pitch = g_clamp(pitch, -1.4, 1.4);                    
                }
            }
            else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                running = false;
            }
        }

        Uint32 now = SDL_GetTicks();
        double dt =(now - lastTick) / 1000.0;
        lastTick = now;
        if(!dragging) yaw += dt * 0.15;

        fb.pixels = fb.backgroundpixels;
        globe.renderGlobe(fb,icosphere,yaw,pitch,sun_direction, w * 0.42, w/2, h/2);

        SDL_UpdateTexture(texture, nullptr, fb.pixels.data(), w * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);  

    }
#if 0 // debug    
    icosphere.debugPrintUVCoords(debugpath);
    icosphere.debugPrintFaces();
    icosphere.debugPrintVertices();
    icosphere.exportAsOBJ(outputPath);
#endif
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
};