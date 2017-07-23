#include <string>
#include <iostream>

#include "Mesh.hpp"

#include <glm/glm.hpp>

Mesh::Mesh() {
	return;
}

std::vector<unsigned short> Mesh::getIndices()
{
	return this->indices;
}

std::vector<glm::vec3> Mesh::getIndexedVertices()
{
	return this->indexed_vertices;
}

std::vector<glm::vec2> Mesh::getIndexedUvs()
{
	return this->indexed_uvs;
}

std::vector<glm::vec3> Mesh::getIndexedNormals()
{
	return this->indexed_normals;
}

GLuint Mesh::getVertexBuffer()
{
	return vertexbuffer;
}

GLuint Mesh::getUvBuffer()
{
	return uvbuffer;
}

GLuint Mesh::getNormalBuffer()
{
	return normalbuffer;
}

GLuint Mesh::getElementBuffer()
{
	return elementbuffer;
}

Mesh::Mesh(std::string path)
{
	loadMesh(path);
}

void Mesh::loadMesh(std::string path)
{
	bool res = loadOBJ(path.c_str(), vertices, uvs, normals);
	if (res)
		std::cout << path << "obj loaded." << std::endl;
	else
		exit(100);

	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
}

void Mesh::createBuffers()
{
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

}



void Mesh::deleteBuffers()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
}
