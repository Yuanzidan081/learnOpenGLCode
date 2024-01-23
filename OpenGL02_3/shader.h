#pragma once
#include <glad/glad.h>; // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char* vs, const char* fs)
    {
        
        //glCreateShader����һ����ɫ�������㴫�ݵĲ�����GL_VERTEX_SHADER��
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertexShaderSrc = loadShaderSrc(vs);
        const GLchar* vertexShaderSource = vertexShaderSrc.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderSrc = loadShaderSrc(fs);
        const GLchar* fragmentShaderSource = fragmentShaderSrc.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        //3.Link VS �� FS
        
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        //4. ɾ��Shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void useShader()
    {
        glUseProgram(shaderProgram);
    }

    void deleteShaderPrograme()
    {
        glDeleteProgram(shaderProgram);
    }

    std::string loadShaderSrc(const char* filename) //����glsl�ļ�
    {
        std::ifstream file;
        std::stringstream buf;
        std::string ret = "";

        file.open(filename);

        if (file.is_open())
        {
            buf << file.rdbuf();
            ret = buf.str();
        }
        else
        {
            std::cout << "Could not open " << filename << std::endl;
        }
        file.close();
        return ret;
    }
private:
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
};