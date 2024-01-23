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
#pragma endregion
//============================ Function(End) ============================

/*
 //每次都定义字符串比较麻烦，改成从glsl文件读取
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
    //============================ 初始化(start) ============================
#pragma region
    //实例化GLFW窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//自适应窗口大小
#pragma endregion
    //============================ 初始化(end) ============================


    //============================ 顶点数据(start) ============================
#pragma region
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);//顶点顺时针显示反面GL_BACK，逆时针显示正面GL_FRONT

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//默认模式：填充颜色
//1.定义顶点数据
    //1.1绘制一个三角形
    /*
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f
     };
    */

    //1.2 绘制两个三角形
    /*
    float vertices[] = {
        // 第一个三角形
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, 0.5f, 0.0f,  // 左上角
        // 第二个三角形
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
     };
    */

    //1.3 使用EBO绘制两个三角形
    float vertices[] = {
    0.5f, 0.5f, 0.0f,   // 右上角
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {
        // 注意索引从0开始! 
        // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        // 这样可以由下标代表顶点组合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    //2.定义VAO, VBO, EBO
    unsigned int VAO, VBO, EBO;
    
    //3.产生Buffer和VertexArray
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象
    glGenBuffers(1, &EBO);
    
    //4.绑定Buffer和VertexArray
    glBindVertexArray(VAO);//绑定VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    //5.创建指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);//启用顶点属性

#pragma endregion
    //============================ 顶点数据(end) ============================





    //============================ shader(start) ============================
#pragma region
//1.VS

    unsigned int vertexShader;
    //glCreateShader创建一个着色器，顶点传递的参数是GL_VERTEX_SHADER。
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //着色器源码附加到着色器对象上，然后编译它
    //glShaderSource函数参数1：要编译的着色器对象。
    //                  参数2：指定了传递的源码字符串数量，这里只有1个。
    //                  参数3：顶点着色器真正的源码，
    //                  参数4：我们先设置为NULL。
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    /*
    //检测编译时错误知道错误是什么，
    int success;
    char infoLog[512];
    //glGetShaderiv检查是否编译成功。如果编译失败，用glGetShaderInfoLog获取错误消息，然后打印它。
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

    //3.Link VS 和 FS
    unsigned int shaderProgram[2];
    //glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用。
    shaderProgram[0] = glCreateProgram();
    //把之前编译的着色器附加到程序对象上，然后用glLinkProgram链接它们：
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader[0]);
    glLinkProgram(shaderProgram[0]);

    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader[1]);
    glLinkProgram(shaderProgram[1]);

    //检测编译时错误知道错误是什么，
    /*
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram[0], 512, NULL, infoLog);
    }
    */

    

    //4. 删除Shader
        //着色器对象链接到程序对象以后，记得删除着色器对象,不再需要
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader[0]);
    glDeleteShader(fragmentShader[1]);
#pragma endregion
    //============================ shader(end) ============================


    //============================ 渲染循环(start) ============================
#pragma region
    while (!glfwWindowShouldClose(window))
    {
        //输入
        processInput(window);

        // 渲染指令-类似黑板的颜色
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //使用glUseProgram激活程序对象
        //在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象（也就是之前写的着色器)
        glUseProgram(shaderProgram[0]);
        glBindVertexArray(VAO);
        //绘制一个三角形
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //绘制两个三角形：使用VBO
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //绘制两个三角形：使用EBO，参数1：绘制的模式，参数2：顶点数，参数3：索引的类型，参数4：偏移量
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram[1]);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3*sizeof(unsigned int)));
        //glBindVertexArray(0);//不需要一直解绑
        //交换前后帧
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
    //============================ 渲染循环(end) ============================
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram[0]);
    glDeleteProgram(shaderProgram[1]);
    glfwTerminate();
    return 0;
}
