#pragma once
#include <glad/glad.h> // ����glad����ȡ���еı���OpenGLͷ�ļ�

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }
    void setMat4(const std::string& name, glm::mat4& trans) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(trans));
    }
    void setVec3(const std::string& name, float valueX, float valueY, float valueZ) const
    {
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), valueX, valueY, valueZ);
    }
    
private:
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;
};