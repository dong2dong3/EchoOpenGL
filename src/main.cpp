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

//#include "SOIL.h"
#include <GLFW/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Vertices.h"
#include "ChernoShader.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void loadAndCreateTexture();
void draft_dir();
unsigned int createShaderProgram();
void listFilesRecursively(const std::filesystem::path& path);
//const char* getResourcePathWith(const std::string filename);
const char* getResourcePathWith(const std::string& filename);
const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename);

const char* getImageResourcePathWith(const std::string& filename);

const char* getShadersResourcePathWith(const std::string& filename);
// 自定义错误回调函数
static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
// Holds uniform value of texture mix
GLfloat mixValue = 0.2f;

// Shaders
const GLchar* vertexShaderSource =
//"#version 330 core\n"
//    "layout (location = 0) in vec3 position;\n" -- layout (location = 0)与glVertexAttribPointer(0, 3,...)对应
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

// Load and create a texture
//GLuint texture;
//Shader ourShader("./echo_shaders/textures.vs", "./echo_shaders/textures.frag");
//"./echo_shaders/echo_shader.vs",  "./echo_shaders/echo_shader.frag"

//GLuint VBO, VAO, EBO;

// The MAIN function, from here we start the application and run the game loop

std::filesystem::path executableParentPath = std::filesystem::current_path();

std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}
int main()
{
  std::cout << "Starting GLFW context, OpenGL 4.1" << std::endl;
//  trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
//  std::cout << trans << std::endl;
//
//  vec = trans * vec;
//  std::cout << vec.x << "xx" << vec.y  << "yy" << vec.z << "zz" << std::endl;
  //我们的箱子向左侧旋转，并是原来的一半大小
//  trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
//  trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
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
  //make window context before glewInit()
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
  
  // print opengl version
  std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
  
  // Define the viewport dimensions
  glViewport(0, 0, HEIGHT, WIDTH);
  
  /**
   在深度测试过程中，当一个像素要被绘制时，OpenGL会将该像素的深度值与深度缓冲区中对应位置的深度值进行比较。
   如果该像素的深度值比深度缓冲区中的值更接近观察者，那么它将被绘制；否则，它将被认为是被覆盖的像素而被丢弃。
   */
  // Setup OpenGL options
  glEnable(GL_DEPTH_TEST);

/**
 
 ERROR::SHADER::PROGRAM::LINKING_FAILED
 ERROR: Compiled vertex shader was corrupt.
 ERROR: Compiled fragment shader was corrupt.
 
 */
  
  // Build and compile our shader program
//  Shader ourShader((executableParentPath/ "Resources" / "textures.vs").c_str(), (executableParentPath/ "Resources" / "textures.frag").c_str());
  Shader ourShader(getShadersResourcePathWith("textures.vs"), getShadersResourcePathWith("textures.frag"));

  // Set up vertex data (and buffer(s)) and attribute pointers
//  GLfloat vertices[] = {
//      // Positions          // Colors           // Texture Coords
//       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f, 3.0f, // Top Right
//       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, 0.0f, // Bottom Right
//      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
//      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 3.0f  // Top Left
//  };
//  GLuint indices[] = {  // Note that we start from 0!
//      0, 1, 3, // First Triangle
//      1, 2, 3  // Second Triangle
//  };
  
  GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };
  
  glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };
  
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
//  // Color attribute
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//  glEnableVertexAttribArray(1);
  // TexCoord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // Unbind VAO

  
  // Load and create a texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // Set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Load image, create texture and generate mipmaps
  int width, height;
  unsigned char* image = SOIL_load_image(getImageResourcePathWith("container.jpg"), &width, &height, 0, SOIL_LOAD_RGB);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
  
  // ===========
  // Texture 2
  // ===========
  GLuint texture2;
  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  // Set our texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  
  // Set texture filtering
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  //Load, Create texture and generate mipmaps
  image = SOIL_load_image(getImageResourcePathWith("awesomeface.png"), &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  
//  draft_dir();
  unsigned int chernoProgram = createShaderProgram();
  // Game loop
  int seed = 0;
  while (!glfwWindowShouldClose(window))
  {
    // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
    glfwPollEvents();
    
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Draw our first triangle
    // 使用着色器程序
//    glUseProgram(shaderProgram);
    
    // Activate Shader
//    ourShader.Use();
    glUseProgram(chernoProgram);
    
    // Bind Texture
//    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(chernoProgram, "ourTexture1"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glUniform1i(glGetUniformLocation(chernoProgram, "ourTexture2"), 1);
    
    // Set current value of uniform mix
    glUniform1f(glGetUniformLocation(chernoProgram, "mixValue"), mixValue);
    
    //要让箱子随着时间推移旋转，我们必须在游戏循环中更新变换矩阵，因为它在每一次渲染迭代中都要更新。
//    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
//    glm::mat4 trans = glm::mat4(1.0f);
//    trans = glm::rotate(trans, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
//
//    GLuint transformLoc = glGetUniformLocation(ourShader.Program, "transform");
//    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    
    // Create transformations
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
//    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//    model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
    // Get their uniform location
    GLint modelLoc = glGetUniformLocation(chernoProgram, "model");
    GLint viewLoc = glGetUniformLocation(chernoProgram, "view");
    GLint projLoc = glGetUniformLocation(chernoProgram, "projection");
    // Pass them to the shaders
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Draw container
    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);

    glBindVertexArray(VAO);
    for(GLuint i = 1; i < 11; i++)
    {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
//      bool valid =  (seed + i) % 4 > 0 && (seed + i) % 4 < 4;
      GLfloat angle = 10.0f * i;
      if(i % 3 == 0)  angle = glfwGetTime() * angle;

//      seed = seed + 1;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    
    // Second transformation
    // ===================
//    glm::mat4 trans2 = glm::mat4(1.0f); // Reset it to an identity matrix
//    trans2 = glm::translate(trans2, glm::vec3(-0.5f, 0.5f, 0.0f));
//    GLfloat scaleAmount = sin(glfwGetTime());
//    trans2 = glm::scale(trans2, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
//    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
//
    // Now with the uniform matrix being replaced with new transformations, draw it again.
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
    
    
//    float offset = -0.5f;
//    ourShader.setFloat(offset);
//    glUniform1f(glGetUniformLocation(ourShader.Program, "someUniform"), 1.0f);
    
    
    // 更新uniform颜色
//    GLfloat timeValue = glfwGetTime();
//    GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
//    GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
//    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);


    // 绑定VAO并绘制顶点数据
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);

    // Swap the screen buffers
    glfwSwapBuffers(window);
  }
  
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
//  glDeleteBuffers(1, &EBO);
  
  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  // 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
  // 关闭应用程序
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  
  // change value of uniform with arrow keys(set amount of textre mix)
  if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
    mixValue += 0.1f;
    if (mixValue >= 1.0f) mixValue = 1.0f;
    
  }
  if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
  {
    mixValue -= 0.1f;
    if (mixValue <= 0.0f) mixValue = 0.0f;
  }
}

