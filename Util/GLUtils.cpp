/************************************************************************/
/* GLUtils 
/* -------
/* A namespace containing functions for glsl shader manipulation
/************************************************************************/
#include "GLUtils.h"
#include "Shaders/Program.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;


tdogl::Program *GLUtils::defaultProgram = nullptr;

tdogl::Program *createShaderProgram(const string& vertexShaderFilename, const string& fragmentShaderFilename);

void GLUtils::init()
{
	// Initialize glew
	GLenum result = glewInit();
	if (GLEW_OK == result) {
		if (!glewIsSupported("GL_VERSION_3_3")) {
			cerr << "OpenGL 3.0 is not supported\n";
			exit(EXIT_FAILURE);
		}
		cout << "GLEW initialized: " << glewGetString(GLEW_VERSION) << "\n"
		     << "OpenGL version: "   << glGetString(GL_VERSION) << "\n"
		     << "GLSL version: "     << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
		     << "Vendor: "           << glGetString(GL_VENDOR) << "\n"
		     << "Renderer: "         << glGetString(GL_RENDERER) << "\n";
		int numExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
		for(int i = 0; i < numExtensions; ++i)
		{
			const GLubyte *extension_name=glGetStringi(GL_EXTENSIONS, i);
			cout << "Extension: " << extension_name << "\n";
		}
	} else {
		cerr << "Failed to initialize glew.\n"
		     << "Reason: " << glewGetErrorString(result) << "\n";
		exit(EXIT_FAILURE);
	}

	try {
		cout << "Creating default shader program...\n";
		defaultProgram = createShaderProgram(default_vertex_shader, default_fragment_shader);
	} catch(const exception& e) {
		cerr << "Failed to create shader program from '" << default_vertex_shader << "' and '" << default_fragment_shader << "'\n"
	         << "Exception: " << e.what();
		exit(EXIT_FAILURE);
	}
}

void GLUtils::cleanup()
{
	delete defaultProgram;
}

tdogl::Program *createShaderProgram(const string& vertexShaderFile, const string& fragmentShaderFile)
{
	vector<tdogl::Shader> shaderList;
	shaderList.push_back(tdogl::Shader::shaderFromFile(vertexShaderFile.c_str(), GL_VERTEX_SHADER));
	shaderList.push_back(tdogl::Shader::shaderFromFile(fragmentShaderFile.c_str(), GL_FRAGMENT_SHADER));
	return new tdogl::Program(shaderList);
}


// ----------------------------------------------------------------------------

GLUtils::Mesh::Mesh() {}
GLUtils::Mesh::~Mesh() 
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}

GLUtils::Sphere::Sphere() {}

void GLUtils::Sphere::init()
{
	init(1.f, 10, 10);
}

void GLUtils::Sphere::init( float radius, unsigned int rings, unsigned int sectors )
{
	const float R = 1.f / (float)(rings - 1);
	const float S = 1.f / (float)(sectors - 1);
	const float PI = 3.1415927f;

	// Create vertex array data
	std::vector<glm::vec3> verts(rings * sectors);// * 3);
	std::vector<glm::vec3> normals(rings * sectors);// * 3);
	std::vector<glm::vec2> texcoords(rings * sectors);// * 2);
	auto v = begin(verts);
	auto n = begin(normals);
	auto t = begin(texcoords);
	for(unsigned int r = 0; r < rings; r++)
	for(unsigned int s = 0; s < sectors; s++) {
		const float y = sin( -2*PI + PI * r * R );
		const float x = cos( 2*PI * s * S ) * sin( PI * r * R );
		const float z = sin( 2*PI * s * S ) * sin( PI * r * R );

		*v++ = glm::vec3(x,y,z) * radius;
		*n++ = glm::vec3(x,y,z);
		*t++ = glm::vec2(s*S, r*R);
	}

	// Interleave vertex buffer data
	vertices.resize(3 * verts.size() + 3 * normals.size() + 2 * texcoords.size());
	auto d = begin(vertices);
	v = begin(verts);
	n = begin(normals);
	t = begin(texcoords);
	while (v != end(verts)) {
		// Location 0 - vertices
		vertices.push_back(v->x);
		vertices.push_back(v->y);
		vertices.push_back(v->z);
		//*d++ = v->x;
		//*d++ = v->y;
		//*d++ = v->z;
		// Location 1 - texcoords
		vertices.push_back(t->s);
		vertices.push_back(t->t);
		//*d++ = t->s;
		//*d++ = t->t;
		// Location 2 - normals
		vertices.push_back(n->x);
		vertices.push_back(n->y);
		vertices.push_back(n->z);
		//*d++ = n->x;
		//*d++ = n->y;
		//*d++ = n->z;
		// next set of vectors
		++v;
		++n;
		++t;
	}

	// Create vertex array indices
	indices.resize(rings * sectors * 4);
	std::vector<unsigned short>::iterator i = begin(indices);
	for(unsigned int r = 0; r < rings-1; r++)
	for(unsigned int s = 0; s < sectors-1; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s+1);
		*i++ = (r+1) * sectors + (s+1);
		*i++ = (r+1) * sectors + s;
	}		

	// Get shader attribute locations
	const GLuint vertexAttribLoc   = defaultProgram->attrib("vertex");
	const GLuint texcoordAttribLoc = defaultProgram->attrib("texcoord");
	const GLuint normalAttribLoc   = defaultProgram->attrib("normal");

	// Create vertex buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texcoordAttribLoc);
	glEnableVertexAttribArray(normalAttribLoc);

	const GLsizei stride = 8 * sizeof(GLfloat);
	const GLvoid *tex_coord_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	const GLvoid *normal_offset = (const GLvoid *) (5 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(texcoordAttribLoc, 2, GL_FLOAT, GL_TRUE, stride, tex_coord_offset);
	glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_TRUE, stride, normal_offset);

	// Create index buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBuffer), &indexBuffer, GL_STATIC_DRAW);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLUtils::Sphere::render() const
{
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

	// Assumes shader is bound and ready to go...
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_QUADS, 100, GL_UNSIGNED_SHORT, 0);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

