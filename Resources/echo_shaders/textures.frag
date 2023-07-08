#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

void main()
{
//    color = texture(ourTexture, TexCoord);
  //我们还可以把得到的纹理颜色与顶点颜色混合，来获得更有趣的效果。我们只需把纹理颜色与顶点颜色在片段着色器中相乘来混合二者的颜色：
  color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
}
