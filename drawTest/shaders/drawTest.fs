#version 440

uniform sampler2D tex;

in vec4 outColor;

out vec4 vFragColor;

void main() 
{
	//vFragColor = texture(tex, gl_PointCoord) * vec4(1.0);//outColor;
	vFragColor = outColor;
}