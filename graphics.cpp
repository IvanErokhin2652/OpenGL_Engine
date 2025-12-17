#include "graphics.h"
#include "matrix.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include "state.h"


void MovingCamera(GLFWwindow* window){
    float speed = 0.05f;   // скорость полёта камеры

    // матрица поворота камеры: локальное -> мир
    float r_cam[9];
    make_r((float)ax, (float)ay, (float)az, r_cam);

    // базисные векторы в локальной системе камеры
    float f_local[3] = { 0.0f, 0.0f, -1.0f }; // вперёд
    float r_local[3] = { 1.0f, 0.0f,  0.0f }; // вправо
    float u_local[3] = { 0.0f, 1.0f,  0.0f }; // вверх

    // те же векторы, но в мировых координатах
    float f_world[3], r_world[3], u_world[3];
    matr_vec_mult(r_cam, f_local, f_world);
    matr_vec_mult(r_cam, r_local, r_world);
    matr_vec_mult(r_cam, u_local, u_world);

    // движение вперёд/назад
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camX += speed * f_world[0];
        camY += speed * f_world[1];
        camZ += speed * f_world[2];
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camX -= speed * f_world[0];
        camY -= speed * f_world[1];
        camZ -= speed * f_world[2];
    }

    // влево/вправо
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camX += speed * r_world[0];
        camY += speed * r_world[1];
        camZ += speed * r_world[2];
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camX -= speed * r_world[0];
        camY -= speed * r_world[1];
        camZ -= speed * r_world[2];
    }

    // вверх/вниз
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camX += speed * u_world[0];
        camY += speed * u_world[1];
        camZ += speed * u_world[2];
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camX -= speed * u_world[0];
        camY -= speed * u_world[1];
        camZ -= speed * u_world[2];
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ax -= deg_to_rad(a);
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ax += deg_to_rad(a);
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) ay -= deg_to_rad(a);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) ay += deg_to_rad(a);
            //фронтальный сброс
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
            ax = -deg_to_rad(35.2643897f);
            ay = -deg_to_rad(45.0f);
            az = 0.0f;
        }

}

void MovingFigure(GLFWwindow *window){
    float objSpeed = deg_to_rad(1.0f);

    // допустим:
    // W/S — вращаем вокруг X
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        objAx += objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        objAx -= objSpeed;
    }

    // A/D — вокруг Y
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        objAy += objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        objAy -= objSpeed;
    }

    // Space / Shift — вокруг Z
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        objAz += objSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        objAz -= objSpeed;
    }
}


// (поиск центра фигуры)

void compute_center(const float point[][3], int n, float pivot[3])
{
    pivot[0] = pivot[1] = pivot[2] = 0.0;
    for (int i = 0; i < n; i++) {
        pivot[0] += point[i][0];
        pivot[1] += point[i][1];
        pivot[2] += point[i][2];
    }
    pivot[0] /= n;
    pivot[1] /= n;
    pivot[2] /= n;
}


static void DrawFrame(const float point[][3], int nPoints, const int edges[][2], int nEdges)
{
    const int MAXPOINTS = 1024;
    float p_w[MAXPOINTS][2];

    float pivot[3];
    compute_center(point, nPoints, pivot);

    float R_obj[9];
    make_r((float)objAx, (float)objAy, (float)objAz, R_obj);

    float R_view[9];
    make_r((float)(-ax), (float)(-ay), (float)(-az), R_view);

    double scale = 300.0;

    for (int i = 0; i < nPoints; ++i) {
        float local[3] = {
            point[i][0] - pivot[0],
            point[i][1] - pivot[1],
            point[i][2] - pivot[2]
        };

        float rotated[3];
        matr_vec_mult(R_obj, local, rotated);

        float v_obj[3] = {
            rotated[0] + pivot[0],
            rotated[1] + pivot[1],
            rotated[2] + pivot[2]
        };



        float v_rel[3] = {
            v_obj[0] - camX,
            v_obj[1] - camY,
            v_obj[2] - camZ
        };

        float v_cam[3];
        matr_vec_mult(R_view, v_rel, v_cam);



        float point2d[2];
        proj_iso(v_cam, point2d);

        to_screen(H, W, point2d[0], point2d[1],
                  scale, &p_w[i][0], &p_w[i][1]);
    }

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(ld);
    glBegin(GL_LINES);
    for (int e = 0; e < nEdges; ++e) {
        int a = edges[e][0];
        int b = edges[e][1];
        glVertex2f(p_w[a][0], p_w[a][1]);
        glVertex2f(p_w[b][0], p_w[b][1]);
    }
    glEnd();
}


