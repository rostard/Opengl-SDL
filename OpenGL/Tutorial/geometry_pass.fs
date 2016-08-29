#version 330

in vec2 TexCoord0;
in vec3 WorldPos0;
in vec3 Normal0;
 
layout(location = 0)out vec3 WorldPos;
layout(location = 1)out vec3 Diffuse;
layout(location = 2)out vec3 Normal;
layout(location = 3)out vec2 TexCoord;
 
uniform samler2D gColorMap;

void main()
{
	WorldPos = WorldPos0;
	Diffuse = texture(gColorMap,TexCoord0).xyz;
	Normal = normalize(Normal);
	TexCoord = vec3(TexCoord, 0.0);
}