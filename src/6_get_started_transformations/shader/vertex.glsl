#version 330 core
layout(location = 0) in vec3 Position;//要对应layout，因为这个是封装Geometry类时指定好顶点属性指针的
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(Position, 1.0f); //矩阵顺序从右往左读
  outTexCoord = TexCoords;
}