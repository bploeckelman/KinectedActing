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

	extern tdogl::Program *defaultProgram;

	void init();
	void cleanup();

	GLuint createShader(GLenum eShaderType, const std::string& shaderFileName);
	GLuint createProgram(const std::vector<GLuint>& shaderList);

	const std::string getShaderSource(const std::string& shaderFileName);


	class Mesh
	{
	public:
		Mesh();
		virtual ~Mesh();

		virtual void init() = 0;
		virtual void render() const = 0;

	private:
		// Non-copyable
		Mesh(const Mesh& mesh);
		Mesh& operator=(const Mesh& mesh);

	protected:
		GLuint vertexBuffer;
		GLuint indexBuffer;
		std::vector<GLfloat> vertices;
		std::vector<GLushort> indices;
	};

	class Sphere : public Mesh
	{
	public:
		Sphere();

		void init();
		void init(float radius, unsigned int rings, unsigned int sectors);
		void render() const;

	private:
		// Non-copyable
		Sphere(const Sphere& sphere);
		Sphere& operator=(const Sphere& sphere);
	};

} // namespace GLUtils
