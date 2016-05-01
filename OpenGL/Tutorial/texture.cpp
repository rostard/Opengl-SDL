#include <iostream>
#include "texture.h"

Texture::Texture(GLenum TextureTarget, const char *FileName)
{
	m_textureTarget = TextureTarget;
	m_fileName = &(*FileName);
	m_pImage = NULL;
}

bool Texture::Load()
{
	int COLOR_FLAG;

	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ((m_pImage = IMG_Load(m_fileName)))
	{
		if (m_pImage->format->BitsPerPixel == 24)
			COLOR_FLAG = GL_RGB; // Здесь пиксель состоит из трех байт.
		else
			COLOR_FLAG = GL_RGBA;
		/* Set the status to true */
		//Status = TRUE;

		/* Create The Texture */
		glGenTextures(1, &m_textureObject);

		/* Typical Texture Generation Using Data From The Bitmap */
		glBindTexture(m_textureTarget, m_textureObject);

		/* Generate The Texture */
		glTexImage2D(m_textureTarget, 0, COLOR_FLAG, m_pImage->w,
			m_pImage->h, -0.5, COLOR_FLAG,
			GL_UNSIGNED_BYTE, m_pImage->pixels);

		/* Linear Filtering */
		glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return true;
	}
	printf("IMG_Load: %s\n", IMG_GetError());
	return false;
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(m_textureTarget, m_textureObject);
}