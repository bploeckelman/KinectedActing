#include <gl/glew.h>

#include "CubeMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <algorithm>
#include <vector>
#include <string>
#include <cassert>


CubeMesh::CubeMesh( const std::string& name )
{
	this->name = name;
	// Generate vertex buffer data
	const size_t numverts = 8;
	const size_t vsize = 8 * numverts; // floats: 3 vertex, 2 texture, 3 normal
	// TODO : fixup texture coordinates
	GLfloat vdata[vsize] = {
		// vertex x,y,z       texcoord s,t    normal x,y,z
		-0.5f,  0.5f,  0.5f,    0.f, 0.f,    0.f, 0.f, 1.f,
		-0.5f, -0.5f,  0.5f,    0.f, 1.f,    0.f, 0.f, 1.f,
		 0.5f,  0.5f,  0.5f,    1.f, 0.f,    0.f, 0.f, 1.f,
		 0.5f, -0.5f,  0.5f,    1.f, 1.f,    0.f, 0.f, 1.f,

		-0.5f,  0.5f, -0.5f,    0.f, 0.f,    0.f, 0.f, -1.f,
		-0.5f, -0.5f, -0.5f,    0.f, 1.f,    0.f, 0.f, -1.f,
		 0.5f,  0.5f, -0.5f,    1.f, 0.f,    0.f, 0.f, -1.f,
		 0.5f, -0.5f, -0.5f,    1.f, 1.f,    0.f, 0.f, -1.f,
	};
	vertexData.insert(end(vertexData), &vdata[0], &vdata[vsize]);

	// Create vertex buffer object and transfer data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vsize, &vertexData[0], GL_STATIC_DRAW);

	// Enable and set up vertex attributes
	const GLuint vertexAttribLoc   = GLUtils::defaultProgram->attrib("vertex");
	const GLuint texcoordAttribLoc = GLUtils::defaultProgram->attrib("texcoord");
	const GLuint normalAttribLoc   = GLUtils::defaultProgram->attrib("normal");
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texcoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);

	const GLsizei stride = 8 * sizeof(GLfloat);
	const GLvoid *tex_coord_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	const GLvoid *normal_offset = (const GLvoid *) (5 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(texcoordAttribLoc, 2, GL_FLOAT, GL_TRUE, stride, tex_coord_offset);
	glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_TRUE, stride, normal_offset);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	// Generate index buffer data
	const size_t numfaces = 6;
	const size_t isize = 6 * numfaces;
	GLushort idata[isize] = {
		0, 1, 2,    2, 1, 3,
		3, 7, 2,    2, 7, 6,
		6, 7, 5,    5, 4, 6,
		4, 5, 0,    0, 5, 1,
		1, 5, 3,    3, 5, 7,
		0, 2, 4,    2, 6, 4
	};
	indexData.insert(end(indexData), &idata[0], &idata[isize]);

	// Create index buffer object and transfer data
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * isize, &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CubeMesh::~CubeMesh()
{
	// Nothing to do, super class cleans everything up...
}

void CubeMesh::render() const
{
	// Have to do things the old fashioned way until I figure out whats up with the VAO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	const GLuint vertAttribLoc     = GLUtils::defaultProgram->attrib("vertex");
	const GLuint texcoordAttribLoc = GLUtils::defaultProgram->attrib("texcoord");
	const GLuint normalAttribLoc   = GLUtils::defaultProgram->attrib("normal");
	glEnableVertexAttribArray(vertAttribLoc);
	glEnableVertexAttribArray(texcoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);

	const GLsizei stride = 8 * sizeof(GLfloat);
	const GLvoid *tex_coord_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	const GLvoid *normal_offset = (const GLvoid *) (5 * sizeof(GLfloat));
	glVertexAttribPointer(vertAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(texcoordAttribLoc, 2, GL_FLOAT, GL_TRUE,  stride, tex_coord_offset);
	glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_TRUE,  stride, normal_offset);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, indexData.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
