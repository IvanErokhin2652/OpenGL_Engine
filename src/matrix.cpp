#include "matrix.h"
#include <cmath>

const double PI = 3.1415926535897932384626433832795028841971693993;

//умножение матриц 3х3
void matr_mult(float a[9], float b[9], float *res)
{
    res[0] = a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
    res[1] = a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
    res[2] = a[0]*b[2] + a[1]*b[5] + a[2]*b[8];

    res[3] = a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
    res[4] = a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
    res[5] = a[3]*b[2] + a[4]*b[5] + a[5]*b[8];

    res[6] = a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
    res[7] = a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
    res[8] = a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}

//умножение матрицы на вектор
void matr_vec_mult(float a[9], float b[3], float *res)
{
    res[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    res[1] = a[3] * b[0] + a[4] * b[1] + a[5] * b[2];
    res[2] = a[6] * b[0] + a[7] * b[1] + a[8] * b[2];
}

// задание матриц поворота
void make_rx(float a, float r[9])
{
    float c, s;
    c = cos(a); s = sin(a);
    r[0] = 1; r[1] = 0; r[2] = 0;
    r[3] = 0; r[4] = c; r[5] = -s;
    r[6] = 0; r[7] = s; r[8] = c; 
}

void make_ry(float a, float r[9])
{
    float c, s;
    c = cos(a); s = sin(a);
    r[0] = c; r[1] = 0; r[2] = s;
    r[3] = 0; r[4] = 1; r[5] = 0;
    r[6] = -s; r[7] = 0; r[8] = c;
}

void make_rz(float a, float r[9]) 
{
    float c, s;
    c = cos(a); s = sin(a);
    r[0] = c; r[1] = -s; r[2] = 0;
    r[3] = s; r[4] = c; r[5] = 0;
    r[6] = 0; r[7] = 0; r[8] = 1;
}

// создание общей матрицы
void make_r(float ax, float ay, float az, float r[9]) 
{
    float rx[9], ry[9], rz[9], temp[9];
    make_rx(ax, rx);
    make_ry(ay, ry);
    make_rz(az, rz);
    matr_mult(rz, ry, temp);
    matr_mult(temp, rx, r);
}

//поворот относительно точки "крепления"
void pivot_turn(float r[9], float p[3], float v[3], float out[3])
{
    float w[3] = {v[0] - p[0], v[1] - p[1], v[2] - p[2]};
    float x = w[0], y = w[1], z = w[2];
    float xr, yr, zr;
    xr = r[0] * x + r[1] * y + r[2] * z;
    yr = r[3] * x + r[4] * y + r[5] * z;
    zr = r[6] * x + r[7] * y + r[8] * z;

    out[0] = xr + p[0];
    out[1] = yr + p[1];
    out[2] = zr + p[2];

}

//из градусов в радианы
float deg_to_rad(float a)
{
    return a * PI / 180.0;
}

//проекция 3д -> 2д
void proj_iso(float v[3], float out2[3])
{
    out2[0] =  1 * v[0] + 0.70710678 * v[2];
    out2[1] =  0.40824829 * v[0] + 0.81649658 * v[1] - 0.40824829 * v[2];
}

void to_screen(int H, int W, float x2d, float y2d, float scale, float *x, float *y)
{
    float p = 10;
    float tx = 0.5 * W, ty = 0.5 * H;
    *x = tx + p + scale * x2d;
    *y = ty + p - scale * y2d;
}
