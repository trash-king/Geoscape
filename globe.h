#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include "vector3.h"
#include "icosphere.h"

struct frame_buffer{
    int width;
    int height;
    std::vector<uint8_t> pixels;

    frame_buffer(int w, int h);

    void clear(uint8_t r, uint8_t g, uint8_t b);
    void setPixel(int x,int y, uint8_t r, uint8_t g, uint8_t, b);
};

struct vector2{double x,y};

vector3 rotate(const vector3& vec, double yawRad, double pitchRad)
{

}

bool isLand(const vector3& unitPos)
{

}

struct Triangle{
    vector2 p[3];
    uint8_t r, g, b;
};

std::vector<Triangle> buildFrame(const IcoSphere & sphere, double yaw, double pitch
    const vector3 & sun_direction, double radiusPx, double centerX, double centerY);


void rasterizerTriangle(frame_buffer& fb, const Triangle& tri);

void renderGlobe(frame_buffer& fb, const IcoSphere& sphere, double yaw, double pitch,
    const vector3 & sun_direction, double radiusPx, double centerX, double centerY);

