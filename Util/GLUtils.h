#pragma once
/************************************************************************/
/* GLUtils 
/* -------
/* A namespace containing functions for glsl shader manipulation
/************************************************************************/

#include <GL/glew.h>

#include <string>
#include <vector>

namespace tdogl { class Program; }


namespace GLUtils
{
	const std::string default_vertex_shader("Shaders/default.vert");
	const std::string default_fragment_shader("Shaders/default.frag");
	const std::string simple_vertex_shader("Shaders/simple.vert");
	const std::string simple_fragment_shader("Shaders/simple.frag");

	extern tdogl::Program *defaultProgram;
	extern tdogl::Program *simpleProgram;

	void init();
	void cleanup();

	GLuint createShader(GLenum eShaderType, const std::string& shaderFileName);
	GLuint createProgram(const std::vector<GLuint>& shaderList);

	const std::string getShaderSource(const std::string& shaderFileName);

} // namespace GLUtils
