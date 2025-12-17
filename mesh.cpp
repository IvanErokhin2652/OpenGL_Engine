#include "mesh.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include "matrix.h"
#include "state.h"
#include <GLFW/glfw3.h>

struct Point2D {
    float x, y;
};

static float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 add(const Vec3& a, const Vec3& b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static float length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

static Vec3 normalize(const Vec3& v) {
    float len = length(v);
    if (len == 0.0f) {
        return {0.0f, 0.0f, 0.0f};
    }
    return { v.x / len, v.y / len, v.z / len };
}

void DrawMesh(const std::vector<Vec3> &vertices,
              const std::vector<Edge> &edges,
              const std::vector<Face> &faces)
{
    if (vertices.empty()) return;

    // экранные координаты и глубина
    std::vector<Point2D> screenPoints(vertices.size());
    std::vector<float>   screenDepth(vertices.size());
    std::vector<Vec3>    worldVerts(vertices.size()); 

    // центр объекта
    float pivot[3] = {0, 0, 0};
    for (const auto& v : vertices) {
        pivot[0] += v.x;
        pivot[1] += v.y;
        pivot[2] += v.z;
    }
    pivot[0] /= vertices.size();
    pivot[1] /= vertices.size();
    pivot[2] /= vertices.size();

    // матрицы поворота
    float R_obj[9], R_view[9];
    make_r((float)objAx, (float)objAy, (float)objAz, R_obj);
    make_r((float)-ax,   (float)-ay,   (float)-az,   R_view);

    double scale = 15.0;

    // трансформация всех вершин
    for (size_t i = 0; i < vertices.size(); ++i) {
        // локально относительно центра
        float local[3] = {
            vertices[i].x - pivot[0],
            vertices[i].y - pivot[1],
            vertices[i].z - pivot[2]
        };

        // поворот объекта
        float rotated[3];
        matr_vec_mult(R_obj, local, rotated);

        // мировые координаты
        float v_obj[3] = {
            rotated[0] + pivot[0],
            rotated[1] + pivot[1],
            rotated[2] + pivot[2]
        };
        worldVerts[i] = { v_obj[0], v_obj[1], v_obj[2] };

        // относительно камеры
        float v_rel[3] = {
            v_obj[0] - camX,
            v_obj[1] - camY,
            v_obj[2] - camZ
        };

        // поворот камеры
        float v_cam[3];
        matr_vec_mult(R_view, v_rel, v_cam);

        // глубина (0 -> 1)
        float zNear = 0.1f, zFar = 100.0f;
        float depth = (-v_cam[2] - zNear) / (zFar - zNear);
        if (depth < 0.0f) depth = 0.0f;
        if (depth > 1.0f) depth = 1.0f;
        screenDepth[i] = depth;

        // проекция + в экран
        float point2d[2];
        proj_iso(v_cam, point2d);
        to_screen(H, W, point2d[0], point2d[1], scale,
                  &screenPoints[i].x, &screenPoints[i].y);
    }

    // нормали вершин по граням
    std::vector<Vec3> normals(vertices.size(), {0.0f, 0.0f, 0.0f});

    for (const auto& f : faces) {
        Vec3 A = worldVerts[f.a];
        Vec3 B = worldVerts[f.b];
        Vec3 C = worldVerts[f.c];

        Vec3 AB = { B.x - A.x, B.y - A.y, B.z - A.z };
        Vec3 AC = { C.x - A.x, C.y - A.y, C.z - A.z };

        Vec3 N = normalize(cross(AB, AC));
        normals[f.a] = add(normals[f.a], N);
        normals[f.b] = add(normals[f.b], N);
        normals[f.c] = add(normals[f.c], N);
    }

    for (auto& n : normals) {
        n = normalize(n);
    }

    Vec3 lightDir = normalize({1.0f, 1.0f, 0.5f});

    glEnable(GL_DEPTH_TEST);
    glBegin(GL_TRIANGLES);
    for (const auto& f : faces) {
        int idx[3] = { f.a, f.b, f.c };
        for (int k = 0; k < 3; ++k) {
            int i = idx[k];
            float I = std::max(0.0f, dot(normals[i], lightDir));
            glColor3f(I, I, I);
            glVertex3f(screenPoints[i].x,
                       screenPoints[i].y,
                       screenDepth[i]);
        }
    }
    glEnd();

    glLineWidth(ld);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for (const auto& e : edges) {
        int a = e.a;
        int b = e.b;

        glVertex3f(screenPoints[a].x,
                   screenPoints[a].y,
                   screenDepth[a]);

        glVertex3f(screenPoints[b].x,
                   screenPoints[b].y,
                   screenDepth[b]);
    }
    glEnd();
}