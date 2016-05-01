#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <gl\glew.h>
#include <SDL\SDL.h>
#include <SDL\SDL_image.h>

class Texture
{
public:
	Texture(GLenum TextureTarget, const char *FileName);

	bool Load();

	void Bind(GLenum TextureUnit);
private:
	const char *m_fileName=NULL;
	GLenum m_textureTarget;
	GLuint m_textureObject;
	SDL_Surface* m_pImage;
};

#endif