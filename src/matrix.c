#include <stdio.h>
#include <math.h>

#include "matrix.h"

void matrix_cpy(Matrix4* dest, Matrix4* src) {
	for (unsigned i = 0; i < 4; i++)
		for (unsigned j = 0; j < 4; j++)
			(*dest)[i][j] = (*src)[i][j];
}

// Does a *= b
void matrix_multiply(Matrix4* a, Matrix4* b) {
	Matrix4 result = {{0}, {0}, {0}, {0}};

	for (unsigned i = 0; i < 4; i++)
		for (unsigned j = 0; j < 4; j++)
			for (unsigned k = 0; k < 4; k++)
				result[i][j] += (*a)[i][k] * (*b)[k][j];

	matrix_cpy(a, &result);
}

void print_matrix(Matrix4* a) {
	printf("Matrix Data:\n");
	for (unsigned i = 0; i < 4; i++) {
		for (unsigned j = 0; j < 4; j++) {
			printf("%f, ", (*a)[i][j]);
		}
		printf("\n");
	}
}

void perespective_projection_matrix(
	float near,
	float far,
	float fov,
	float aspect_ratio,
	Matrix4* matrix) {
	float tan_fov_by_2 = tan(fov / 2);
	Matrix4 perspective_matrix = {
		{1 / (tan_fov_by_2 * aspect_ratio), 0, 0, 0},
		{0, 1 / tan_fov_by_2, 0, 0},
		{0, 0, (far + near) / (far - near), -2 * (far * near) / (far - near)},
		{0, 0, 1, 0},
	};
	matrix_cpy(matrix, &perspective_matrix);
}

void transform_point_by_matrix(Point* pt, Matrix4* transform) {
	Point temp;

	temp[0] = (*transform)[0][0] * (*pt)[0];
	temp[0] += (*transform)[0][1] * (*pt)[1];
	temp[0] += (*transform)[0][2] * (*pt)[2];
	temp[0] += (*transform)[0][3] * (*pt)[3];

	temp[1] = (*transform)[1][0] * (*pt)[0];
	temp[1] += (*transform)[1][1] * (*pt)[1];
	temp[1] += (*transform)[1][2] * (*pt)[2];
	temp[1] += (*transform)[1][3] * (*pt)[3];

	temp[2] = (*transform)[2][0] * (*pt)[0];
	temp[2] += (*transform)[2][1] * (*pt)[1];
	temp[2] += (*transform)[2][2] * (*pt)[2];
	temp[2] += (*transform)[2][3] * (*pt)[3];

	temp[3] = (*transform)[3][0] * (*pt)[0];
	temp[3] += (*transform)[3][1] * (*pt)[1];
	temp[3] += (*transform)[3][2] * (*pt)[2];
	temp[3] += (*transform)[3][3] * (*pt)[3];

	(*pt)[0] = temp[0];
	(*pt)[1] = temp[1];
	(*pt)[2] = temp[2];
	(*pt)[3] = temp[3];
}
