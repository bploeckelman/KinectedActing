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
		cout << "GLEW initialized: " << glewGetString(GLEW_VERSION) << "\n";
		cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
		cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
		cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
		cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
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
