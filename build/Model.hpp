#ifndef MODEL_HPP
#define MODEL_HPP

#include<glm\gtx\transform.hpp>

#include "Mesh.hpp"

// matrix transform

// Guarda as informações de cada objeto criado no cenário.
class Model {

private:
	Mesh mesh;
	glm::mat4 transform_matrix;
	glm::mat4 translate_matrix;
	glm::mat4 scale_matrix;
	glm::mat4 rotation_matrix;
	glm::mat4 shear_matrix;

	glm::mat4 ModelMatrix;

public:
	Model(Mesh);

	glm::mat4 getTransformMatrix();
	glm::mat4 getModelMatrix();
	void setTransformMatrix(glm::mat4 &matrix);

	void translate(glm::mat4 trans_mat);
	void scale(glm::mat4 scale_mat);
	void rotation(glm::mat4 rot_mat);
	void shear(glm::mat4 shear_mat);
	void setTransformation();
	void drawModel();

};

#endif