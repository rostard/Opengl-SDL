#include "ds_geom_pass_technique.h"

DSGeomPassTech::DSGeomPassTech()
{
}

bool DSGeomPassTech::Init()
{
	if (!Technique::Init()) {
		return false;
	}
	if (!Technique::AddShader(GL_VERTEX_SHADER, "geometry_pass.vs")) {
		return false;
	}
	if (!Technique::AddShader(GL_FRAGMENT_SHADER, "geometry_pass.fs")) {
		return false;
	}
	if (!Technique::Finalize()) {
		return false;
	}
	m_WVPLocation = GetUniformLocation("gWVP");
	m_WorldMatrixLocation = GetUniformLocation("gWorld");
	m_colorTextureUnitLocation = GetUniformLocation("gColorMap");

	return true;
}

void DSGeomPassTech::SetWVP(const Matrix4f & WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void DSGeomPassTech::SetWorldMatrix(const Matrix4f & WVP)
{
	glUniformMatrix4fv(m_WorldMatrixLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}

void DSGeomPassTech::SetColorTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_colorTextureUnitLocation, TextureUnit);
}
