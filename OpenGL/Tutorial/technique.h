#ifndef TEXHNIQUE_H
#define TEXHNIQUE_H

#include <GL/glew.h>
#include <list>

#include <string>
#include <fstream>
#include <iostream>

#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF

using namespace std;

class Technique
{
    public:
        Technique();
        ~Technique();
        virtual bool Init();
        void Enable();

    protected:
		//const char *pVS;
		//const char *pFS;

		bool LoadShaderText(string& str, const char *fn);
        bool AddShader(GLenum ShaderType, const char* fn);
        bool Finalize();
        GLint GetUniformLocation(const char* pUniformName);
		GLuint m_shaderProg;
    private:
        
        typedef std::list<GLuint> ShaderObjList;
        ShaderObjList m_shaderObjList;
};

#endif /* TEXHNIQUE_H */
