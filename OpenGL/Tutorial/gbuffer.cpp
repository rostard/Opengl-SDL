#include "gbuffer.h"
#include "util.h"

#include <iostream>
using std::cout;
using std::endl;

GBuffer::GBuffer()
{
	m_fbo = 0;
	m_depthTexture = 0;
	ZERO_MEM(m_textures);
}

GBuffer::~GBuffer()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_textures[0] != 0) {
        glDeleteTextures(ARRAY_SIZE_IN_ELEMENTS(m_textures), m_textures);
    }

	if (m_depthTexture != 0) {
		glDeleteTextures(1, &m_depthTexture);
	}
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
	// Create the FBO
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Create the gbuffer textures
	glGenTextures(4, m_textures);
	

	for (unsigned int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);

	}
	
	// depth
	//glGenTextures(1, &m_depthTexture);
	//glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

	GLuint DrawBuffers[4] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(4, DrawBuffers);

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth, WindowWidth);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// restore default FBO
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	
}

void GBuffer::BindForReading()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
	
}

void GBuffer::SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType)
{
	glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}
