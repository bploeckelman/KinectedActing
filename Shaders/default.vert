#version 330 compatibility

// TODO setup uniforms and remove compatibility profile
//uniform mat4 modelview_mat;
//uniform mat4 projection_mat;

layout(location = 0) in vec4 position;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * position;
}
