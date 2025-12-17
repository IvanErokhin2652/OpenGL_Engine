#include <GLFW/glfw3.h>
#include <cmath>
#include "matrix.h"
#include "graphics.h"
#include "mesh.h"
#include "obj_loader.h"
#include <iostream>

//obj файлы
std::vector<Vec3> objVerts;
std::vector<Edge> objEdges;
std::vector<Face> objFaces;

void init() {
    if (!loadObjMesh("12140_Skull_v3_L2.obj", objVerts, objEdges, objFaces)) {
        std::cerr << "Failed to load OBJ file" << std::endl;
    }
}

//Угол поворота на кадр
float a = 2.0;
float ld = 2.0;

//положение камеры
float camX, camY, camZ;
bool cameraMode = true;
//Размеры экрана
int W = 1280, H = 720;

// углы поворота
double ax, ay, az;
double objAx, objAy, objAz;
void solve()
{
    std::cout << "КОМАНДЫ НА ВЫБОР:" << std::endl;
    
        std::cout << "1. Ширина линий (1 - обычные, 2 - жирные): \n";
        int cmd;
        std::cin >> cmd;
        if (cmd == 1) ld = 2.0;
        else ld = 6.0;
        std::cout << "2. Скорость вращения(1 - обычная, 2 - быстро, 3 - медленно): ";
        std::cin >> cmd;
        if (cmd == 1) a = 2.0;
        else if (cmd == 2) a = 10.0;
        else a = 0.1;
    
    
}



void SetupFor2D(void)
{
    //glDisable(GL_CULL_FACE); // отсечение по направлению

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, W, H, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);    // стандартный вариант
    glClearDepth(1.0);         // максимум глубины

}


int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_DEPTH_BITS, 64);

    camX = 0.5f; camY = 0.5f; camZ = 3.0f;

    ax = -deg_to_rad(35.2643897f);
    ay = -deg_to_rad(45.0f);
    az = 0.0f;

    int prevTurnC = GLFW_RELEASE;
    
    bool run = true;
    init();
    while (run) {
        solve();
        GLFWwindow* window = glfwCreateWindow(W, H, "OpenGL_ENGINE", NULL, NULL);
        
        glfwMakeContextCurrent(window);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearDepth(1.0);

        SetupFor2D();
        while (!glfwWindowShouldClose(window)){
            int turnC = glfwGetKey(window, GLFW_KEY_C);
            if (turnC == GLFW_PRESS && prevTurnC == GLFW_RELEASE){
                cameraMode = !cameraMode;
                if (cameraMode)
                    std::cout << "РЕЖИМ: Камера\n";
                else
                    std::cout << "РЕЖИМ: Фигура\n";
            }
            prevTurnC = turnC;

            glClearColor(0.95f, 0.95f, 0.95f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (cameraMode){
                MovingCamera(window);
            } 
            else{
                MovingFigure(window);
            }

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            }

            DrawMesh(objVerts, objEdges, objFaces);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if (glfwWindowShouldClose(window)) {
            run = false;
        }
    }

    glfwTerminate();
    return 0;
}


