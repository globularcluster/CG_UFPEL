#pragma once

// model [...]
// mesh [...]
// draw

// MESH 1<--->N MODEL
#include <AntTweakBar.h>
#include "Model.hpp"
#include "Mesh.hpp"
#include "Camera.h"
//#include "controls.hpp"

#include <string>

class ModelManager {

private:
	std::vector<Mesh> meshs;
	std::vector<Model> models;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 MVP;

public:
	enum mesh_enum {cube, goose, suzanne};
	std::vector<std::string> mesh_list = { "cube", "goose", "suzanne" };

	ModelManager();
	std::vector<Model>* getModels();
	void addModel(mesh_enum me);
	void draw(Camera cam, GLuint m, GLuint v, GLuint mvp);
	void deleteMeshs();

};