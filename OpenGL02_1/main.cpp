#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#pragma endregion
//============================ Function(End) ============================

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

    //2.定义VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);//绑定VAO
//3.定义VBO
    /*

    */
    unsigned int VBO;
    glGenBuffers(1, &VBO);//glGenBuffers和缓冲ID生成一个VBO对象

    //顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER，用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)。
    //调用glBufferData函数，把之前定义的顶点数据复制到缓冲的内存中
    //glBufferData 参数1：目标缓冲的类型：GL_ARRAY_BUFFER
    //             参数2：指定传输数据的大小(以字节为单位)；sizeof计算
    //             参数3：希望发送的实际数据
    //             参数4：显卡管理给定数据的方式： GL_STATIC_DRAW（数据不会或几乎不会改变）。
    //                                          GL_DYNAMIC_DRAW（数据会被改变很多）。
    //                                          GL_STREAM_DRAW（数据每次绘制时都会改变）。
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //4.定义EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //5.创建指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);//启用顶点属性

    /*
    // glVertexAttribPointer已经定义了顶点属性，解绑VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO也可以解绑，其他的VAO不会修改当前的VAO，但很少发生，所以一般很少使用解绑VAO和VBO
    glBindVertexArray(0);
    */
#pragma endregion
    //============================ 顶点数据(end) ============================




    //============================ shader(start) ============================
#pragma region
//1.VS
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
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
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //3.Link VS 和 FS
    unsigned int shaderProgram;
    //glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用。
    shaderProgram = glCreateProgram();
    //把之前编译的着色器附加到程序对象上，然后用glLinkProgram链接它们：
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    /*
    //检测编译时错误知道错误是什么，
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        ...
    }
    */

    //4. 删除Shader
        //着色器对象链接到程序对象以后，记得删除着色器对象,不再需要
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //绘制一个三角形
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //绘制两个三角形：使用VBO
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //绘制两个三角形：使用EBO，参数1：绘制的模式，参数2：顶点数，参数3：索引的类型，参数4：偏移量
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
