#pragma once
#include <map>
#include <vector>
#include "vector3.h"

struct face{int a, b, c;};

class Icosphere{
    public:
    explicit Icosphere(int subdivisions);
    void debugPrintVertices();
    void debugPrintFaces();

    std::vector<vector3>    vertices;
    std::vector<face>       faces;

    protected:

    private:
    static uint64_t edgeKey(int a, int b);
    int addVertice(const vector3& nv);
    void generateFaces();
    void fixWinding(std::vector<face>& faces, const std::vector<vector3>& verts);
    void subdivide();
    int getMidpoint(int i1, int i2);
    void constructIcosphere();

    std::map<uint64_t, int> midpoints; 
};