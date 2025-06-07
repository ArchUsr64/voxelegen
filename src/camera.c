#include "camera.h"

#include <math.h>

void camera_handle_inputs(struct Camera* camera, struct CameraKeys keys) {
	float delta_x, delta_z;
	float cos_yaw = cos(-camera->yaw);
	float sin_yaw = sin(-camera->yaw);

	delta_x = (keys.right - keys.left) * KEYBOARD_SENS;
	delta_z = (keys.in - keys.out) * KEYBOARD_SENS;
	camera->position[0] += delta_x * cos_yaw - delta_z * sin_yaw;
	camera->position[1] += (float)(keys.up - keys.down) * KEYBOARD_SENS;
	camera->position[2] += delta_x * sin_yaw + delta_z * cos_yaw;
}

void camera_to_view_matrix(struct Camera camera, Matrix4* matrix) {
	float cos_pitch = cos(camera.pitch);
	float sin_pitch = sin(camera.pitch);
	float cos_yaw = cos(camera.yaw);
	float sin_yaw = sin(camera.yaw);

	Matrix4 view_translation = {
		{1, 0, 0, camera.position[0]},
		{0, 1, 0, camera.position[1]},
		{0, 0, 1, camera.position[2]},
		{0, 0, 0, 1},
	};

	Matrix4 view_rotation_x = {
		{1, 0, 0, 0},
		{0, cos_pitch, -sin_pitch, 0},
		{0, sin_pitch, cos_pitch, 0},
		{0, 0, 0, 1},
	};

	Matrix4 view_rotation_y = {
		{cos_yaw, 0, sin_yaw, 0},
		{0, 1, 0, 0},
		{-sin_yaw, 0, cos_yaw, 0},
		{0, 0, 0, 1},
	};

	matrix_multiply(&view_rotation_y, &view_rotation_x);

	matrix_multiply(&view_translation, &view_rotation_y);

	matrix_cpy(matrix, &view_translation);
}
