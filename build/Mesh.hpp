#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h> 

#include "objloader.hpp"
#include "vboindexer.hpp"

// buffers

using namespace glm;

// Possui a informa��o de cada objeto poss�vel de criado.
class Mesh {

private:
	// v�rtices
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


public:
	std::vector<unsigned short> getIndices();
	std::vector<glm::vec3> getIndexedVertices();
	std::vector<glm::vec2> getIndexedUvs();
	std::vector<glm::vec3> getIndexedNormals();
	GLuint getVertexBuffer();
	GLuint getUvBuffer();
	GLuint getNormalBuffer();
	GLuint getElementBuffer();

	Mesh();
	Mesh(std::string path);

	void loadMesh(std::string path);
	void createBuffers();
	void deleteBuffers();
};

#endif