#ifndef FONTTECHNIQUE
#define FONTTECHNIQUE

#include "technique.h"
#include "math_3d.h"
class FontTechnique : public Technique
{
public:
	FontTechnique();

	virtual bool Init();

	void SetColor(const Vector3f Color);
	void SetWorld(const Matrix4f World);
	void SetColorTextureUnit(unsigned int TextureUnit);
private:
	GLuint m_colorTextureLocation;
	GLuint m_colorLocation;
	GLuint m_worldLocation;
};

#endif