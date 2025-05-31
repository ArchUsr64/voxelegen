#ifndef MATRIX_H_

#define MATRIX_H_

typedef float Matrix4[4][4];
typedef float Point[4];

// Does a *= b
void matrix_multiply(Matrix4* a, Matrix4* b);

void print_matrix(Matrix4 *a);

void perespective_projection_matrix(float near_plane, float far_plane, float fov, float aspect_ratio, Matrix4* matrix);

// Does pt = transform * pt
void transform_point_by_matrix(Point *pt, Matrix4 *transform);

#endif
