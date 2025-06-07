#ifndef CAMERA_H_

#define CAMERA_H_

#include "matrix.h"

#define KEYBOARD_SENS 0.1

struct Camera {
	float position[3];
	float yaw, pitch;
};

struct CameraKeys {
	unsigned char up, down, left, right, in, out;
};

void camera_handle_inputs(struct Camera* camera, struct CameraKeys);

void camera_to_view_matrix(struct Camera camera, Matrix4* matrix);

#endif
