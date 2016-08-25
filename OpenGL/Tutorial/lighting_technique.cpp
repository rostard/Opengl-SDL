#include <limits.h>
#include <string.h>

#include "lighting_technique.h"
#include "util.h"

#define snprintf _snprintf_s

LightingTechnique::LightingTechnique(){
}

bool LightingTechnique::Init()
{
    if (!Technique::Init())
    {
        return false;
    }


    if (!AddShader(GL_VERTEX_SHADER, "Lighting.vs"))
    {
        return false;
    }

	/*if (!AddShader(GL_TESS_CONTROL_SHADER, "Lighting.tcs"))
	{
		return false;
	}

	if (!AddShader(GL_TESS_EVALUATION_SHADER, "Lighting.tes"))
	{
		return false;
	}*/

    if (!AddShader(GL_FRAGMENT_SHADER, "Lighting.fs"))
    {
        return false;
    }

    if (!Finalize())
    {
        return false;
    }

	//m_VPLocation = GetUniformLocation("gVP");
	//m_WVPLocation = GetUniformLocation("gWVP");
	//m_WorldMatrixLocation = GetUniformLocation("gWorld");
	m_colorMapLocation = GetUniformLocation("gColorMap");
	//m_normalMapLocation = GetUniformLocation("gNormalMap");
	//m_displacementMapLocation = GetUniformLocation("gDisplacementMap");
	m_eyeWorldPosLocation = GetUniformLocation("gEyeWorldPos"); 
	m_dirLightLocation.Color = GetUniformLocation("gDirectionalLight.Base.Color");
	m_dirLightLocation.AmbientIntensity = GetUniformLocation("gDirectionalLight.Base.AmbientIntensity");
	m_dirLightLocation.Direction = GetUniformLocation("gDirectionalLight.Direction");
	m_dirLightLocation.DiffuseIntensity = GetUniformLocation("gDirectionalLight.Base.DiffuseIntensity");
	m_matSpecularIntensityLocation = GetUniformLocation("gMatSpecularIntensity");
	m_matSpecularPowerLocation = GetUniformLocation("gSpecularPower");
	m_numPointLightsLocation = GetUniformLocation("gNumPointLights");
	m_numSpotLightsLocation = GetUniformLocation("gNumSpotLights");
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_colorLocation); i++) {
		char Name[32];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "gColor[%d]", i);

		m_colorLocation[i] = GetUniformLocation(Name);

		if (m_colorLocation[i] == INVALID_UNIFORM_LOCATION) {
			return false;
		}
	}
	//m_LightWVPLoaction = GetUniformLocation("gLightWVP");
	//m_shadowMapLocation= GetUniformLocation("gShadowMap");
	//m_TLLocation = GetUniformLocation("gTesselationLevel");
	//m_dispFactorLocation = GetUniformLocation("gDispFactor");
	if (m_dirLightLocation.AmbientIntensity == INVALID_UNIFORM_LOCATION ||
		m_WVPLocation == INVALID_UNIFORM_LOCATION ||
		m_LightWVPLoaction == INVALID_UNIFORM_LOCATION ||
		m_WorldMatrixLocation == INVALID_UNIFORM_LOCATION ||
		m_colorMapLocation == INVALID_UNIFORM_LOCATION ||
		m_shadowMapLocation == INVALID_UNIFORM_LOCATION ||
		m_displacementMapLocation == INVALID_UNIFORM_LOCATION ||
		m_eyeWorldPosLocation == INVALID_UNIFORM_LOCATION ||
		m_dirLightLocation.Color == INVALID_UNIFORM_LOCATION ||
		m_dirLightLocation.DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
		m_dirLightLocation.Direction == INVALID_UNIFORM_LOCATION ||
		m_matSpecularIntensityLocation == INVALID_UNIFORM_LOCATION ||
		m_matSpecularPowerLocation == INVALID_UNIFORM_LOCATION ||
		m_numPointLightsLocation == INVALID_UNIFORM_LOCATION ||
		m_numSpotLightsLocation == INVALID_UNIFORM_LOCATION ||
		m_normalMapLocation == INVALID_UNIFORM_LOCATION ||
		m_dispFactorLocation== INVALID_UNIFORM_LOCATION) {
		//return false;
	}


	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLightsLocation); i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
		m_pointLightsLocation[i].Color = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
		m_pointLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Position", i);
		m_pointLightsLocation[i].Position = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
		m_pointLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
		m_pointLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
		m_pointLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
		m_pointLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

		if (m_pointLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
			m_pointLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
			return false;
		}
	}

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_spotLightsLocation); i++) {
		char Name[128];
		memset(Name, 0, sizeof(Name));
		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
		m_spotLightsLocation[i].Color = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
		m_spotLightsLocation[i].AmbientIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Position", i);
		m_spotLightsLocation[i].Position = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
		m_spotLightsLocation[i].Direction = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
		m_spotLightsLocation[i].Cutoff = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
		m_spotLightsLocation[i].DiffuseIntensity = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
		m_spotLightsLocation[i].Atten.Constant = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
		m_spotLightsLocation[i].Atten.Linear = GetUniformLocation(Name);

		snprintf(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
		m_spotLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

		if (m_spotLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Direction == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Cutoff == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
			m_spotLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
			return false;
		}
	}

    return true;
}

