#include <gl/glew.h>

#include "CapsuleMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <vector>
#include <string>

// TODO : make the c'tor params once a Mesh caching structure is in place
static const unsigned short res = 64;

/************************************************************************/
/* Vertex and index data generation code from:
/* http://paulbourke.net/geometry/capsule/capsule.c
/* 
/* NOTE : texture coordinates and indexing is not quite right yet
/************************************************************************/


CapsuleMesh::CapsuleMesh( const std::string& name )
{
	this->name = name;

	const float radius = 1.f;
	const float height = 1.f;
	const float PI = glm::pi<float>();
	const float HALF_PI = glm::half_pi<float>();

	// Create vertex array data
	const int num_vertices = (res + 1) * (res / 2 + 2);
	std::vector<glm::vec3> verts(num_vertices);
	std::vector<glm::vec3> normals(num_vertices);
	std::vector<glm::vec2> texcoords(num_vertices);

	auto v = begin(verts);
	auto n = begin(normals);
	auto t = begin(texcoords);

	float theta = 0.f;
	float phi = 0.f;

	// Top Cap
	for (int j = 0; j <= res / 4; ++j)
	for (int i = 0; i <= res;     ++i) {
		// Calculate angles
		theta = i * 2.f * PI / res;
		phi = -HALF_PI + PI * j / (res / 2.f);

		// Vertex
		*v = glm::vec3(glm::cos(phi) * glm::cos(theta)
					 , glm::cos(phi) * glm::sin(theta)
					 , glm::sin(phi)) * radius;
		// Texture
		const float s = -atan2(v->y, v->x) / (2.f * PI);
		*t = glm::vec2((s < 0.f) ? 1.f + s : s
					 , 0.5f + atan2(v->z, sqrt(v->x * v->x + v->y * v->y)) / PI);
		// Normal
		*n = glm::normalize(*v);

		// Offset for central cylinder
		v->z -= height / 2.f;

		// Next set of vectors
		++v; ++t; ++n;
	}

	// Bottom Cap
	for (int j = res / 4; j <= res / 2; ++j)
	for (int i = 0;       i <= res;     ++i) {
		// Calculate angles
		theta = i * 2.f * PI / res;
		phi = -HALF_PI + PI * j / (res / 2.f);

		// Vertex 
		*v = glm::vec3(glm::cos(phi) * glm::cos(theta)
					 , glm::cos(phi) * glm::sin(theta)
					 , glm::sin(phi)) * radius;
		// Texture
		const float s = -atan2(v->y, v->x) / (2.f * PI);
		*t = glm::vec2((s < 0.f) ? 1.f + s : s
					 , 0.5f + atan2(v->z, sqrt(v->x * v->x + v->y * v->y)) / PI);
		// Normal
		*n = glm::normalize(*v);

		// Offset for central cylinder
		v->z += height / 2.f;

		// Next set of vectors
		++v; ++t; ++n;
	}

	// Interleave vertex buffer data
	vertexData.resize(8 * num_vertices); // num floats: 3 vertex, 2 texcoord, 3 normal
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
	indexData.clear();
	GLushort i1, i2, i3, i4;
	for (GLushort j = 0; j <= res / 2; ++j)
	for (GLushort i = 0; i <= res; ++i) {
		// Calculate indices
		i1 =  j    * (res+1) +  i    + 1;
		i2 =  j    * (res+1) + (i+1) + 1;
		i3 = (j+1) * (res+1) + (i+1) + 1;
		i4 = (j+1) * (res+1) +  i    + 1;

		// Face 1
		indexData.push_back(i1);
		indexData.push_back(i2);
		indexData.push_back(i3);
		// Face 2
		indexData.push_back(i1);
		indexData.push_back(i3);
		indexData.push_back(i4);
	}

	// Create index buffer object and transfer data
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CapsuleMesh::~CapsuleMesh()
{
	// Nothing to do, super class cleans everything up...
}

void CapsuleMesh::render() const
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
