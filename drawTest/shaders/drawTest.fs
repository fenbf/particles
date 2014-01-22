#version 330

out vec4 vFragColor;

in flat float id;

void main() 
{
	vFragColor = vec4(1.0-id, 1.0, id, 1.0);
}