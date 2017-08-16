#pragma once

// model [...]
// mesh [...]
// draw

// MESH 1<--->N MODEL
#include <AntTweakBar.h>
#include <algorithm>
#include "Model.hpp"
#include "Mesh.hpp"
#include "Camera.h"
//#include "controls.hpp"

#include <string>

class ModelManager {

private:
	std::vector<Mesh> meshs;
	std::vector<Model> models;
	std::vector<Model> paredes;
	std::vector<Model> targets;

	std::vector<int> removidos;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 MVP;

public:
	enum mesh_enum { cube, suzanne, esfera, chao, parede };
	std::vector<std::string> mesh_list = { "cube", "suzanne", "esfera", "chao", "parede" };

	ModelManager();
	std::vector<Model>* getModels();
	std::vector<Model> *getTargets();
	Model *addModel(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot = glm::vec3(0, 0, 0));
	Model *addParede(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot = glm::vec3(0, 0, 0));
	Model *addTarget(mesh_enum me, glm::vec3 pos, int id, glm::vec3 rot = glm::vec3(0, 0, 0));

	void removeModel(int id);
	void draw(GLuint pID, Camera cam);
	void deleteMeshs();

};