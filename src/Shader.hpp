//
//  Shader.hpp
//  Echo First GLFW APP
//
//  Created by zhangjie on 2023/7/8.
//

#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>

class Shader {
  
public:
  GLuint Program;
  Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
  
  void Use();
  
  void setFloat(float offset);
//  void setShow(int show);
};

#endif /* Shader_hpp */
