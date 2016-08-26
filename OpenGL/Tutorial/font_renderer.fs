#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform samler2D text;
uniform vec3 gTextColor;

void main()
{
	vec4 sampled = vec4(1.0,1.0,1.0, texture(text,TexCoords).r);
	color= vec4(textColor,1.0)*sampled;
}