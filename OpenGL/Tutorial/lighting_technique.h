#ifndef LIGHTINGTECHNIQUE_H
#define LIGHTINGTECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

struct BaseLight
{
	Vector3f Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = Vector3f(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};

struct DirectionalLight : public BaseLight
{
	Vector3f Direction;

	DirectionalLight()
	{
		Direction = Vector3f(0.0f, 0.0f, 0.0f);
	}
};

struct PointLight : public BaseLight
{
	Vector3f Position;

	struct
	{
		float Constant;
		float Linear;
		float Exp;
	}Attenuation;

	PointLight()
	{
		Position = Vector3f(0.0f, 0.0f, 0.0f);
		Attenuation.Constant = 1.0f;
		Attenuation.Linear = 0.0f;
		Attenuation.Exp = 0.0;
	}
};

struct SpotLight : public PointLight
{
	Vector3f Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = Vector3f(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}
};


class LightingTechnique : public Technique
{
    public:

		static const unsigned int MAX_POINT_LIGHTS = 2;
		static const unsigned int MAX_SPOT_LIGHTS = 2;

        LightingTechnique();

        virtual bool Init();

		void SetVP(const Matrix4f& VP);
        void SetWVP(const Matrix4f& WVP);
		void SetLightWVP(const Matrix4f& LightWVP);
		void SetWorldMatrix(const Matrix4f& WorldInverse);

        void SetColorTextureUnit(unsigned int TextureUnit);
		void SetNormalMapTextureUnit(unsigned int TextureUnit);
		void SetShadowMapTextureUnit(unsigned int TextureUnit);
		void SetDisplacementTextureUnit(unsigned int TextureUnit);

        void SetDirectionalLight(const DirectionalLight& Light);
		void SetPointLights(unsigned int NumLights, const PointLight* pLights);
		void SetSpotLights(unsigned int NumLights, const SpotLight* pLights);

		void SetEyeWorldPos(const Vector3f& EyeWorldPos);
		void SetMatSpecularIntensity(float Intensity);
		void SetMatSpecularPower(float Power);

		void SetDispFactor(float Factor);
		
    private:
		GLuint m_VPLocation;
        GLuint m_WVPLocation;
		GLuint m_LightWVPLoaction;
		GLuint m_WorldMatrixLocation;

        GLuint m_colorMapLocation;
		GLuint m_shadowMapLocation;
		GLuint m_normalMapLocation;
		GLuint m_displacementMapLocation;
		GLuint m_eyeWorldPosLocation;
		GLuint m_matSpecularIntensityLocation;
		GLuint m_matSpecularPowerLocation;
		GLuint m_numPointLightsLocation;
		GLuint m_numSpotLightsLocation;
		GLuint m_dispFactorLocation;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		}m_dirLightLocation;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint Position;
			struct{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} m_pointLightsLocation[MAX_POINT_LIGHTS];

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint Position;
			GLuint Direction;
			GLuint Cutoff;
			struct{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} m_spotLightsLocation[MAX_SPOT_LIGHTS];
};

#endif // LIGHTINGTECHNIQUE_H
