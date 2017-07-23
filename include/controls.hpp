#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs(bool rotate, int &look, int nUseMouse = 0, int nWidth = 1024, int nHeight = 768);
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getPosition();

#endif