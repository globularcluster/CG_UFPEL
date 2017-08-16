#ifndef MODEL_HPP
#define MODEL_HPP

#include<glm\gtx\transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mesh.hpp"

// matrix transform

// Guarda as informações de cada objeto criado no cenário.
class Model {

private:
	Mesh mesh;
	int id;
	glm::mat4 transform_matrix;
	glm::mat4 translate_matrix;
	glm::mat4 scale_matrix;
	glm::quat rotation_matrix;
	glm::mat4 shear_matrix;
	glm::mat4 ModelMatrix;

public:
	Model();
	Model(Mesh msh, glm::vec3 pos, int id, glm::vec3 rot = glm::vec3(0, 0, 0));

	glm::mat4 getTransformMatrix();
	glm::mat4 getModelMatrix();
	glm::mat4 getTranslateMatrix();
	glm::quat getRotationMatrix();
	int getId();
	GLuint getTexture();
	void setTransformMatrix(glm::mat4 &matrix);

	void translate(glm::mat4 trans_mat);
	void scale(glm::mat4 scale_mat);
	void rotation(glm::quat rot_mat);
	void shear(glm::mat4 shear_mat);
	void setTransformation();
	void drawModel();

};

#endif