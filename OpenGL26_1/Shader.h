#pragma once
#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件

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
    Shader(const char* vs, const char* fs, const char* gs = nullptr)
    {
        int success;
        char infoLog[512];
        //glCreateShader创建一个着色器，顶点传递的参数是GL_VERTEX_SHADER。

        // vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertexShaderSrc = loadShaderSrc(vs);
        const GLchar* vertexShaderSource = vertexShaderSrc.c_str();
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, "Vertex");// 打印编译错误（如果有的话）
        // fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderSrc = loadShaderSrc(fs);
        const GLchar* fragmentShaderSource = fragmentShaderSrc.c_str();
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, "Fragment");
        // geometry shader
        if (gs != nullptr)
        {
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            std::string geometryShaderSrc = loadShaderSrc(gs);
            const GLchar* geometryShaderSource = geometryShaderSrc.c_str();
            glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
            glCompileShader(geometryShader);
            checkCompileErrors(geometryShader, "Geometry");
        }
        //3.Link VS 和 FS

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        if (gs != nullptr)
            glAttachShader(shaderProgram, geometryShader);
        glLinkProgram(shaderProgram);


        
        checkCompileErrors(shaderProgram, "Program");// 打印连接错误（如果有的话）

        //4. 删除Shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (gs != nullptr)
            glDeleteShader(geometryShader);
    }

    ~Shader()
    {
        deleteShaderPrograme();
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
    std::string loadShaderSrc(const char* filename) //导入glsl文件
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
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
    }
private:

    void checkCompileErrors(const GLuint shader, const std::string& type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int geometryShader;
    unsigned int shaderProgram;
};