#include <../Camera.h>

using namespace glm;

Camera::Camera(GLFWwindow* window, glm::vec3 position) {
	g_pWindow = window;
	this->position = position;

	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;
	FoV = 90.0f;
	zNear = 0.1f;
	zFar = 100;
	speed = 3.0f;  // 3 units / second
	mouseSpeed = 0.005f;

	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	up = glm::cross(right, direction);

}

glm::mat4 Camera::getViewMatrix() {
	return ViewMatrix;
}
glm::mat4 Camera::getProjectionMatrix() {
	return ProjectionMatrix;
}
glm::vec3 Camera::getPosition()
{
	return position;
}
void Camera::setPosition(glm::vec3 pos)
{
	position += pos;
}
void Camera::computeMatricesFromInputs(int nUseMouse, int nWidth, int nHeight) {

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

	// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, zNear, zFar);


	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);


	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void Camera::rotate(float x, float y, float z, float px, float py, float pz)
{
	 static float d = glm::distance(position, vec3(px, py, pz));

	position.x = sin(x) * d;
	position.z = cos(x) * d;
	
	direction = vec3(px, py, pz);

	ViewMatrix = glm::lookAt(
		position,
		direction,
		up
	);

	direction = vec3(0, 0, -1);
	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);
	//position = vec3(0, 0, 5);

}

void Camera::look(float x, float y)
{
	int nWidth = 1024;
	int nHeight = 768;

	vec3 dir = vec3(x, y, -1);

	direction = dir;
	up = vec3(0, 1, 0);
	right = glm::cross(direction, up);

	// Reset mouse position for next frame
	//glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);
}

void Camera::pitch(float x, float y, float z)
{
	direction = glm::rotate(direction, x, vec3(1, 0, 0));
	direction = glm::rotate(direction, y, vec3(0, 1, 0));
	direction = glm::rotate(direction, z, vec3(0, 0, 1));

}

void Camera::setFov(float ang)
{
	FoV = ang;
}

void Camera::setClipping(float n, float f)
{
	zNear = n;
	zFar = f;		
}
