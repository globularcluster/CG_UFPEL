#include "ModelManager.hpp"
#include <iostream>

ModelManager::ModelManager()
{


	for (auto obj : mesh_list) {
		Mesh mesh = Mesh(obj);
		mesh.createBuffers();
		meshs.push_back(mesh);
	}


}

std::vector<Model>* ModelManager::getModels()
{
	return &models;
}

std::vector<Model>* ModelManager::getTargets()
{
	return &targets;
}

Model *ModelManager::addModel(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot)
{
	std::cout << "addModel()" << std::endl;
	Model model = Model(meshs.at(me), pos, id, rot);
	models.push_back(model);
	return &model;
}

Model *ModelManager::addParede(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot)
{
	std::cout << "addParede()" << std::endl;
	Model model = Model(meshs.at(me), pos, id, rot);
	paredes.push_back(model);
	return &model;
}

Model *ModelManager::addTarget(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot)
{
	std::cout << "addTarget()" << std::endl;
	Model model = Model(meshs.at(me), pos, id, rot);
	targets.push_back(model);
	return &model;
}

void ModelManager::removeModel(int id)
{
	removidos.push_back(id);
}


void ModelManager::draw(GLuint pID, Camera cam)
{	

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(pID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(pID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(pID, "M");

	GLuint TextureID = glGetUniformLocation(pID, "myTextureSampler");

	ProjectionMatrix = cam.getProjectionMatrix();
	ViewMatrix = cam.getViewMatrix();
	glm::mat4 ModelMatrix;
	
	for (int i = 0; i < models.size(); i++) {


		Model mdl = models.at(i);
		
		mdl.setTransformation();

		ModelMatrix = mdl.getModelMatrix();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mdl.getTexture());
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		mdl.drawModel();
	}

	for (int i = 0; i < paredes.size(); i++) {

		Model mdl = paredes.at(i);

		mdl.setTransformation();

		ModelMatrix = mdl.getModelMatrix();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mdl.getTexture());
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		mdl.drawModel();
	}

	for (int i = 0; i < targets.size(); i++) {

		Model mdl = targets.at(i);

		mdl.setTransformation();

		ModelMatrix = mdl.getModelMatrix();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mdl.getTexture());
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		mdl.drawModel();
	}
}

void ModelManager::deleteMeshs()
{
	for (auto msh : meshs) {
		msh.deleteBuffers();
	}
}
