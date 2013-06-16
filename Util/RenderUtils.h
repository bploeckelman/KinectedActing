#pragma once
/************************************************************************/
/* Render 
/* ------
/* A namespace containing functions for rendering various things 
/************************************************************************/
#include <glm/glm.hpp>


namespace Render
{
	// Initialize buffers, textures, etc...
	void init();

	// Draw a basic cube at the specified position with the specified scale
	void cube(const glm::vec3& position=glm::vec3(0,0,0)
	        , const float scale=1.f);

	// Draw the specified plane as a quad with the specified radius
	//static void plane(const Plane& plane, const float radius=10.f);
	void ground(const float alpha = 1.f);

	//draw pyramid
	void pyramid(const glm::vec3& pos
	           , const float radius
			   , const float height);

	// Draw a vector from the specified point
	void vector(const glm::vec3& vec
	          , const glm::vec3& point
			  , const glm::vec4& color=glm::vec4(1,1,1,1));

	// Draw the specified coordinate axes at the specified position, scaling vectors by specified scale
	void basis(const float scale=1.f
	         , const glm::vec3& pos=glm::vec3(0,0,0)
	         , const glm::vec3& x=glm::vec3(1,0,0)
	         , const glm::vec3& y=glm::vec3(0,1,0)
	         , const glm::vec3& z=glm::vec3(0,0,1)
	         , const float alpha=1.f);
};