void programShaders() {
  
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
}

void draft_dir() {
  //  Shader ourShader("echo_shader.vs", "echo_shader.frag");
  //  Shader ourShader((resourcePath() + "echo_shader.vs").c_str(), (resourcePath() + "echo_shader.frag").c_str());
  
  // 获取当前工作目录
//  fs::path currentPath = fs::current_path();
  
  // 打印当前目录
  //"/Users/zhangjie/Library/Developer/Xcode/DerivedData/Echo_First_GLFW_APP-dnwdcxxpjabybdfweantaekjawge/Build/Products/Debug"
//  std::cout << "Current Directory: " << currentPath << std::endl;
//  listFilesRecursively(currentPath);
  
//  std::filesystem::path executablePath = std::filesystem::current_path() / "myExecutable";
//  std::cout << "executablePath file exists: " << executablePath << std::endl;

//  std::filesystem::path resourcePath = executablePath.parent_path() / "Resources" / "textures.frag";
//  std::filesystem::path resourcePath1 = executableParentPath / "Resources" / "textures.frag";
//
//  std::filesystem::path dirPath = std::filesystem::current_path();
//  const char* file = concatenatePath(dirPath, "Resources/textures.frag");  //getResourcePathWith("Resources/textures.frag");
  // zjzjzj
  // /Users/zhangjie/Resources/textures.frag
  const char* file1 = getShadersResourcePathWith("textures.vs");
  const char* file2 = getShadersResourcePathWith("textures.frag");
  std::cout <<"\nzjzjzj"<< file1 << "\nzjzjzj\n" << file2 << std::endl;
//  if (std::filesystem::exists(resourcePath1)) {
//      std::cout << "Resource file exists: " << resourcePath << std::endl;
//  } else {
//      std::cout << "Resource file does not exist: " << resourcePath << std::endl;
//  }

//  for (const auto& entry : fs::directory_iterator(currentPath)) {
//    std::cout << entry.path().filename() << std::endl;
//    listFilesRecursively(currentPath);
//  }
  
  
  //  Shader ourShader("/Users/zhangjie/Documents/SFML learning/Echo First GLFW APP(static)/echo_shaders/echo_shader.vs", "/Users/zhangjie/Documents/SFML learning/Echo First GLFW APP(static)/echo_shaders/echo_shader.frag"); //✅
  
  //  Shader ourShader("./echo_shaders/echo_shader.vs",  "./echo_shaders/echo_shader.frag");
}

