#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

//============================ Function(Start) ============================
#pragma region


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
#pragma endregion
//============================ Function(End) ============================

/*
 //ÿ�ζ������ַ����Ƚ��鷳���ĳɴ�glsl�ļ���ȡ
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(0.0f, 0.5f, 1.0f, 1.0f);\n"
"}\0";

const char* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";
*/


std::string vertexShaderSrc = loadShaderSrc("assets/vertex_core.glsl");
const GLchar* vertexShaderSource = vertexShaderSrc.c_str();
std::string fragmentShaderSrc1 = loadShaderSrc("assets/fragment1_core.glsl");
const GLchar* fragmentShaderSource1 = fragmentShaderSrc1.c_str();
std::string fragmentShaderSrc2 = loadShaderSrc("assets/fragment2_core.glsl");
const GLchar* fragmentShaderSource2 = fragmentShaderSrc2.c_str();



int main()
{
    //============================ ��ʼ��(start) ============================
#pragma region
    //ʵ����GLFW����
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //����һ�����ڶ���
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //��ʼ��GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//����Ӧ���ڴ�С
#pragma endregion
    //============================ ��ʼ��(end) ============================


    //============================ ��������(start) ============================
#pragma region
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);//����˳ʱ����ʾ����GL_BACK����ʱ����ʾ����GL_FRONT

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//�߿�ģʽ
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//Ĭ��ģʽ�������ɫ
//1.���嶥������
    //1.1����һ��������
    /*
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
     };
    */

    //1.2 ��������������
    /*
    float vertices[] = {
        // ��һ��������
        0.5f, 0.5f, 0.0f,   // ���Ͻ�
        0.5f, -0.5f, 0.0f,  // ���½�
        -0.5f, 0.5f, 0.0f,  // ���Ͻ�
        // �ڶ���������
        0.5f, -0.5f, 0.0f,  // ���½�
        -0.5f, -0.5f, 0.0f, // ���½�
        -0.5f, 0.5f, 0.0f   // ���Ͻ�
     };
    */

    //1.3 ʹ��EBO��������������
    float vertices[] = {
    0.5f, 0.5f, 0.0f,   // ���Ͻ�
    0.5f, -0.5f, 0.0f,  // ���½�
    -0.5f, -0.5f, 0.0f, // ���½�
    -0.5f, 0.5f, 0.0f   // ���Ͻ�
    };

    unsigned int indices[] = {
        // ע��������0��ʼ! 
        // ����������(0,1,2,3)���Ƕ�������vertices���±꣬
        // �����������±��������ϳɾ���

        0, 1, 3, // ��һ��������
        1, 2, 3  // �ڶ���������
    };

    //2.����VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    
    //3.����Buffer��VertexArray
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����
    glGenBuffers(1, &EBO);
    
    //4.��Buffer��VertexArray
    glBindVertexArray(VAO);//��VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //5.����ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);//���ö�������

#pragma endregion
    //============================ ��������(end) ============================





    //============================ shader(start) ============================
#pragma region
//1.VS

    unsigned int vertexShader;
    //glCreateShader����һ����ɫ�������㴫�ݵĲ�����GL_VERTEX_SHADER��
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //��ɫ��Դ�븽�ӵ���ɫ�������ϣ�Ȼ�������
    //glShaderSource��������1��Ҫ�������ɫ������
    //                  ����2��ָ���˴��ݵ�Դ���ַ�������������ֻ��1����
    //                  ����3��������ɫ��������Դ�룬
    //                  ����4������������ΪNULL��
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    /*
    //������ʱ����֪��������ʲô��
    int success;
    char infoLog[512];
    //glGetShaderiv����Ƿ����ɹ����������ʧ�ܣ���glGetShaderInfoLog��ȡ������Ϣ��Ȼ���ӡ����
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    */

    //2.FS
    unsigned int fragmentShader[2];
    fragmentShader[0] = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader[0], 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader[0]);
    fragmentShader[1] = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader[1], 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader[1]);

    //3.Link VS �� FS
    unsigned int shaderProgram[2];
    //glCreateProgram��������һ�����򣬲������´�����������ID���á�
    shaderProgram[0] = glCreateProgram();
    //��֮ǰ�������ɫ�����ӵ���������ϣ�Ȼ����glLinkProgram�������ǣ�
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader[0]);
    glLinkProgram(shaderProgram[0]);

    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader[1]);
    glLinkProgram(shaderProgram[1]);

    //������ʱ����֪��������ʲô��
    /*
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram[0], 512, NULL, infoLog);
    }
    */

    

    //4. ɾ��Shader
        //��ɫ���������ӵ���������Ժ󣬼ǵ�ɾ����ɫ������,������Ҫ
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader[0]);
    glDeleteShader(fragmentShader[1]);
#pragma endregion
    //============================ shader(end) ============================


    //============================ ��Ⱦѭ��(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //����
        processInput(window);

        // ��Ⱦָ��-���ƺڰ����ɫ
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //ʹ��glUseProgram����������
        //��glUseProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ������������Ҳ����֮ǰд����ɫ��)
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(VAO);
        //����һ��������
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //�������������Σ�ʹ��VBO
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //�������������Σ�ʹ��EBO������1�����Ƶ�ģʽ������2��������������3�����������ͣ�����4��ƫ����
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram[1]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        //glBindVertexArray(0);//����Ҫһֱ���
        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ ��Ⱦѭ��(end) ============================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram[0]);
    glDeleteProgram(shaderProgram[1]);
    glfwTerminate();
    return 0;
}
