#include <vector>
#include <string>
#include <set>
#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool loadObjMesh(const std::string &path,
                 std::vector<Vec3> &vertices,
                 std::vector<Edge> &edges,
                 std::vector<Face> &faces)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Cannot open OBJ file!\n";
        return false;
    }

    vertices.clear();
    edges.clear();
    faces.clear();

    std::set<std::pair<int, int>> edgeSet;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices.push_back({x, y, z});
        }
        else if (type == "f") {
            std::vector<int> faceIndices;
            std::string vtx;

            while (ss >> vtx) {
                std::stringstream vs(vtx);
                std::string indexStr;
                std::getline(vs, indexStr, '/');   
                int idx = std::stoi(indexStr);
                faceIndices.push_back(idx - 1);    
            }

            int n = (int)faceIndices.size();
            if (n < 3) continue;

            // 1) рёбра
            for (int i = 0; i < n; ++i) {
                int a = faceIndices[i];
                int b = faceIndices[(i + 1) % n];
                if (a > b) std::swap(a, b);

                if (edgeSet.insert({a, b}).second) {
                    edges.push_back({a, b});
                }
            }

            // 2) треугольники
            for (int i = 1; i + 1 < n; ++i) {
                Face f;
                f.a = faceIndices[0];
                f.b = faceIndices[i];
                f.c = faceIndices[i + 1];
                faces.push_back(f);
            }
        }
    }

    return true;
}

bool loadObjWireFrame(const std::string &path,
                      std::vector<Vec3> &vertices,
                      std::vector<Edge> &edges)
{
    std::vector<Face> dummy;
    return loadObjMesh(path, vertices, edges, dummy);
}
