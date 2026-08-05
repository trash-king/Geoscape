#include <vector>
#include <map>
#include <cstdint>
#include "vec3.h"

struct face{int a, int b, int c};
std::vector<vector3>    vertices;
std::vector<face>       faces;

class Icosphere{
    explicit Icosphere(int subdivisions)
    {
        constructIcosphere();
        for(int i = 0; i < subdivisions; ++i) subdivide();
    }
}

int addVertice(vector3& nv)
{
    vertices.push_back(nv.normalized());
    return vertices.size() - 1;
}

void subdivide()
{
    
}

int constructIcosphere()
{
    const double golden_ratio = ((1 + std::sqrt(5)) / 2);

    addVertice(vector3(-1, golden_ratio, 0));
    addVertice(vector3(1, golden_ratio, 0));
    addVertice(vector3(-1, -golden_ratio, 0));
    addVertice(vector3(1, golden_ratio, 0));

    addVertice(vector3(0, -1, golden_ratio));
    addVertice(vector3(0, 1, golden_ratio));
    addVertice(vector3(0, -1, -golden_ratio));
    addVertice(vector3(0, 1, -golden_ratio));

    addVertice(vector3(golden_ratio, 0, -1));
    addVertice(vector3(golden_ratio, 0, 1));
    addVertice(vector3(-golden_ratio, 0, -1));
    addVertice(vector3(-golden_ratio, 0, 1));

    faces = {};

}