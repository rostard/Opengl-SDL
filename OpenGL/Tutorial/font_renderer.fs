#version 330
in vec2 TexCoords;
out vec4 FragColor; 

uniform sampler2D gText;
uniform vec3 gTextColor;

void main()
{
	vec4 sampled = vec4(1.0,1.0,1.0,texture(gText,TexCoords).r);
	FragColor = vec4(gTextColor,1.0)*sampled;
}