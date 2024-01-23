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
        int success;
        char infoLog[512];
        //glCreateShader����һ����ɫ�������㴫�ݵĲ�����GL_VERTEX_SHADER��
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertexShaderSrc = loadShaderSrc(vs);
        const GLchar* vertexShaderSource = vertexShaderSrc.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        // ��ӡ�����������еĻ���
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderSrc = loadShaderSrc(fs);
        const GLchar* fragmentShaderSource = fragmentShaderSrc.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        //3.Link VS �� FS
        
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // ��ӡ���Ӵ�������еĻ���
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

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

    unsigned int getShaderPrograme()
    {
        return shaderProgram;
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