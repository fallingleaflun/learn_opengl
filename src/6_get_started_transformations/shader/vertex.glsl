#version 330 core
layout(location = 0) in vec3 Position;//要对应layout，因为这个是封装Geometry类时指定好顶点属性指针的
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

out vec2 outTexCoord;
out vec3 outNormal;//输出给片元的顶点法向量
out vec3 outFragPos;//输出给片元的片元位于世界空间中的坐标

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(Position, 1.0f); //矩阵顺序从右往左读
  outTexCoord = TexCoords;
  outFragPos = vec3(model * vec4(Position, 1.0f));//计算位于世界空间中的坐标只需左乘model矩阵，注意要套一个vec3()在外面
  //移动和不等比缩放都会改变法向量方向
  outNormal = mat3(transpose(inverse(model))) * Normal;//在顶点着色器中计算法线矩阵，涉及到求逆，是耗时操作
}