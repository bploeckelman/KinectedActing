#pragma once
#include <sfml/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>
#include <string>


class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	virtual void render() const = 0;

	const std::string& getName() const;

protected:
	std::string name;
	std::vector<GLfloat> vertexData;
	std::vector<GLushort> indexData;
	GLuint vertexBuffer;
	GLuint indexBuffer;

private:
	// Non-copyable
	Mesh(const Mesh& mesh);
	Mesh& operator=(const Mesh& mesh);

};


inline const std::string& Mesh::getName() const { return name; }
