#include <iostream>

// Include GLFW
#include <glfw3.h>
extern GLFWwindow* g_pWindow; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

							  // Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 0, 5);
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

glm::vec3 getPosition() {
	return position;
}

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;


glm::vec3 direction = vec3(
	cos(verticalAngle) * sin(horizontalAngle),
	sin(verticalAngle),
	cos(verticalAngle) * cos(horizontalAngle));

glm::vec3 right = glm::vec3(
	sin(horizontalAngle - 3.14f / 2.0f),
	0,
	cos(horizontalAngle - 3.14f / 2.0f)
);

glm::vec3 up = glm::cross(right, direction);


void computeMatricesFromInputs(bool rotate, int &look, int nUseMouse, int nWidth, int nHeight) {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();
	static int nLastUseMouse = 1;

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos = nWidth / 2, ypos = nHeight / 2;


	if (nUseMouse) {

		if (nLastUseMouse != nUseMouse)
			glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);

		glfwGetCursorPos(g_pWindow, &xpos, &ypos);

		// Reset mouse position for next frame
		glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);
			

		// Compute new orientation
		horizontalAngle += mouseSpeed * float(nWidth / 2 - xpos);
		verticalAngle += mouseSpeed * float(nHeight / 2 - ypos);

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		direction = vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		// Right vector
		right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);

		// Up vector
		up = glm::cross(right, direction);
	}

	nLastUseMouse = nUseMouse;

	if (look) {
		vec3 dir = vec3(0.2, 0, -1);

		direction = dir;
		up = vec3(0, 1, 0);
		right = glm::cross(up, direction);

		// Reset mouse position for next frame
		glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);

		//horizontalAngle = 3.14;

	}

#pragma region TRANSLATE
	// Move forward
	if (glfwGetKey(g_pWindow, GLFW_KEY_UP) == GLFW_PRESS
		|| glfwGetKey(g_pWindow, GLFW_KEY_E) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(g_pWindow, GLFW_KEY_DOWN) == GLFW_PRESS 
		|| glfwGetKey(g_pWindow, GLFW_KEY_Q) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(g_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS
		|| glfwGetKey(g_pWindow, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS
		|| glfwGetKey(g_pWindow, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}
	if (glfwGetKey(g_pWindow, GLFW_KEY_W) == GLFW_PRESS) {
		position.y += 1 * deltaTime * speed;
	}
	if (glfwGetKey(g_pWindow, GLFW_KEY_S) == GLFW_PRESS) {
		position.y -= 1 * deltaTime * speed;
	}
#pragma endregion

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

						   // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	
	
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	if (rotate) {
		vec3 point(0, 0, 0);
		static float rot =  0.5f;
		
		mat4 Camera = glm::translate(glm::mat4(1.0f), point);
		Camera = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 1.0f, 0.0f)) * Camera;
		Camera = translate(Camera, -point);


		ViewMatrix *= Camera;

		//mat4 Camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
		//mat4 PivotPoint = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f));
		//
		//Camera = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 1.0f, 0.0f)) * Camera;
		//ViewMatrix = PivotPoint * Camera;

		rot += 0.1f;
	}

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}