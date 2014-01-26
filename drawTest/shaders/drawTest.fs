#version 330

uniform sampler2D tex;

in vec4 outColor;

out vec4 vFragColor;

void main() 
{
	vFragColor = texture(tex, gl_PointCoord) * outColor;
}