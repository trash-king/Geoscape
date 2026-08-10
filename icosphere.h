#include <vector>
#include <map>
#include <cstdint>
#include <utility>
#include <iostream>
#include "vector3.h"

struct face{int a, int b, int c};


class Icosphere{
    public:
    explicit Icosphere(int subdivisions)
    {
        constructIcosphere();
        generateFaces();
        for(int i = 0; i < subdivisions; ++i) subdivide();
    }

    std::vector<vector3>    vertices;
    std::vector<face>       faces;

    void debugPrintVertices()
    {
        int len = vertices.size();
        std::cout<< "CURRENTLY " << len << " VERTICES" << std::endl; 
        for(int i = 0; i < len; i++)
        {
            std::cout << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << " ";
        }
    }

    void debugPrintFaces()
    {
        int len = faces.size();
        std::cout<< "CURRENTLY " << len << " FACES" << std::endl; 
        for(int i = 0; i < len; i++)
        {
            std::cout << faces[i].a << " " << faces[i].b << " " << faces[i].c << " ";
        }       
    }

    protected:

    private:
    std::map<uint64_t, int> midpoints; 

    static uint64_t edgeKey(int a, int b)
    {
        if(a > b) std::swap(a, b);
        return static_cast<uint64_t>(a) << 32 | static_cast<uint64_t>(b); 
    }

    int addVertice(vector3& nv)
    {
        vertices.push_back(nv.normalized());
        return vertices.size() - 1;
    }

    void generateFaces()
    {
        double mindist = INT_MAX;
        bool adjacent[12][12] = {};
        
        for(int i= 0; i < 12;i++)
        {
            for(int j = 0; j < 12; j++)
            {
                if(i != j)
                {
                    double d = (vertices[i] - vertices[j].length());
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

    void fixWinding(std::vector<Face>& faces, const std::vector<vector3>& verts )
    {
        vector3 a = verts[i], b = verts[j], c = verts[k];
        vector3 u = b - a;
        vector3 v = c - a;

        vector3 normal = u.cross(v);
        vector3 centroid = (a + b + c) * (1.0/3.0);

        if(normal.dot(centroid) < 0) std::swap(j,k);
    }

    void subdivide()
    {
        std::vector<face> newFaces;
        newFaces.reserve(faces.size()) * 4;
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

    int getMidpoint(int i1, int i2)
    {
        uint64_t key = edgeKey(i1, i2);
        auto it = midpoints.find(key);
        if(it != midpoints.end()) return it->second;
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

        generateFaces();

    }

};
