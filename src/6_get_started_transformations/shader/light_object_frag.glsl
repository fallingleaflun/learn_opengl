#version 330 core
//光源本身的片元着色器
//当我们想要绘制我们的物体时，需要使用其他着色器
//当我们想要绘制灯的时候，我们会使用这个着色器
out vec4 FragColor;
in vec2 outTexCoord;
uniform vec3 lightColor;

void main() {
  FragColor = vec4(lightColor, 1.0f);
}