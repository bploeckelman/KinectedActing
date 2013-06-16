#pragma once
/************************************************************************/
/* GLUtils 
/* -------
/* A namespace containing functions for glsl shader manipulation
/* Note: derived from framework @ http://www.arcsynthesis.org/gltut/
/************************************************************************/

#include <GL/glew.h>

#include <string>
#include <vector>


namespace GLUtils
{
	const std::string default_vertex_shader("Shaders/default.vert");
	const std::string default_fragment_shader("Shaders/default.frag");

	extern GLuint defaultProgram;

	void init();
	void cleanup();

	GLuint createShader(GLenum eShaderType, const std::string& shaderFileName);
	GLuint createProgram(const std::vector<GLuint>& shaderList);

	const std::string getShaderSource(const std::string& shaderFileName);

} // namespace GLUtils
