#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "globe.h"

frame_buffer::frame_buffer(int w, int h)
{
    width = w;
    height = h;
}

void frame_buffer::clear(uint8_t r, uint8_t g, uint8_t b)
{
    return;
}

void frame_buffer::setPixel(int x,int y, uint8_t r, uint8_t g, uint8_t b)
{
    return;
}


Globe::Globe()
{
    std::cout << "Globe constructor called" << std::endl;
}

vector3 Globe::rotate(const vector3& vec, double yawRad, double pitchRad)
{
    vector3 temp;
    temp.x = 1;
    temp.y = 2;
    temp.z = 3;
    return temp;
}

bool isLand(const vector3& unitPos)
{
    return false;
}

std::vector<Triangle> buildFrame(const Icosphere & sphere, double yaw, double pitch,
    const vector3 & sun_direction, double radiusPx, double centerX, double centerY)
{
    std::vector<Triangle> temp = {};
    return temp;
}

void rasterizerTriangle(frame_buffer& fb, const Triangle& tri)
{
    /*
        double calculateWeight(vector2 coord1, vector2 coord2, vector2 coord3, vector2 point)
    {
        double weight = ((coord2.y - coord3.y) * (point.x - coord3.x) + (coord3.x - coord2.x) * (point.y - coord3.y))/

                        ((coord2.y - coord3.y) * (coord1.x - coord3.x) + (coord3.x - coord2.x) * (coord1.y - coord3.y));
        return weight;
    }
        */
    return;
}

void renderGlobe(frame_buffer& fb, const Icosphere& sphere, double yaw, double pitch,
    const vector3 & sun_direction, double radiusPx, double centerX, double centerY)
{
    return;
}