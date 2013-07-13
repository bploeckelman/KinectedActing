#include <gl/glew.h>

#include "Mesh.h"


Mesh::Mesh()
	: name("")
	, vertexData()
	, indexData()
	, vertexBuffer(0)
	, indexBuffer(0)
{}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &indexBuffer);
	glDeleteBuffers(1, &vertexBuffer);
}

