#version 330

layout(location=0) in vec4 vVertex;

out flat float id;

void main() 
{
    gl_Position = vVertex;
	id = gl_VertexID*0.33;
}