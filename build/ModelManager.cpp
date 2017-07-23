#include "ModelManager.hpp"
#include <iostream>

ModelManager::ModelManager()
{
	for (auto obj : mesh_list) {
		std::string obj_path = "../mesh/" + obj + ".obj";
		Mesh mesh = Mesh(obj_path);
		mesh.createBuffers();
		meshs.push_back(mesh);
	}

}

std::vector<Model>* ModelManager::getModels()
{
	return &models;
}

void ModelManager::addModel(mesh_enum me)
{
	std::cout << "addModel()" << std::endl;
	Model model = Model(meshs.at(me));
	models.push_back(model);
}

void ModelManager::draw(Camera cam, GLuint m, GLuint v, GLuint mvp)
{	
	ProjectionMatrix = cam.getProjectionMatrix();
	ViewMatrix = cam.getViewMatrix();
	glm::mat4 ModelMatrix;
	
	for (auto mdl : models) {
		mdl.setTransformation();

		ModelMatrix = mdl.getModelMatrix();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		glUniformMatrix4fv(mvp, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(v, 1, GL_FALSE, &ViewMatrix[0][0]);
		
		mdl.drawModel();
	}
}

void ModelManager::deleteMeshs()
{
	for (auto msh : meshs) {
		msh.deleteBuffers();
	}
}
