#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "vector3.h"
#include "icosphere.h"

#define WRITE_TO_PIXEL_BUF          1
#define WRITE_TO_BACKGROUND_BUF     2

struct frame_buffer
{
    int width;
    int height;
    std::vector<uint8_t> pixels;
    std::vector<uint8_t> backgroundpixels;

    frame_buffer(int w, int h) : width(w), height(h), pixels(w * h * 3, 0), backgroundpixels(w * h * 3, 0) {};
    void clear(uint8_t r, uint8_t g, uint8_t b);
    void background(SDL_Surface * buf, int w, int h);
    void setPixel(int destination, int x,int y, uint8_t r, uint8_t g, uint8_t b);
};

struct vector2
{
    double x;
    double y;
};

struct Triangle
{
    vector2 p[3];
    uint8_t r, g, b;
};

class Globe
{
    public:
    Globe();
    vector3 rotate(const vector3& vec, double yawRad, double pitchRad);
    bool isLand(const vector3& unitPos);
    void rasterizeTriangle(frame_buffer& fb, const Triangle& tri);
    void renderGlobe(frame_buffer& fb, const Icosphere& sphere, double yaw, double pitch, const vector3 & sun_direction, double radiusPx, double centerX, double centerY);
    std::vector<Triangle> buildFrame(const Icosphere & sphere, double yaw, double pitch, const vector3 & sun_direction, double radiusPx, double centerX, double centerY);
    
    protected:

    private:
};

