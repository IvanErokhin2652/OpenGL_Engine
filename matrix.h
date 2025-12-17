#ifndef _MATRIX_H_
#define _MATRIX_H_

void matr_mult(float a[9], float b[9], float *res);
void matr_vec_mult(float a[9], float b[3], float *res);
void make_rx(float a, float r[9]);
void make_ry(float a, float r[9]);
void make_rz(float a, float r[9]);
void make_r(float ax, float ay, float az, float r[9]);
void pivot_turn(float r[9], float p[3], float v[3], float out[3]);
float deg_to_rad(float a);
void proj_iso(float v[3], float out2[3]);
void to_screen(int H, int W, float x2d, float y2d, float scale, float *x, float *y);
int BuildEdges(float cube[8][3], int edges[12][2]);


#endif /* _MATRIX_H_ */
