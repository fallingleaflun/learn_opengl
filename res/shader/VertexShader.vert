#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置为0
layout (location = 1) in vec3 aColor;// 颜色变量的属性位置为1

out vec3 vertexColor; // 为片段着色器指定一个颜色输出

void main()
{
    gl_Position = vec4(aPos, 1.0); // 注意如何把一个vec3作为vec4的构造器的参数
    vertexColor = aColor; // 把输出变量设置为aColor输入
}