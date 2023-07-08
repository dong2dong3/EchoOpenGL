//
//  main.cpp
//  Echo First GLFW APP
//
//  Created by zhangjie on 2023/7/3.
//

#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Shader.hpp"
#include "ResourcePath.hpp"
#include <filesystem>
namespace fs = std::filesystem;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// 自定义错误回调函数
static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource =
//"#version 330 core\n"
//    "layout (location = 0) in vec3 position;\n"
//    "void main()\n"
//    "{\n"
//    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
//    "}\0";

//"#version 330 core\n"
//"layout (location = 0) in vec3 position;\n" // position变量的属性位置值为0
//"out vec4 vertexColor;\n" // 为片段着色器指定一个颜色输
//"void main()\n"
//"{\n"
//    "gl_Position = vec4(position, 1.0);\n" // 注意我们如何把一个vec3作为vec4的构造器的参数
//    "vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);\n" // 把输出变量设置为暗红色
//"}\0";

//R"(
//#version 330 core
//layout (location = 0) in vec3 position; // position变量的属性位置值为0
//
//out vec4 vertexColor; // 为片段着色器指定一个颜色输出
//
//void main()
//{
//    gl_Position = vec4(position, 1.0); // 注意我们如何把一个vec3作为vec4的构造器的参数
//    vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f); // 把输出变量设置为暗红色
//}
//)";

R"(
#version 330 core
layout (location = 0) in vec3 position; // 位置变量的属性位置值为 0
layout (location = 1) in vec3 color;    // 颜色变量的属性位置值为 1

out vec3 ourColor; // 向片段着色器输出一个颜色

void main()
{
    gl_Position = vec4(position, 1.0);
    ourColor = color; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
}

)";
const GLchar* fragmentShaderSource =
//"#version 330 core\n"
//    "out vec4 color;\n"
//    "void main()\n"
//    "{\n"
//    "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//    "}\n\0";

//R"(
//#version 330 core
//in vec4 vertexColor; // 从顶点着色器传来的输入变量（名称相同、类型相同）
//
//out vec4 color; // 片段着色器输出的变量名可以任意命名，类型必须是vec4
//
//void main()
//{
//    color = vertexColor;
//}
//)";


//R"(
//#version 330 core
//out vec4 color;
//
//uniform vec4 ourColor; // 在OpenGL程序代码中设定这个变量
//
//void main()
//{
//    color = ourColor;
//}
//)";

R"(
#version 330 core
in vec3 ourColor;
out vec4 color;

void main()
{
    color = vec4(ourColor, 1.0f);
}
)";

// Set up vertex data (and buffer(s)) and attribute pointers
//GLfloat vertices[] = {
//    -0.5f, -0.5f, 0.0f, // Left
//     0.5f, -0.5f, 0.0f, // Right
//     0.0f,  0.5f, 0.0f  // Top
//};

GLfloat vertices[] = {
    // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

GLfloat vertices2[] = {
     0.5f,  0.5f, 0.0f,  // Top Right
     0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f   // Top Left
};
GLuint indices[] = {  // Note that we start from 0!
    0, 1, 3,  // First Triangle
    1, 2, 3   // Second Triangle
};

// The MAIN function, from here we start the application and run the game loop
int main()
{
  std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
  
  // Init GLFW
  glfwInit();
  
  // 设置错误回调函数
  glfwSetErrorCallback(errorCallback);
  
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 正向兼容模式
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  
  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
  if (!window)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  
  glfwMakeContextCurrent(window);
  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);
  
  // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  if (glewInit() != GLEW_OK)
  {
    std::cout << "Failed to initialize GLEW" << std::endl;
    return -1;
  }
  
  // Define the viewport dimensions
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  
  // 创建顶点着色器对象和片段着色器对象
  // Build and compile our shader program
  // Vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // Check for compile time errors
  GLint success;
  GLchar infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // Fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // Check for compile time errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
  
  // 创建着色器程序对象并链接着色器
  // Link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  
  // Check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  // 删除着色器对象，因为它们已经链接到着色器程序中
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  // 创建VAO和VBO
  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  // 绑定VAO
  // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
  glBindVertexArray(VAO);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  // 绑定VBO并存储顶点数据
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  GLuint EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


  // 配置顶点属性指针
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//  glEnableVertexAttribArray(0);
//
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//  glEnableVertexAttribArray(1);
  
  // 位置属性
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // 颜色属性
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // 解绑VBO和VAO
  // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
  glBindVertexArray(0);

//  Shader ourShader("echo_shader.vs", "echo_shader.frag");
//  Shader ourShader((resourcePath() + "echo_shader.vs").c_str(), (resourcePath() + "echo_shader.frag").c_str());
  
  // 获取当前工作目录
    fs::path currentPath = fs::current_path();

    // 打印当前目录
  //"/Users/zhangjie/Library/Developer/Xcode/DerivedData/Echo_First_GLFW_APP-dnwdcxxpjabybdfweantaekjawge/Build/Products/Debug"
    std::cout << "Current Directory: " << currentPath << std::endl;
  
  for (const auto& entry : fs::directory_iterator(currentPath)) {
      std::cout << entry.path().filename() << std::endl;
  }

  
//  Shader ourShader("/Users/zhangjie/Documents/SFML learning/Echo First GLFW APP(static)/echo_shaders/echo_shader.vs", "/Users/zhangjie/Documents/SFML learning/Echo First GLFW APP(static)/echo_shaders/echo_shader.frag"); //✅
  
    Shader ourShader("./echo_shaders/echo_shader.vs",  "./echo_shaders/echo_shader.frag");


/**
 
 ERROR::SHADER::PROGRAM::LINKING_FAILED
 ERROR: Compiled vertex shader was corrupt.
 ERROR: Compiled fragment shader was corrupt.
 
 */

  // Game loop
  while (!glfwWindowShouldClose(window))
  {
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();
    
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw our first triangle
    // 使用着色器程序
//    glUseProgram(shaderProgram);
    
    
    ourShader.Use();
    glUniform1f(glGetUniformLocation(ourShader.Program, "someUniform"), 1.0f);
    //        DrawStuff();
    
    
    
    // 更新uniform颜色
    GLfloat timeValue = glfwGetTime();
    GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


    // 绑定VAO并绘制顶点数据
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }
  
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  
  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
    // 关闭应用程序
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
} 
