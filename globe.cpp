#include <cstdint>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "globe.h"

double g_clamp(double comp, double low, double high)
{
    if(comp < low) return low;
    if(comp > high) return high;

    return comp;
}

void frame_buffer::background(SDL_Surface * buf, int w, int h)
{
    Uint8* bytes = static_cast<Uint8*>(buf->pixels);
    for(size_t i = 0; i < (buf->h); i++)
    {
        for(size_t j = 0; j < (buf->w); j++)
        {
            uint8_t r, g, b;
            int offset = i * buf->pitch + j * buf->format->BytesPerPixel;
            Uint8* addr = bytes + offset;
            Uint32 pixel = *reinterpret_cast<Uint32*>(addr);

            SDL_GetRGB(pixel, buf->format, &r,&g,&b);
            setPixel(WRITE_TO_BACKGROUND_BUF, j,i, r, g, b);
        }
    }
    SDL_FreeSurface(buf);
}

void frame_buffer::clear(uint8_t r, uint8_t g, uint8_t b)
{
    for(size_t i = 0; i < pixels.size(); i+= 3)
    {
        pixels[i] = r; 
        pixels[i+1] = g; 
        pixels[i+2] = b;
    }
}

void frame_buffer::setPixel(int destination, int x,int y, uint8_t r, uint8_t g, uint8_t b)
{
    if(destination == WRITE_TO_PIXEL_BUF)
    {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        size_t idx = (static_cast<size_t>(y) * width + x) * 3;
        pixels[idx] = r; 
        pixels[idx + 1] = g; 
        pixels[idx + 2] = b;        
    }
    else
    {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        size_t idx = (static_cast<size_t>(y) * width + x) * 3;
        backgroundpixels[idx] = r; 
        backgroundpixels[idx + 1] = g; 
        backgroundpixels[idx + 2] = b;             
    }

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

/*
void frame_buffer::setPixel(int destination, int x,int y, uint8_t r, uint8_t g, uint8_t b)
{
    if(destination == WRITE_TO_PIXEL_BUF)
    {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        size_t idx = (static_cast<size_t>(y) * width + x) * 3;
        pixels[idx] = r; 
        pixels[idx + 1] = g; 
        pixels[idx + 2] = b;        
    }
*/

void Globe::setTexture(SDL_Surface * buf)
{
    
    texture_width = buf->w;
    texture_height = buf->h;
    texture.resize(static_cast<size_t>(buf->w) * buf->h * 3);
    printf("Resized Texture\n");

    Uint8* bytes = static_cast<Uint8*>(buf->pixels);
    for(size_t i = 0; i < (buf->h); i++)
    {
        for(size_t j = 0; j < (buf->w); j++)
        {
            uint8_t r, g, b;
            int offset = i * buf->pitch + j * buf->format->BytesPerPixel;
            Uint8* addr = bytes + offset;
            //Uint32 pixel = *reinterpret_cast<Uint32*>(addr);
            Uint32 pixel = 0;
            memcpy(&pixel, addr, buf->format->BytesPerPixel);

            SDL_GetRGB(pixel, buf->format, &r,&g,&b);

            size_t idx = (i * buf->w + j) * 3;
            texture[idx] = r;
            texture[idx+1] = g;
            texture[idx+2] = b;
        }
    }
    printf("Texture mapping complete!");
    SDL_FreeSurface(buf);
}
/*
the Calculations - 

latitude  = asin(y)              // where on the sphere, vertically
longitude = atan2(z, x)          // where on the sphere, around the pole axis
u = (longitude + π) / (2π)       // squash that into 0-1
v = 1 - (latitude + π/2) / π     // squash that into 0-1

*/

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
        double brightness = 0.05 + 0.95 * lit;

        vector3 original_center = (a + b + c) * (1.0/3.0);
        bool land = isLand(original_center.normalized());

        uint8_t br = land ? 60 : 20;
        uint8_t bg = land ? 110 : 70;
        uint8_t bb = land ? 55 : 160;

        Triangle tri;

        tri.uv[0] = sphere.uv_coords[i.a];
        tri.uv[1] = sphere.uv_coords[i.b];
        tri.uv[2] = sphere.uv_coords[i.c];

        tri.brightness = brightness;

        double maxU = std::max({tri.uv[0].x,tri.uv[1].x,tri.uv[2].x});
        double minU = std::min({tri.uv[0].x,tri.uv[1].x,tri.uv[2].x});

        if (maxU - minU > 0.5) 
        {
            for (int k = 0; k < 3; k++) 
            {
            if (tri.uv[k].x < 0.5) tri.uv[k].x += 1.0;
            }
        }

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

void Globe::mapTexture(const vector3& unitPos)
{
    
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
            if(a >= 0 && b >= 0 && c >= 0)
            {
                double pixelU = a * tri.uv[0].x + b * tri.uv[1].x + c * tri.uv[2].x;
                double pixelV = a * tri.uv[0].y + b * tri.uv[1].y + c * tri.uv[2].y;

                pixelU = pixelU - std::floor(pixelU);
                pixelV = g_clamp(pixelV, 0.0, 1.0);

                int texelX = static_cast<int>(pixelU * texture_width);
                int texelY = static_cast<int>(pixelV * texture_height);

                texelX = std::min(texelX,texture_width-1);
                texelY = std::min(texelY, texture_height-1);

                size_t idx = (static_cast<size_t>(texelY) * texture_width + texelX) * 3;
                uint8_t tr = static_cast<uint8_t>(texture[idx] * tri.brightness);
                uint8_t tg = static_cast<uint8_t>(texture[idx + 1] * tri.brightness);
                uint8_t tb = static_cast<uint8_t>(texture[idx + 2] * tri.brightness);

                fb.setPixel(WRITE_TO_PIXEL_BUF,x,y, tr, tg, tb);
            }
            //fb.setPixel(WRITE_TO_PIXEL_BUF, x,y, tri.r,tri.g,tri.b);
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