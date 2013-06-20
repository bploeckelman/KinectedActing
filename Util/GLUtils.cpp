/************************************************************************/
/* GLUtils 
/* -------
/* A namespace containing functions for glsl shader manipulation
/* Note: derived from framework @ http://www.arcsynthesis.org/gltut/
/************************************************************************/
#include "GLUtils.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


GLuint GLUtils::defaultProgram;
GLint GLUtils::projectionMatUniformLoc;
GLint GLUtils::modelviewMatUniformLoc;

void GLUtils::init()
{
	// Initialize glew
	GLenum result = glewInit();
	if (GLEW_OK == result) {
		if (!glewIsSupported("GL_VERSION_3_3")) {
			std::cerr << "OpenGL 3.0 is not supported\n";
			exit(1);
		}
		std::cout << "GLEW initialized: " << glewGetString(GLEW_VERSION) << "\n";
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	} else {
		std::cerr << "Failed to initialize glew.\n"
	              << "Reason: " << glewGetErrorString(result) << "\n";
		exit(1);
	}

	// Initialize shaders
	cout << "Creating shader objects...\n";
	vector<GLuint> shaderList;
	shaderList.push_back(createShader(GL_VERTEX_SHADER, default_vertex_shader));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, default_fragment_shader));

	cout << "Creating shader program...\n";
	defaultProgram = createProgram(shaderList);

	cout << "Getting uniform locations...\n";
	projectionMatUniformLoc = glGetUniformLocation(GLUtils::defaultProgram, "projection_mat");
	modelviewMatUniformLoc  = glGetUniformLocation(GLUtils::defaultProgram, "modelview_mat");

	cout << "Deleting shader objects...\n";
	for_each(begin(shaderList), end(shaderList), glDeleteShader);
}

void GLUtils::cleanup()
{
	cout << "Deleting shader program...\n";
	glDeleteProgram(defaultProgram);
}

GLuint GLUtils::createShader(GLenum eShaderType, const string& shaderFileName)
{
	// Read source from shader file
	const string& shaderSource = getShaderSource(shaderFileName);
	if (shaderSource.empty()) {
		return 0;
	}

	// Get a string representation of the shader type
	const char *shaderTypeStr = nullptr;
	switch (eShaderType) {
		case GL_VERTEX_SHADER:   shaderTypeStr = "vertex";   break;
		case GL_FRAGMENT_SHADER: shaderTypeStr = "fragment"; break;
		case GL_GEOMETRY_SHADER: shaderTypeStr = "geometry"; break;
	}

	// Create the shader object and compile the source code
	GLuint shader = glCreateShader(eShaderType);
	const char *source = shaderSource.c_str();
	glShaderSource(shader, 1, &source, NULL);

	glCompileShader(shader);

	// Check for compilation errors
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, infoLog);

		cerr << "Compile failed for " << shaderTypeStr << " shader "
	         << "'" << shaderFileName << "'\n" << infoLog;
		delete[] infoLog;

		glDeleteShader(shader);
		shader = 0;
	}

	if (0 != shader) {
		cout << "\tShader source '" << shaderFileName << "' (" << shaderTypeStr << ") compiled.\n";
	}

	return shader;
}

GLuint GLUtils::createProgram(const vector<GLuint>& shaderList)
{
	// Create the program object, attach shader objects, and link it
	GLuint program = glCreateProgram();
	for (auto shader : shaderList) {
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	// Check for linker errors
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *infoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, infoLog);

		cerr << "Linker failed: \n" << infoLog;
		delete[] infoLog;
	}

	// Detach shader objects from program 
	for (auto shader : shaderList) {
		glDetachShader(program, shader);
	}

	// If the link failed, clean up the program
	if (GL_FALSE == status) {
		glDeleteProgram(program);
		program = 0;
	}

	if (0 != program) {
		cout << "\tShader program linked.\n";
	}

	return program;
}

const string GLUtils::getShaderSource(const string& shaderFileName) {
	ifstream infile(shaderFileName, ios::in);
	if (!infile.is_open()) {
		cerr << "Unable to open shader file: " << shaderFileName << "\n";
		return "";
	}

	string line;
	stringstream ss;
	while (getline(infile, line)) {
		ss << line << "\n";
	}
	infile.close();

	return ss.str();
}
