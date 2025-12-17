#ifndef OBJ_H_
#define OBJ_H_

#include <string>
#include <vector>
#include "mesh.h"


bool loadObjWireFrame(const std::string &path,
                      std::vector<Vec3> &vertices,
                      std::vector<Edge> &edges);

bool loadObjMesh(const std::string &path,
                 std::vector<Vec3> &vertices,
                 std::vector<Edge> &edges,
                 std::vector<Face> &faces);

#endif /* OBJ_H_ */
