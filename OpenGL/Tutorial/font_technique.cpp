#include "font_technique.h"

FontTechnique::FontTechnique() {
}

bool FontTechnique::Init() {
	if (!Technique::Init()) {
		return false;
	}

	if (!Technique::AddShader(GL_VERTEX_SHADER, "font_renderer.vs")) {
		return false;
	}
	if (!Technique::AddShader(GL_FRAGMENT_SHADER,"font_renderer.fs")) {
		return false;
	}
	if (!Technique::Finalize()) {
		return false;
	}
	m_colorLocation = GetUniformLocation("gTextColor");
	m_worldLocation = GetUniformLocation("gProjection");
	m_colorTextureLocation = GetUniformLocation("gText");
	return true;
}
void FontTechnique::SetColor(const Vector3f Color) {
	glUniform3f(m_colorLocation, Color.x, Color.y, Color.z);
}

void FontTechnique::SetWorld(const Matrix4f World) {
	glUniformMatrix4fv(m_worldLocation, 1, GL_TRUE, (const GLfloat*)World.m);
}

void FontTechnique::SetColorTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_colorTextureLocation,TextureUnit);
}
