#version 330

uniform mat4 modelviewMat;
uniform mat4 projectionMat;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

out vec4 outColor;

void main() 
{
    gl_Position = projectionMat * modelviewMat * vVertex;
	outColor = vColor;
}