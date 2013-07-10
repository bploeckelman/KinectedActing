#include <gl/glew.h>

#include "SphereMesh.h"
#include "Mesh.h"
#include "Util/GLUtils.h"
#include "Shaders/Program.h"

#include <vector>
#include <string>

// TODO : make the c'tor params once a Mesh caching structure is in place
static const unsigned short num_slices = 20;
static const unsigned short num_stacks = 20;

/************************************************************************/
/* Vertex and index data generation code from:
/* http://stackoverflow.com/a/5989676/2171891
/************************************************************************/


SphereMesh::SphereMesh( const std::string& name )
{
	this->name = name;

	const float R = 1.f / (float)(num_slices - 1);
	const float S = 1.f / (float)(num_stacks - 1);
	const float radius = 1.f;
	const float PI = glm::pi<float>();

	// Create vertex array data
	const size_t num_vertices = num_slices * num_stacks;
	std::vector<glm::vec3> verts(num_vertices);
	std::vector<glm::vec3> normals(num_vertices);
	std::vector<glm::vec2> texcoords(num_vertices);

	auto v = begin(verts);
	auto n = begin(normals);
	auto t = begin(texcoords);

	for(unsigned short r = 0; r < num_slices; ++r)
	for(unsigned short s = 0; s < num_stacks; ++s) {
		const float y = sin( -PI/2.f + PI * r * R );
		const float x = cos(  2*PI * s * S ) * sin( PI * r * R );
		const float z = sin(  2*PI * s * S ) * sin( PI * r * R );

		*v++ = glm::vec3(x,y,z) * radius;
		*n++ = glm::vec3(x,y,z);
		*t++ = glm::vec2(s*S, r*R);
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
	indexData.resize(num_slices * num_stacks * 4);
	std::vector<GLushort>::iterator i = begin(indexData);
	for(unsigned int r = 0; r < num_slices - 1; r++)
	for(unsigned int s = 0; s < num_stacks - 1; s++) {
		*i++ = r * num_stacks + s;
		*i++ = r * num_stacks + (s+1);
		*i++ = (r+1) * num_stacks + (s+1);
		*i++ = (r+1) * num_stacks + s;
	}		

	// Create index buffer object and transfer data
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SphereMesh::~SphereMesh()
{
	// Nothing to do, super class cleans everything up...
}

void SphereMesh::render() const
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
	glDrawElements(GL_QUADS, indexData.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}