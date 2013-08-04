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
	//const GLuint colorAttribLoc  = GLUtils::simpleProgram->attrib("color");
	glEnableVertexAttribArray(vertexAttribLoc);
	//glEnableVertexAttribArray(colorAttribLoc);

	const GLsizei stride = 7 * sizeof(GLfloat);
	//const GLvoid *color_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//glVertexAttribPointer(colorAttribLoc,  4, GL_FLOAT, GL_FALSE, stride, color_offset);

	//glDisableVertexAttribArray(colorAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	// Create index buffer object and transfer data (index buffer unused)
	//glGenBuffers(1, &indexBuffer);

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
	const GLuint vertexAttribLoc = GLUtils::defaultProgram->attrib("vertex");
	//const GLuint colorAttribLoc  = GLUtils::defaultProgram->attrib("color");
	glEnableVertexAttribArray(vertexAttribLoc);
	//glEnableVertexAttribArray(colorAttribLoc);

	const GLsizei stride = 7 * sizeof(GLfloat);
	//const GLvoid *color_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	//glVertexAttribPointer(colorAttribLoc,  4, GL_FLOAT, GL_FALSE, stride, color_offset);

	// NOTE: this is hacky
	GLUtils::defaultProgram->setUniform("useLighting", 0);
	GLUtils::defaultProgram->setUniform("color", glm::vec4(1,0,0,1));
	glDrawArrays(GL_LINES, 0, 2);
	GLUtils::defaultProgram->setUniform("color", glm::vec4(0,1,0,1));
	glDrawArrays(GL_LINES, 2, 2);
	GLUtils::defaultProgram->setUniform("color", glm::vec4(0,0,1,1));
	glDrawArrays(GL_LINES, 4, 2);

	//glDisableVertexAttribArray(colorAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
