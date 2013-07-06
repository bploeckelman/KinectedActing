/************************************************************************/
/* Render 
/* ------
/* A namespace containing functions for rendering various things 
/************************************************************************/
#include <GL/glew.h>

#include "RenderUtils.h"
#include "GLUtils.h"
#include "Shaders/Program.h"
#include "Scene/Objects/CubeMesh.h"
//#include "ImageManager.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

using glm::vec3;
using glm::vec4;
using glm::value_ptr;

std::shared_ptr<CubeMesh> cubeMesh;

GLuint quad_vao;
GLuint quad_vbo;
const GLfloat buffer_data[] = {
	// triangle strip quad
	//  X     Y     Z       U     V        nX    nY    nZ
	 -1.0f,  1.0f, 0.0f,   1.0f, 0.0f,//    0.0f, 0.0f, 1.0f,
	 -1.0f, -1.0f, 0.0f,   1.0f, 1.0f,//    0.0f, 0.0f, 1.0f,
	  1.0f,  1.0f, 0.0f,   0.0f, 0.0f,//    0.0f, 0.0f, 1.0f,
	  1.0f, -1.0f, 0.0f,   0.0f, 1.0f //,    0.0f, 0.0f, 1.0f
};


void loadBufferObjects()
{
	// FIXME: quad_vbo becomes invalidated somewhere between here and first use... 
	// Create and bind vao
	glGenVertexArrays(1, &quad_vao);
	glBindVertexArray(quad_vao);

	// Create + bind vbo, upload buffer data
	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);

	// Enable and set up vertex attributes
	const GLuint vertexAttribLoc   = GLUtils::defaultProgram->attrib("vertex");
	const GLuint texcoordAttribLoc = GLUtils::defaultProgram->attrib("texcoord");
	//const GLuint normalAttribLoc = GLUtils::defaultProgram->attrib("normal");
	glEnableVertexAttribArray(vertexAttribLoc);
	glEnableVertexAttribArray(texcoordAttribLoc);
	//glEnableVertexAttribArray(normalAttribLoc);

	const GLsizei stride = 5 * sizeof(GLfloat);
	//const GLsizei stride = 8 * sizeof(GLfloat);
	const GLvoid *tex_coord_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	//const GLvoid *normal_offset = (const GLvoid *) (5 * sizeof(GLfloat));
	glVertexAttribPointer(vertexAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(texcoordAttribLoc, 2, GL_FLOAT, GL_TRUE, stride, tex_coord_offset);
	//glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_TRUE, stride, normal_offset);

	//glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertexAttribLoc);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Render::init()
{
	loadBufferObjects();
	cubeMesh = std::shared_ptr<CubeMesh>(new CubeMesh("cube"));
}

void Render::cleanup()
{
	glDeleteBuffers(1, &quad_vbo);
	//glDeleteVertexArrays(1, &quad_vao);
}

void Render::quad()
{
	// FIXME: tri_vao becomes invalidated between creation and here... 
	// a call to glIsVertexArray(quad_vao) returns true in init() after glGenVertexArrays(),
	// but checking it again here returns false, and I'm not sure where it gets broken
	//glBindVertexArray(quad_vao);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//glBindVertexArray(0);

	// Have to do things the old fashioned way until I figure out whats up with the VAO
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

	const GLuint vertAttribLoc = GLUtils::defaultProgram->attrib("vertex");
	const GLuint texcoordAttribLoc = GLUtils::defaultProgram->attrib("texcoord");
	//const GLuint normalAttribLoc = GLUtils::defaultProgram->attrib("normal");
	glEnableVertexAttribArray(vertAttribLoc);
	glEnableVertexAttribArray(texcoordAttribLoc);
	//glEnableVertexAttribArray(normalAttribLoc);

	const GLsizei stride = 5 * sizeof(GLfloat);
	//const GLsizei stride = 8 * sizeof(GLfloat);
	const GLvoid *tex_coord_offset = (const GLvoid *) (3 * sizeof(GLfloat));
	//const GLvoid *normal_offset = (const GLvoid *) (5 * sizeof(GLfloat));
	glVertexAttribPointer(vertAttribLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glVertexAttribPointer(texcoordAttribLoc, 2, GL_FLOAT, GL_TRUE,  stride, tex_coord_offset);
	//glVertexAttribPointer(normalAttribLoc, 3, GL_FLOAT, GL_TRUE,  stride, normal_offset);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glDisableVertexAttribArray(normalAttribLoc);
	glDisableVertexAttribArray(texcoordAttribLoc);
	glDisableVertexAttribArray(vertAttribLoc);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Render::cube() 
{
	cubeMesh->render();
}

void Render::pyramid( const vec3& pos, const float radius, const float height )
{
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);

	glPushMatrix();
		glRotatef(180, 1, 0, 0);
		glBegin(GL_QUADS);
			glVertex3d(-radius, 0, -radius);
			glVertex3d(-radius, 0,  radius);
			glVertex3d( radius, 0,  radius);
			glVertex3d( radius, 0, -radius);
		glEnd();
	glPopMatrix();

	glBegin(GL_TRIANGLE_FAN);
		glVertex3d(0, height, 0);
		glVertex3d(-radius, 0, -radius);
		glVertex3d(-radius, 0,  radius);
		glVertex3d( radius, 0,  radius);
		glVertex3d( radius, 0, -radius);
		glVertex3d(-radius, 0, -radius);
	glEnd();

	glPopMatrix();
}

void Render::vector( const glm::vec3& vec, const glm::vec3& point , const glm::vec4& color/*=glm::vec4(1,1,1,1)*/ )
{
	glColor4fv(value_ptr(color));

	glBegin(GL_LINES);
		glVertex3fv(value_ptr(point));
		glVertex3fv(value_ptr(point + vec));
	glEnd();

	glBegin(GL_POINTS);
		glVertex3fv(value_ptr(point + vec));
	glEnd();

	glColor4f(1,1,1,1);
}

void Render::basis( const float scale/*=1.f */
				  , const vec3& pos/*=vec3(0,0,0) */
				  , const vec3& x/*=vec3(1,0,0) */
				  , const vec3& y/*=vec3(0,1,0) */
				  , const vec3& z/*=vec3(0,0,1)*/
				  , const float alpha/*=1.f*/ )
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//static const vec3 origin(0,0,0);
	const vec4 red   (1,0,0,alpha);
	const vec4 green (0,1,0,alpha);
	const vec4 blue  (0,0,1,alpha);

	vector(x * scale, pos, red);
	vector(y * scale, pos, green);
	vector(z * scale, pos, blue);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void Render::ground( const float alpha )
{
	static const float Y = 0.f;
	static const float R = 10.5f;

	//glDisable(GL_LIGHTING);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//texture.bind(&texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//const float radius = 100.f;
	glBegin(GL_TRIANGLE_STRIP);
		/*glNormal3f(0, 1, 0);*/ glTexCoord2f(0.f, 0.f); glVertex3f( R, Y,  R);
		/*glNormal3f(0, 1, 0);*/ glTexCoord2f(100.f, 0.f); glVertex3f( R, Y, -R);
		/*glNormal3f(0, 1, 0);*/ glTexCoord2f(0.f, 100.f); glVertex3f(-R, Y,  R);
		/*glNormal3f(0, 1, 0);*/ glTexCoord2f(100.f, 100.f); glVertex3f(-R, Y, -R);
	glEnd();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glBindTexture(GL_TEXTURE_2D, 0);

	//glDisable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_LIGHTING);
}
