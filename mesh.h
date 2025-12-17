#ifndef MESH_H_
#define MESH_H_

#include <vector>

struct Vec3 { 
    float x;
    float y;
    float z;
};

struct Edge {
    int a;
    int b;
};

struct Face { 
    int a;
    int b;
    int c;
};

void DrawMesh(const std::vector<Vec3> &vertices,
              const std::vector<Edge> &edges,
              const std::vector<Face> &faces);

#endif /* MESH_H_ */