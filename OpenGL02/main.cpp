#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//============================ Function(Start) ============================
#pragma region
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
#pragma endregion
//============================ Function(End) ============================

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
#pragma endregion
//============================ ��ʼ��(end) ============================


//============================ ��������(start) ============================
#pragma region
//1.���嶥������
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
            };
//2.����VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);//��VAO
//3.����VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);//glGenBuffers�ͻ���ID����һ��VBO����

    //���㻺�����Ļ���������GL_ARRAY_BUFFER����glBindBuffer�������´����Ļ���󶨵�GL_ARRAY_BUFFERĿ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //ʹ�õ��κΣ���GL_ARRAY_BUFFERĿ���ϵģ�������ö����������õ�ǰ�󶨵Ļ���(VBO)��
    //����glBufferData��������֮ǰ����Ķ������ݸ��Ƶ�������ڴ���
    //glBufferData ����1��Ŀ�껺������ͣ�GL_ARRAY_BUFFER
    //             ����2��ָ���������ݵĴ�С(���ֽ�Ϊ��λ)��sizeof����
    //             ����3��ϣ�����͵�ʵ������
    //             ����4���Կ�����������ݵķ�ʽ�� GL_STATIC_DRAW�����ݲ���򼸺�����ı䣩��
    //                                          GL_DYNAMIC_DRAW�����ݻᱻ�ı�ࣩܶ��
    //                                          GL_STREAM_DRAW������ÿ�λ���ʱ����ı䣩��
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //����ָ��
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);//���ö�������

    /*
    // glVertexAttribPointer�Ѿ������˶������ԣ����VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAOҲ���Խ��������VAO�����޸ĵ�ǰ��VAO�������ٷ���������һ�����ʹ�ý��VAO��VBO
    glBindVertexArray(0);
    */
#pragma endregion
//============================ ��������(end) ============================




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

//3.Link VS �� FS
    unsigned int shaderProgram;
    //glCreateProgram��������һ�����򣬲������´�����������ID���á�
    shaderProgram = glCreateProgram();
    //��֮ǰ�������ɫ�����ӵ���������ϣ�Ȼ����glLinkProgram�������ǣ�
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    /*
    //������ʱ����֪��������ʲô��
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        ...
    }
    */

//4. ɾ��Shader
    //��ɫ���������ӵ���������Ժ󣬼ǵ�ɾ����ɫ������,������Ҫ
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
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
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //����ǰ��֡
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
#pragma endregion
//============================ ��Ⱦѭ��(end) ============================

    glfwTerminate();
    return 0;
}