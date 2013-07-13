#include <gl/glew.h>

#include "PlaneMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <algorithm>
#include <vector>
#include <string>
#include <cassert>


PlaneMesh::PlaneMesh( const std::string& name
					, float width/*=1.f */
					, float height/*=1.f */
					, float spacing/*=0.1f*/ )
{
	assert(width > 0.f && height > 0.f && spacing > 0.f);

	this->name = name;
	const unsigned int rows = static_cast<unsigned int>((height / spacing) + 1);
	const unsigned int cols = static_cast<unsigned int>((width  / spacing) + 1);

	// Generate vertex buffer data
	const size_t numverts = rows * cols;
	const size_t vsize = 8 * numverts; // floats: 3 vertex, 2 texture, 3 normal
	vertexData.resize(vsize);
	auto v = begin(vertexData);
	for (float z = -height / 2.f; z <= height / 2.f; z += spacing)
	for (float x = -width  / 2.f; x <= width  / 2.f; x += spacing) {
		// x,y,z vertex
		v[0] = x;
		v[1] = 0.f;
		v[2] = z;
		// s,t texcoord [0..1]
		v[3] = (x + (width  / 2.f)) / width;
		v[4] = (z + (height / 2.f)) / height;
		// x,y,z normal (world up +y)
		glm::vec3 n = glm::normalize(glm::vec3(x / width, 1.f, z / height));
		v[5] = n.x;
		v[6] = n.y;
		v[7] = n.z;

		v += 8; // next vertex
	}

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
	// Note: CCW winding, extra vertices add degenerate triangles between rows
	for (unsigned int r = 0, i = 0; r < rows - 1; ++r) {
		indexData.push_back(r * cols);
		for (unsigned int c = 0; c < cols; ++c) {
			indexData.push_back((r + 1) * cols + c);
			indexData.push_back(r * cols + c);
		}
		indexData.push_back((r + 1) * cols + (cols - 1));
	}
	const size_t isize = indexData.size();

	// Create index buffer object and transfer data
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * isize, &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

PlaneMesh::~PlaneMesh()
{
	// Nothing to do, super class cleans everything up...
}

void PlaneMesh::render() const
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
	glDrawElements(GL_TRIANGLE_STRIP, indexData.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
