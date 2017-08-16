#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h> 

#include "objloader.hpp"
#include "vboindexer.hpp"
#include "texture.hpp"

// buffers

using namespace glm;

// Possui a informação de cada objeto possível de criado.
class Mesh {

private:
	// vértices
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	// VBO
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;

	// bufers
	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;

	GLuint Texture;


public:
	std::vector<unsigned short> getIndices();
	std::vector<glm::vec3> getIndexedVertices();
	std::vector<glm::vec2> getIndexedUvs();
	std::vector<glm::vec3> getIndexedNormals();
	GLuint getVertexBuffer();
	GLuint getUvBuffer();
	GLuint getNormalBuffer();
	GLuint getElementBuffer();
	GLuint getTexture();

	Mesh();
	Mesh(std::string obj);

	void loadMesh(std::string path);
	void createBuffers();
	void deleteBuffers();
};

#endif