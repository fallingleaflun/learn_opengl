#version 330 core
out vec4 FragColor;

in vec2 TexCoord; //从顶点着色器传来的输入变量(纹理)

//采样器(Sampler)，它以纹理类型作为后缀
uniform sampler2D texture1;//声明一个uniform
uniform sampler2D texture2;//声明一个uniform
//改变mix值
uniform float mixValue; 

void main()
{
    //texture函数来采样纹理的颜色
    //  ourTexture: 纹理采样器
    //  TexCoord: 对应的纹理坐标
    //mix函数是混合两种颜色值
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1-TexCoord.x, TexCoord.y)), mixValue);
}