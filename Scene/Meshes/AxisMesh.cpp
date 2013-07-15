#include <gl/glew.h>

#include "AxisMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <algorithm>
#include <vector>
#include <string>
#include <cassert>


AxisMesh::AxisMesh( const std::string& name )
{
	this->name = name;
	// Generate vertex buffer data
	const size_t num_axes = 3;
	const size_t verts_per_axis = 2;
	const size_t vsize = 7 * verts_per_axis * num_axes; // floats: 3 vertex, 4 color
	// Note: duplication is due to the same vertex with different texcoords/normals for different faces
	GLfloat vdata[vsize] = {
		// vertex x,y,z       color r,g,b,a
		// x axis
		0.f, 0.f, 0.f,        1.f, 0.f, 0.f, 1.f,
		1.f, 0.f, 0.f,        1.f, 0.f, 0.f, 1.f,
		// y axis
		0.f, 0.f, 0.f,        0.f, 1.f, 0.f, 1.f,
		0.f, 1.f, 0.f,        0.f, 1.f, 0.f, 1.f,
		// z axis
		0.f, 0.f, 0.f,        0.f, 0.f, 1.f, 1.f,
		0.f, 0.f, 1.f,        0.f, 0.f, 1.f, 1.f,
	};
	vertexData.insert(end(vertexData), &vdata[0], &vdata[vsize]);

	// Create vertex buffer object and transfer data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vsize, &vertexData[0], GL_STATIC_DRAW);

	// Enable and set up vertex attributes
	const GLuint vertexAttribLoc = GLUtils::simpleProgram->attrib("vertex");
	const GLuint colorAttribLoc  = GLUtils::simpleProgram->attrib("color");
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(colorAttribLoc);

	const GLsizei stride = 7 * sizeof(GLfloat);
	const GLvoid *color_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(colorAttribLoc,  4, GL_FLOAT, GL_FALSE, stride, color_offset);

	glDisableVertexAttribArray(colorAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	// Generate index buffer data
	//const size_t numfaces = 6;
	//const size_t isize = 6 * numfaces;
	//GLushort idata[isize] = {
	//	0,  1,  2,     2,  1,  3,
	//	4,  5,  6,     6,  5,  7,
	//	8, 10,  9,     9, 10, 11,
	//	12, 13, 14,    14, 13, 15,
	//	16, 17, 18,    18, 17, 19,
	//	20, 21, 22,    22, 21, 23
	//};
	//indexData.insert(end(indexData), &idata[0], &idata[isize]);

	// Create index buffer object and transfer data
	glGenBuffers(1, &indexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * isize, &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

AxisMesh::~AxisMesh()
{
	// Nothing to do, super class cleans everything up...
}

void AxisMesh::render() const
{
	// Have to do things the old fashioned way until I figure out whats up with the VAO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// Enable and set up vertex attributes
	const GLuint vertexAttribLoc = GLUtils::simpleProgram->attrib("vertex");
	const GLuint colorAttribLoc  = GLUtils::simpleProgram->attrib("color");
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(colorAttribLoc);

	const GLsizei stride = 7 * sizeof(GLfloat);
	const GLvoid *color_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(colorAttribLoc,  4, GL_FLOAT, GL_FALSE, stride, color_offset);

	glDrawArrays(GL_LINES, 0, 6);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glDrawElements(GL_TRIANGLES, indexData.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(colorAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
