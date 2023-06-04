#ifndef __GLVIS_H__
#define __GLVIS_H__


#include "shader.h"
#include "camera.h"
struct Buffers {
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
};


struct Frustum {
	GLfloat FoV = glm::radians(45.f);
	GLfloat aspect = 16 / 9;
	GLfloat nearClippingDistance = 0.1f;
	GLfloat farClippingDistance = 1000.0f;
};

struct CameraTransform {
	// position
	glm::vec3 position = glm::vec3(0, 0, 5);
	// lookAt
	glm::vec3 lookAt = glm::vec3(0, 0, -1);
	// up
	glm::vec3 up = glm::vec3(0, 1, 0);
};

struct Scene {
	Frustum frustum;
	Camera* camera;
};

struct Color {
	float r, g, b;
	Color():r(0.0f), g(0.0f), b(0.0f) {}
	Color(float r, float g, float b): r(0.0f), g(0.0f), b(0.0f) {}
};


#endif