void modifyVAO_VBO_VEO() {
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
}


void loadAndCreateTexture() {
  // Build and compile our shader program
  Shader ourShader("./echo_shaders/textures.vs", "./echo_shaders/textures.frag");

  // Set up vertex data (and buffer(s)) and attribute pointers
  GLfloat vertices[] = {
      // Positions          // Colors           // Texture Coords
       0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f, 3.0f, // Top Right
       0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, 0.0f, // Bottom Right
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
      -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 3.0f  // Top Left
  };
  GLuint indices[] = {  // Note that we start from 0!
      0, 1, 3, // First Triangle
      1, 2, 3  // Second Triangle
  };
  
  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  // TexCoord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // Unbind VAO

  
  // Load and create a texture
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
  // Set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
  // Load image, create texture and generate mipmaps
  int width, height;
  unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
}

void listFilesRecursively(const std::filesystem::path& path)
{
    if (std::filesystem::is_directory(path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (std::filesystem::is_directory(entry))
            {
                listFilesRecursively(entry.path());
            }
            else
            {
                std::cout << entry.path() << std::endl;
            }
        }
    }
    else
    {
        std::cout << path << std::endl;
    }
}

//const char* getResourcePathWith(const std::string &filename) {
//  std::filesystem::path executableParentPath = std::filesystem::current_path();
//  std::filesystem::path filePath = executableParentPath;
//  filePath.append(filename);
//
////  const char* result = filePath.string().c_str();
//  //Returning address of local temporary object
//  return filePath.string().c_str();
//}

const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename)
{
  std::filesystem::path filePath = directoryPath;
  filePath.append(filename);
  std::string filePathString = filePath.string();
  char* result = new char[filePathString.length() + 1];
  std::strcpy(result, filePathString.c_str());
  return result;
}

const char* getResourcePathWith(const std::string& filename)
{
  std::filesystem::path filePath = std::filesystem::current_path();
  filePath.append("Resources/" + filename);
  std::string filePathString = filePath.string();
  char* result = new char[filePathString.length() + 1];
  std::strcpy(result, filePathString.c_str());
  return result;
}

const char* getImageResourcePathWith(const std::string& filename)
{
  std::filesystem::path filePath = std::filesystem::current_path();
  filePath.append("Resources/" + filename);
  std::string filePathString = filePath.string();
  char* result = new char[filePathString.length() + 1];
  std::strcpy(result, filePathString.c_str());
  return result;
}

const char* getShadersResourcePathWith(const std::string& filename)
{
  std::filesystem::path filePath = std::filesystem::current_path();
  filePath.append("Resources/echo_shaders/" + filename);
  std::string filePathString = filePath.string();
  char* result = new char[filePathString.length() + 1];
  std::strcpy(result, filePathString.c_str());
  return result;
}

unsigned int createShaderProgram() {
  
  ShaderProgramSource source = ParseShader(getShadersResourcePathWith("basic_cherno.shader"));
  
//  std::cout << "VERTEX" <<source.VertexSource<< "FRAGMENT" << source.FragmentSource << std::endl;
  unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
  return program;
}
