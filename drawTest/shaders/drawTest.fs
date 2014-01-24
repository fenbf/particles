#version 330

uniform sampler2D tex;

in vec4 outColor;

layout ( location = 0 ) out vec4 vFragColor;

void main() 
{
	vFragColor = texture(tex, gl_PointCoord.xy) * outColor;
}