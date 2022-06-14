#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

// 参数设置
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 将顶点着色器的源码硬编码在文件中。
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

// 将片段着色器的源码硬编码在文件中。
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";

int main() {
    /*
     * 初始化 glfw
     *
     * */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // 上下文主版本号设置为 3 TODO 为什么呢？
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // 上下文次版本号设置为 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式  TODO 为什么要使用核心模式，和其他的模式有什么区别？

#ifdef __APPLE__ // 用于 Mac OS X 系统
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // 创建一个窗口,并设置窗口大小及标题。
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate(); // 释放/删除之前的分配的所有资源
        return -1;
    }
    // 将该窗口的上下文设置为当前线程的主上下文。
    glfwMakeContextCurrent(window);


    /*
     *   初始化 glad
     *   glfwGetProcAddress会根据我们编译的系统定义正确的函数，
     *   这个函数是 用来加载系统相关的OpenGL函数指针地址的函数。
     * */

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 告诉glfw 每当窗口调整大小的时候调用这个函数。
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 创建并编译着色器
    // ------------------------------------
    // 创建顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 附加到着色器对象上，并进行编译。
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);


    // 检测是否有编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    // 检测编译是否出错
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // 链接着色器
    unsigned int shaderProgram = glCreateProgram(); // 创建程序对象
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 检查链接是否成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 已经链接完成了，不需要这写着色器对象了，对这些对象进行释放。
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 设置顶点数据(和缓冲区)并配置顶点属性
    // ------------------------------------------------------------------
    float vertices[] = {
            0.5f,  0.5f, 0.0f,  // 右上角
            0.5f, -0.5f, 0.0f,  // 右下角
            -0.5f, -0.5f, 0.0f,  // 左下角
            -0.5f,  0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {  // 索引从0开始
            0, 1, 3,  // 第一个三角形
            1, 2, 3   // 第二个三角形
    };

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);  // 生成顶点缓冲对象
    glGenVertexArrays(1, &VAO); // 生成顶点数组对象
    glGenBuffers(1, &EBO); // 生成元素缓冲对象
    // 首先绑定顶点数组对象，然后绑定和设置顶点缓冲区，然后配置顶点属性。
    glBindVertexArray(VAO);

    // 将VBO绑定到 GL_ARRAY_BUFFER 的缓冲类型上，可以同时绑定多个缓冲类型。
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 将之前定义的顶点数据复制到缓冲的内存中。
    /*
     * 该函数可以将用户定义的数据复制 到 当前绑定缓冲。
     * 第四个参数是显卡如何管理给定的数据。
     * GL_STATIC_DRAW ：数据不会或几乎不会改变。
     * GL_DYNAMIC_DRAW：数据会被改变很多。
     * GL_STREAM_DRAW ：数据每次绘制时都会改变。
     * */
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /*
     * 参数一：指定要配置的顶点属性
     * 对应于 ("layout(location = 0)"， 它将顶点属性的位置值设置为了0)，我们需要的就是这个顶点，所以这里传递0进去。
     * 参数二：顶点属性的大小， vec3 -> 3。
     * 参数三：数据类型。
     * 参数四：是否希望被初始化。
     * 参数五：步长，代表连续的顶点属性组之间的间隔。
     * 参数六：表示起止位置的偏移量。
     * */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    // 注意，这是允许的，对glVertexAttribPointer的调用注册了VBO作为顶点属性的绑定顶点缓冲区对象，这样之后我们就可以安全地解除绑定.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 你可以在之后解除VAO的绑定，这样其他VAO调用就不会意外地修改这个VAO，但这种情况很少发生。
    // 修改其他的vos需要调用glBindVertexArray，所以当没有直接必要的时候，我们通常不会解绑定vos(或VBOs)。
    glBindVertexArray(0);


    /*
     * 渲染循环
     *
     * */
    // while 循环判断 glfw 是否被要求退出。
    while (!glfwWindowShouldClose(window)) {
        // 在每一帧上面调用输入检测函数。
        processInput(window);

        // 渲染部分
        // ----------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 激活渲染程序对象
        glUseProgram(shaderProgram);
        // 因为我们只有一个VAO，所以没有必要每次都绑定它，但我们这样做是为了让事情更有条理。
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // GL_FILL 设置为填充的模式， GL_LINE ：设置为线框模式。
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        //------------------------------------

        // 检查是否有事件触发
        glfwSwapBuffers(window);
        // 使用双缓冲来避免图像闪烁问题，前缓冲保存最终输出图像，渲染指令在后缓冲上进行绘制。
        // 该函数就是交换前后缓冲。
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 释放/删除之前的分配的所有资源
    glfwTerminate();
    return 0;
}

// 当窗口大小发生改变的时候回调该函数。
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // 设置渲染尺寸的大小。
    // 前两个控制窗口左下角的位置， 然后就是窗口的宽度和高度。
    glViewport(0, 0, width, height);
}

// 询问相关按键在当前帧是否被按下，并且做出相关反应。
void processInput(GLFWwindow *window) {
    // 如果 esc 键按下了，就设置当前窗口允许被关闭。紧接着就会停止渲染循环。
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}