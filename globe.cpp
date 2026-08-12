#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "globe.h"

void frame_buffer::clear(uint8_t r, uint8_t g, uint8_t b)
{
    for(size_t i = 0; i < pixels.size(); i+= 3)
    {
        pixels[i] = r; 
        pixels[i+1] = g; 
        pixels[i+2] = b;
    }
}

void frame_buffer::setPixel(int x,int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    size_t idx = (static_cast<size_t>(y) * width + x) * 3;
    pixels[idx] = r; 
    pixels[idx + 1] = g; 
    pixels[idx + 2] = b;
}


Globe::Globe()
{
    std::cout << "Globe constructor called" << std::endl;
}

vector3 Globe::rotate(const vector3& vec, double yawRad, double pitchRad)
{
    double cpr = cos(pitchRad);
    double spr = sin(pitchRad);

    double cyr = cos(yawRad);
    double syr = sin(yawRad);

    double x = vec.x  * cyr + vec.z  * syr;
    double y1 = vec.y;
    double z1 = -vec.x * syr + vec.z * cyr;

    double y2 = y1 * cpr - z1 * spr;
    double z2 = y1 * spr + z1 * cpr;

    return vector3(x, y2, z2);
}

//this is currently standing in for sampling a real equirectangular texture.
bool Globe::isLand(const vector3& unitPos) {
    double n = std::sin(unitPos.x * 4.7 + unitPos.y * 1.3) +
               std::sin(unitPos.y * 6.1 - unitPos.z * 2.9) +
               std::sin(unitPos.z * 5.3 + unitPos.x * 3.7) +
               std::sin((unitPos.x + unitPos.y + unitPos.z) * 8.0);
    return n > 0.6;
}

std::vector<Triangle> Globe::buildFrame(const Icosphere & sphere, double yaw, double pitch, const vector3 & sun_direction, double radiusPx, double centerX, double centerY)
{
    std::vector<Triangle> output;

    for(const face& i : sphere.faces)
    {
        const vector3& a = sphere.vertices[i.a];
        const vector3& b = sphere.vertices[i.b];
        const vector3& c = sphere.vertices[i.c];

        vector3 ra = rotate(a, yaw, pitch);
        vector3 rb = rotate(b, yaw, pitch);
        vector3 rc = rotate(c, yaw, pitch);

        vector3 normal = (ra, rb, rc) * (1.0 / 3.0);

        if(normal.z <= 0.0) continue;   //if camera is pointing directly at the origin

        double lit  = std::max(0.0, normal.dot(sun_direction));
        double brightness = 0.12 + 0.88 * lit;

        vector3 original_center = (a + b + c) * (1.0/3.0);
        bool land = isLand(original_center.normalized());

        uint8_t br = land ? 60 : 20;
        uint8_t bg = land ? 110 : 70;
        uint8_t bb = land ? 55 : 160;

        Triangle tri;
        tri.p[0] = {centerX + ra.x * radiusPx, centerY - ra.y * radiusPx};
        tri.p[1] = {centerX + rb.x * radiusPx, centerY - rb.y * radiusPx};
        tri.p[2] = {centerX + rc.x * radiusPx, centerY - rc.y * radiusPx};

        tri.r = static_cast<uint8_t>(std::min(255.0, br * brightness));
        tri.g = static_cast<uint8_t>(std::min(255.0, bg * brightness));
        tri.b = static_cast<uint8_t>(std::min(255.0, bb * brightness));

        output.push_back(tri);
    }
    return output;
    
}

void Globe::rasterizeTriangle(frame_buffer& fb, const Triangle& tri)
{
    double minX = std::min({tri.p[0].x,tri.p[1].x,tri.p[2].x});
    double maxX = std::max({tri.p[0].x,tri.p[1].x,tri.p[2].x});
    double minY = std::min({tri.p[0].y,tri.p[1].y,tri.p[2].y});
    double maxY = std::max({tri.p[0].y,tri.p[1].y,tri.p[2].y});

    int x0 = std::max(0, static_cast<int>(std::floor(minX)));
    int x1 = std::min(fb.width-1, static_cast<int>(std::ceil(maxX)));

    int y0 = std::max(0, static_cast<int>(std::floor(minY)));
    int y1 = std::min(fb.width-1, static_cast<int>(std::ceil(maxY)));

    double x1_ = tri.p[0].x, y1_ = tri.p[0].y;
    double x2_ = tri.p[1].x, y2_ = tri.p[1].y;
    double x3_ = tri.p[2].x, y3_ = tri.p[2].y;

    double denominator = (y2_ - y3_) * (x1_ - x3_) + (x3_ - x2_) * (y1_ - y3_);
    if(std::fabs(denominator) < 1e-9) return;

    for(int y = y0; y <= y1; ++y)
    {
        for(int x = x0; x <= x1; ++x)
        {
            double px = x + 0.5, py = y + 0.5;
            double a = ((y2_ - y3_) * (px - x3_) + (x3_ - x2_) * (py - y3_)) / denominator;
            double b = ((y3_ - y1_) * (px - x3_) + (x1_ - x3_) * (py - y3_)) / denominator;
            double c = 1.0 - a - b;
            if(a >= 0 && b >= 0 && c >= 0) fb.setPixel(x,y, tri.r,tri.g,tri.b);
        }
    }

    /*
        double calculateWeight(vector2 coord1, vector2 coord2, vector2 coord3, vector2 point)
    {
        double weight = ((coord2.y - coord3.y) * (point.x - coord3.x) + (coord3.x - coord2.x) * (point.y - coord3.y))/

                        ((coord2.y - coord3.y) * (coord1.x - coord3.x) + (coord3.x - coord2.x) * (coord1.y - coord3.y));
        return weight;
    }
        */
}

void Globe::renderGlobe(frame_buffer& fb, const Icosphere& sphere, double yaw, double pitch,const vector3 & sun_direction, double radiusPx, double centerX, double centerY)
{
    auto tris = buildFrame(sphere, yaw, pitch, sun_direction, radiusPx, centerX, centerY);
    for(const auto& tri : tris) rasterizeTriangle(fb, tri);
}