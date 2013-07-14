#include <gl/glew.h>

#include "CylinderMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <vector>
#include <string>

// TODO : make the c'tor params once a Mesh caching structure is in place
static const unsigned short num_sides = 20;
static const float height = 1.f;

/************************************************************************/
/* Vertex data generation code adapted from:
/* http://gamedev.stackexchange.com/a/17226/27510
/************************************************************************/


CylinderMesh::CylinderMesh( const std::string& name )
{
	this->name = name;

	const float radius = 1.f;
	const float PI = glm::pi<float>();
	const float theta = 2.f * PI / (float) num_sides;
	const float c = glm::cos(theta);
	const float s = glm::sin(theta);

	// Create vertex array data
	const size_t num_vertices = (num_sides + 1) * 2;
	std::vector<glm::vec3> verts(num_vertices);
	std::vector<glm::vec3> normals(num_vertices);
	std::vector<glm::vec2> texcoords(num_vertices);

	auto v = begin(verts);
	auto n = begin(normals);
	auto t = begin(texcoords);

	float x = radius;
	float z = 0.f;
	float nrml,tx;
	for(unsigned short i = 0; i <= num_sides; ++i) {
		tx = (float) i / num_sides;

		// Vertex 1 (bottom)
		nrml = 1.f / glm::sqrt(x*x + z*z);
		*v++ = glm::vec3(x, 0.f, z) * radius;
		*n++ = glm::vec3(x, 0.f, z) * nrml;
		*t++ = glm::vec2(tx, 0.f);

		// Vertex2 (top)
		*v++ = glm::vec3(x, height / radius, z) * radius;
		*n++ = glm::vec3(x, 0.f, z) * nrml;
		*t++ = glm::vec2(tx, 1.f);

		// Next edge
		const float x1 = x;
		x = c*x  - s*z;
		z = s*x1 + c*z;
	}

	// Interleave vertex buffer data
	vertexData.resize(8 * num_vertices); // num floats: 3 vertex, 2 texcoord, 3 normal
	auto d = begin(vertexData);

	v = begin(verts);
	n = begin(normals);
	t = begin(texcoords);

	for (size_t i = 0; i < vertexData.size(); i += 8) {
		// Location 0 - vertices
		vertexData[i + 0] = v->x;
		vertexData[i + 1] = v->y;
		vertexData[i + 2] = v->z;
		// Location 1 - texcoords
		vertexData[i + 3] = t->s;
		vertexData[i + 4] = t->t;
		// Location 2 - normals
		vertexData[i + 5] = n->x;
		vertexData[i + 6] = n->y;
		vertexData[i + 7] = n->z;

		// next set of vectors
		++v; ++n; ++t;
	}

	// Create vertex buffer object and transfer data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

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
	// NOTE : not needed, glDrawArrays() works with this vertex data

	// Create index buffer object and transfer data
	//glGenBuffers(1, &indexBuffer);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CylinderMesh::~CylinderMesh()
{
	// Nothing to do, super class cleans everything up...
}

void CylinderMesh::render() const
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

	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexData.size() / 8);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glDrawElements(GL_QUADS, indexData.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
