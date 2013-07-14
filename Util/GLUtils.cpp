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
tdogl::Program *GLUtils::simpleProgram = nullptr;

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
		cout << "Creating simple shader program...\n";
		simpleProgram = createShaderProgram(simple_vertex_shader, simple_fragment_shader);
	} catch(const exception& e) {
		cerr << "Failed to create shader program\n Exception: " << e.what();
		exit(EXIT_FAILURE);
	}
}

void GLUtils::cleanup()
{
	delete simpleProgram;
	delete defaultProgram;
}

tdogl::Program *createShaderProgram(const string& vertexShaderFile, const string& fragmentShaderFile)
{
	vector<tdogl::Shader> shaderList;
	shaderList.push_back(tdogl::Shader::shaderFromFile(vertexShaderFile.c_str(), GL_VERTEX_SHADER));
	shaderList.push_back(tdogl::Shader::shaderFromFile(fragmentShaderFile.c_str(), GL_FRAGMENT_SHADER));
	return new tdogl::Program(shaderList);
}
