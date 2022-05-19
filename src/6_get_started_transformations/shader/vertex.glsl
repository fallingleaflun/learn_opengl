#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为0
layout (location = 1) in vec2 aTextCoord;//纹理变量的属性位置为2

out vec2 TexCoord;//为片段着色器指定一个纹理输出

uniform mat4 trans;

void main()
{
    gl_Position = trans * vec4(aPos, 1.0f); // 左乘一个变换矩阵
    TexCoord = aTextCoord;
}