void Cube()
{
    float cube[8][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0},
    {1, 1, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1}};
    int cube_edges[12][2] = {{0, 1}, {0, 2}, {0, 3}, {1, 5}, {1, 6}, {2, 4}, {2, 5}, {3, 4}, {3, 6}, {4, 7}, {5, 7}, {6, 7}};
    DrawFrame(cube, 8, cube_edges, 12);
}

void Pyramid()
{
    float pyramid[5][3] = {{0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0.5, 0.5, 1}};
    int pyramid_edges[8][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {0, 4}, {1, 4}, {2, 4}, {3, 4}};
    DrawFrame(pyramid, 5, pyramid_edges, 8);

}

void Tetrahedron()
{
    float tetrahedron[4][3] = { {0, 0, 0}, {1, 0, 0}, {0.5, 0.8661, 0}, {0.5, 0.28887, 0.82} };
    int tetrahedron_edges[6][2] = { {0, 1}, {1, 2}, {2, 0}, {0, 3}, {1, 3}, {2, 3}};
    DrawFrame(tetrahedron, 4, tetrahedron_edges, 6);
}


void Sphere()
{
    const int N_LAT  = 16; 
    const int N_LON  = 32;
    const float R = 1.0f;
    const float PI = 3.14159265358979323846264338f;
    const int N_VERT = (N_LAT - 2) * N_LON + 2;

    float sphere[N_VERT][3];

    // 0 — северный полюс
    sphere[0][0] = 0.0f;
    sphere[0][1] = 0.0f;
    sphere[0][2] = R;

    // южный полюс
    sphere[N_VERT - 1][0] = 0.0f;
    sphere[N_VERT - 1][1] = 0.0f;
    sphere[N_VERT - 1][2] = -R;

    // получаем кольца сферы

    int idx = 1; // текущий индекс вершины 
    for (int i = 1; i <= N_LAT - 2; ++i) {
        //phi - угол между полюсами по меридиану
        float phi = PI * i / (N_LAT - 1);   // от 0 до π
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        for (int j = 0; j < N_LON; ++j) {
            //theta - угол между полюсами по парраллели
            float theta = 2.0f * PI * j / N_LON; 
            float cosTheta = cosf(theta);
            float sinTheta = sinf(theta);

            float x = R * sinPhi * cosTheta;
            float y = R * sinPhi * sinTheta;
            float z = R * cosPhi;
            sphere[idx][0] = x;
            sphere[idx][1] = y;
            sphere[idx][2] = z;
            ++idx;
        }
    }

    // максимальное количество
    const int MAX_EDGES = N_LON * (N_LAT - 1) + (N_LAT - 2) * N_LON;
    int edges[MAX_EDGES][2];
    int e = 0;

    // 1) Меридианы (вертикальные линии)
    for (int j = 0; j < N_LON; ++j) {
        // от северного полюса к первому кольцу
        edges[e][0] = 0;           // северный полюс
        edges[e][1] = 1 + j;       // j-я точка первого кольца
        ++e;

        // между кольцами
        for (int i = 0; i < N_LAT - 3; ++i) {
            int curr = 1 + i * N_LON + j;           
            int next = 1 + (i + 1) * N_LON + j;     
            edges[e][0] = curr;
            edges[e][1] = next;
            ++e;
        }

        // от последнего кольца к южному полюсу
        int lastRingIndex = 1 + (N_LAT - 3) * N_LON + j;
        edges[e][0] = lastRingIndex;
        edges[e][1] = N_VERT - 1;  // южный полюс
        ++e;
    }

    // 2) Параллели (горизонтальные линии, кроме полюсов)
    for (int i = 0; i < N_LAT - 2; ++i) {
        int ringStart = 1 + i * N_LON; // начало i-го кольца
        for (int j = 0; j < N_LON; ++j) {
            int a = ringStart + j;
            int b = ringStart + ( (j + 1) % N_LON ); // замыкание по кругу
            edges[e][0] = a;
            edges[e][1] = b;
            ++e;
        }
    }

    // e — фактическое количество рёбер
    DrawFrame(sphere, N_VERT, edges, e);
}