void LightingTechnique::SetVP(const Matrix4f& VP)
{
	glUniformMatrix4fv(m_VPLocation, 1, GL_TRUE, (const GLfloat*)VP.m);
}

void LightingTechnique::SetLightWVP(const Matrix4f& LightWVP)
{
	glUniformMatrix4fv(m_LightWVPLoaction, 1, GL_TRUE, (const GLfloat*)LightWVP.m);
}

void LightingTechnique::SetShadowMapTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_shadowMapLocation, TextureUnit);
}

void LightingTechnique::SetWVP(const Matrix4f& WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, (const GLfloat*)WVP.m);
}


void LightingTechnique::SetWorldMatrix(const Matrix4f& WorldInverse)
{
	glUniformMatrix4fv(m_WorldMatrixLocation,1,GL_TRUE, (const GLfloat*)WorldInverse.m);
}

void LightingTechnique::SetColorTextureUnit(unsigned int TextureUnit)
{
    glUniform1i(m_colorMapLocation, TextureUnit);
}

void LightingTechnique::SetNormalMapTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_normalMapLocation, TextureUnit);
}

void LightingTechnique::SetDisplacementTextureUnit(unsigned int TextureUnit)
{
	glUniform1i(m_displacementMapLocation, TextureUnit);
}

void LightingTechnique::SetMatSpecularIntensity(float Intensity)
{
	glUniform1f(m_matSpecularIntensityLocation, Intensity);
}

void LightingTechnique::SetMatSpecularPower(float Power)
{
	glUniform1f(m_matSpecularPowerLocation, Power);
}

void LightingTechnique::SetEyeWorldPos(const Vector3f& EyeWorldPos)
{
	glUniform3f(m_eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z);
}

void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light)
{
    glUniform3f(m_dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z);
    glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
	Vector3f Direction = Light.Direction;
	Direction.Normalize();
	glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
	glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}

void LightingTechnique::SetPointLights(unsigned int NumLights, const PointLight* pLights)
{
	glUniform1i(m_numPointLightsLocation, NumLights);

	for (unsigned int i = 0; i < NumLights; i++)
	{
		glUniform3f(m_pointLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
		glUniform1f(m_pointLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
		glUniform1f(m_pointLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
		glUniform3f(m_pointLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
		glUniform1f(m_pointLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
		glUniform1f(m_pointLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
		glUniform1f(m_pointLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
	}
}

void LightingTechnique::SetSpotLights(unsigned int NumLights, const SpotLight* pLights)
{
	glUniform1i(m_numSpotLightsLocation, NumLights);

	for (unsigned int i = 0; i < NumLights; i++) {
		glUniform3f(m_spotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
		glUniform1f(m_spotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
		glUniform1f(m_spotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
		glUniform3f(m_spotLightsLocation[i].Position, pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z);
		Vector3f Direction = pLights[i].Direction;
		Direction.Normalize();
		glUniform3f(m_spotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
		glUniform1f(m_spotLightsLocation[i].Cutoff, cosf(ToRadian(pLights[i].Cutoff)));
		glUniform1f(m_spotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
		glUniform1f(m_spotLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
		glUniform1f(m_spotLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
	}
}

void LightingTechnique::SetDispFactor(float Factor)
{
	glUniform1f(m_dispFactorLocation, Factor);
}

void LightingTechnique::SetTesselationLevel(float TL)
{
	glUniform1f(m_TLLocation, TL);
}

void LightingTechnique::SetColor(unsigned int Index, Vector4f& Color) {
	glUniform4f(m_colorLocation[Index], Color.x, Color.y, Color.z, Color.w);
}