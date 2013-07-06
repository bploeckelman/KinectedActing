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
	const size_t num_faces = 6;
	const size_t verts_per_face = 4;
	const size_t vsize = 8 * verts_per_face * num_faces; // floats: 3 vertex, 2 texture, 3 normal
	// Note: duplication is due to the same vertex with different texcoords/normals for different faces
	GLfloat vdata[vsize] = {
		// vertex x,y,z       texcoord s,t    normal x,y,z
		// Front face  (0, 1, 2  :  2, 1, 3)
		-0.5f,  0.5f,  0.5f,    0.f, 0.f,    0.f, 0.f, 1.f,
		-0.5f, -0.5f,  0.5f,    0.f, 1.f,    0.f, 0.f, 1.f,
		 0.5f,  0.5f,  0.5f,    1.f, 0.f,    0.f, 0.f, 1.f,
		 0.5f, -0.5f,  0.5f,    1.f, 1.f,    0.f, 0.f, 1.f,
		// Right face  (4, 5, 6  :  6, 5, 7)
		 0.5f,  0.5f,  0.5f,    0.f, 0.f,    1.f, 0.f, 0.f,
		 0.5f, -0.5f,  0.5f,    0.f, 1.f,    1.f, 0.f, 0.f,
		 0.5f,  0.5f, -0.5f,    1.f, 0.f,    1.f, 0.f, 0.f,
		 0.5f, -0.5f, -0.5f,    1.f, 1.f,    1.f, 0.f, 0.f,
		// Rear face   (8, 10, 9  :  9, 10, 11)
		-0.5f,  0.5f, -0.5f,    0.f, 0.f,    0.f, 0.f, -1.f,
		-0.5f, -0.5f, -0.5f,    0.f, 1.f,    0.f, 0.f, -1.f,
		 0.5f,  0.5f, -0.5f,    1.f, 0.f,    0.f, 0.f, -1.f,
		 0.5f, -0.5f, -0.5f,    1.f, 1.f,    0.f, 0.f, -1.f,
		// Left face   (12, 13, 14  : 14, 13, 15)
		-0.5f,  0.5f, -0.5f,    0.f, 0.f,   -1.f, 0.f, 0.f,
		-0.5f, -0.5f, -0.5f,    0.f, 1.f,   -1.f, 0.f, 0.f,
		-0.5f,  0.5f,  0.5f,    1.f, 0.f,   -1.f, 0.f, 0.f,
		-0.5f, -0.5f,  0.5f,    1.f, 1.f,   -1.f, 0.f, 0.f,
		// Top face    (16, 17, 18  :  18, 17, 19)
		-0.5f,  0.5f,  0.5f,    0.f, 0.f,    0.f, 1.f, 0.f,
		 0.5f,  0.5f,  0.5f,    1.f, 0.f,    0.f, 1.f, 0.f,
		-0.5f,  0.5f, -0.5f,    0.f, 1.f,    0.f, 1.f, 0.f,
		 0.5f,  0.5f, -0.5f,    1.f, 1.f,    0.f, 1.f, 0.f,
		// Bottom face (20, 21, 22  :  22, 21, 23)
		-0.5f, -0.5f, -0.5f,    0.f, 1.f,    0.f, -1.f, 0.f,
		 0.5f, -0.5f, -0.5f,    1.f, 1.f,    0.f, -1.f, 0.f,
		-0.5f, -0.5f,  0.5f,    0.f, 0.f,    0.f, -1.f, 0.f,
		 0.5f, -0.5f,  0.5f,    1.f, 0.f,    0.f, -1.f, 0.f
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
		 0,  1,  2,     2,  1,  3,
		 4,  5,  6,     6,  5,  7,
		 8, 10,  9,     9, 10, 11,
		12, 13, 14,    14, 13, 15,
		16, 17, 18,    18, 17, 19,
		20, 21, 22,    22, 21, 23
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
