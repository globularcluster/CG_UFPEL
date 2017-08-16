#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glfw3.h>

class Camera {

private:
	GLFWwindow* g_pWindow;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::vec3 position;
	float horizontalAngle;
	float verticalAngle;
	float FoV;
	float zNear;
	float zFar;

	float speed;
	float mouseSpeed;

	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

public:
	Camera(GLFWwindow* window, glm::vec3 position = glm::vec3(0, 0, 5));

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);

	void computeMatricesFromInputs(int nUseMouse = 0, int nWidth = 1024, int nHeight = 768);
	void rotate(float x, float y, float z, float px, float py, float pz);
	void look(float x, float y);
	void pitch(float x, float y, float z);
	void setFov(float ang);
	void setClipping(float n, float f);

};