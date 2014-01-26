#version 330

uniform mat4x4 matModelview;
uniform mat4x4 matProjection;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

out vec4 outColor;

void main() 
{
    gl_Position = matProjection * matModelview * vVertex;
	outColor = vColor;
	gl_PointSize = 10.0;
}