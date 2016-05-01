#include <stdio.h>
#include <string.h>

#include "technique.h"

Technique::Technique(){
    m_shaderProg = 0;
}

Technique::~Technique(){
    for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++){
        glDeleteShader(*it);
    }

    if (m_shaderProg != 0){
        glDeleteProgram(m_shaderProg);
        m_shaderProg = 0;
    }
}

bool Technique::Init(){
    m_shaderProg = glCreateProgram();

    if (m_shaderProg == 0){
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    return true;
}


bool Technique::LoadShaderText(string& str, const char *fn)
{
	// пробуем открыть файл
	ifstream in(fn);
	//если все плохо выводим в консоль сообщение об ошибке
	if (!in.is_open())
	{
		cout << "File " << fn << " cannon be opened" << endl;

		return false;
	}
	//временный буфер на 300 символов
	char tmp[300];
	
	// построчно читаем файл шейдера и записываем в str
	while (!in.eof())
	{
		in.getline(tmp, 300);
		str += tmp;
		str += '\n';
	}
	return true;
}

//Используем этот метод для добавления шейдеров в программу. Когда заканчиваем - вызываем finalize()
bool Technique::AddShader(GLenum ShaderType, const char* fn){
	string ShaderStr;
	LoadShaderText(ShaderStr, fn);
	const char *pShaderText = ShaderStr.c_str();
    GLuint ShaderObj = glCreateShader(ShaderType);



    if (ShaderObj == 0){
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        return false;
    }

    // Сохраним объект шейдера - он будет удален в декструкторе
    m_shaderObjList.push_back(ShaderObj);

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success){
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        return false;
    }

    glAttachShader(m_shaderProg, ShaderObj);

    return true;
}

// После добавления всех шейдеров в программу вызываем эту функцию
// для линковки и проверки программу на ошибки
bool Technique::Finalize(){
    GLint Success = 0;
    GLchar ErrorLog[1024] = {0};

    glLinkProgram(m_shaderProg);

    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0){
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
    if (Success == 0){
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    // Удаляем промежуточные объекты шейдеров, которые были добавлены в программу
    for (ShaderObjList::iterator it = m_shaderObjList.begin(); it != m_shaderObjList.end(); it++){
        glDeleteShader(*it);
    }

    m_shaderObjList.clear();

    return true;
}

void Technique::Enable(){
    glUseProgram(m_shaderProg);
}

GLint Technique::GetUniformLocation(const char* pUniformName){
    GLint Location = glGetUniformLocation(m_shaderProg, pUniformName);

    if (Location == 0xFFFFFFFF){
                fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}




















