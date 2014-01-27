#version 330

uniform mat4x4 matModelview;
uniform mat4x4 matProjection;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

out vec4 outColor;

void main() 
{
    vec4 eyePos = matModelview * gl_Vertex;
    gl_Position = matProjection * eyePos;

	outColor = vColor;
	
	float dist = dot(eyePos.xyz, eyePos.xyz);
	float att = sqrt(1.0f / 0.5f*dist);
	gl_PointSize = 10.0f * att;
}