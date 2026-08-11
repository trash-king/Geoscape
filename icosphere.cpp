#include <cstdint>
#include <iostream>
#include <climits>
#include <fstream>
#include "icosphere.h"

Icosphere::Icosphere(int subdivisions)
{
    constructIcosphere();
    for(int i = 0; i < subdivisions; ++i) subdivide();
}

void Icosphere::debugPrintVertices()
{
    int len = vertices.size();
    std::cout<< "CURRENTLY " << len << " VERTICES" << std::endl; 
    for(int i = 0; i < len; i++)
    {
        std::cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << " ";
    }
}

void Icosphere::debugPrintFaces()
{
    int len = faces.size();
    std::cout<< "CURRENTLY " << len << " FACES" << std::endl; 
    for(int i = 0; i < len; i++)
    {
        std::cout << faces[i].a << " " << faces[i].b << " " << faces[i].c << " ";
    }       
}

void Icosphere::exportAsOBJ(const std::string& path)
{
    std::ofstream objectStream(path);

    if(!objectStream.is_open()){
        std::cout << path << " failed to open!" << std::endl;
        return;
    }

    for (const vector3& v : vertices) {
        objectStream << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    for (const face& f : faces) {
        objectStream << "f " << (f.a + 1) << " " << (f.b + 1) << " " << (f.c + 1) << "\n";
    }

    objectStream.close();
    
}

uint64_t Icosphere::edgeKey(int a, int b)
{
    if(a > b) std::swap(a, b);
    return static_cast<uint64_t>(a) << 32 | static_cast<uint64_t>(b); 
}

int Icosphere::addVertice(const vector3& nv)
{
    vertices.push_back(nv.normalized());
    return vertices.size() - 1;
}

void Icosphere::generateFaces()
{
    double mindist = INT_MAX;
    bool adjacent[12][12] = {};
        
    for(int i= 0; i < 12;i++)
    {
        for(int j = 0; j < 12; j++)
        {
            if(i != j)
            {
                double d = (vertices[i] - vertices[j]).length();
                if( d < mindist) mindist = d;
            }
        }
    }

    for(int i = 0; i < 12; i++)
    {
        for(int j = 0; j < 12; j++)
        {
            if(i != j && std::fabs((vertices[i]-vertices[j]).length() - mindist) < 1e-6)
            adjacent[i][j] = true;
        }
    }

    for(int i = 0; i < 12; i++)
    {
        for(int j = i +1; j < 12; j++)
        {
            for(int k = j+1; k < 12; k++)
            {
                if(adjacent[i][j] && adjacent[j][k] && adjacent[i][k]) this->faces.push_back({i, j, k});
            }
        }
    }
    fixWinding(this->faces, this->vertices);

}

void Icosphere::fixWinding(std::vector<face>& faces, const std::vector<vector3>& verts )
{
    for(face& i: faces)
    {
        vector3 a = verts[i.a];
        vector3 b = verts[i.b];
        vector3 c = verts[i.c];

        vector3 u = (b - a);
        vector3 v = (c - a);

        vector3 normal = u.cross(v);
        vector3 centroid = (a + b + c) * (1.0 / 3.0);
        if(normal.dot(centroid) < 0) std::swap(i.b,i.c);
    }
}

    void Icosphere::subdivide()
    {
        std::vector<face> newFaces;
        newFaces.reserve(faces.size() * 4);
        midpoints.clear();

        for(const face& f: faces)
        {
            int ab = getMidpoint(f.a, f.b);
            int bc = getMidpoint(f.b, f.c);
            int ca = getMidpoint(f.c, f.a);

            newFaces.push_back({f.a, ab, ca});
            newFaces.push_back({f.b, bc, ab});
            newFaces.push_back({f.c, ca, bc});
            newFaces.push_back({ab, bc, ca});
        }
        faces = std::move(newFaces);

    }

    int Icosphere::getMidpoint(int i1, int i2)
    {
        uint64_t key = edgeKey(i1, i2);
        auto it = midpoints.find(key);
        if(it != midpoints.end()) return it->second;
        else
        {
            vector3 nv = (vertices[i1] + vertices[i2]) * 0.5;
            int idx = addVertice(nv);
            midpoints[key] = idx;
            return idx;
        }
    }

    void Icosphere::constructIcosphere()
    {
        const double golden_ratio = ((1 + std::sqrt(5)) / 2);

        addVertice(vector3(-1, golden_ratio, 0));
        addVertice(vector3(1, golden_ratio, 0));
        addVertice(vector3(-1, -golden_ratio, 0));
        addVertice(vector3(1, -golden_ratio, 0));

        addVertice(vector3(0, -1, golden_ratio));
        addVertice(vector3(0, 1, golden_ratio));
        addVertice(vector3(0, -1, -golden_ratio));
        addVertice(vector3(0, 1, -golden_ratio));

        addVertice(vector3(golden_ratio, 0, -1));
        addVertice(vector3(golden_ratio, 0, 1));
        addVertice(vector3(-golden_ratio, 0, -1));
        addVertice(vector3(-golden_ratio, 0, 1));

        generateFaces();

    }

