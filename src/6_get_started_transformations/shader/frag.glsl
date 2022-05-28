#version 330 core
out vec4 FragColor;
in vec2 outTexCoord; //从顶点着色器传来的输入变量(纹理)
//采样器(Sampler)，它以纹理类型作为后缀
uniform sampler2D texture1;//声明一个uniform
uniform sampler2D texture2;//声明一个uniform
uniform float mixValue;//纹理颜色混合
uniform vec3 lightColor;//光照颜色

void main()
{
    //texture函数来采样纹理的颜色
    //  ourTexture: 纹理采样器
    //  TexCoord: 对应的纹理坐标
    //mix函数是混合两种颜色值
    FragColor = mix(texture(texture1, outTexCoord), texture(texture2, vec2(1-outTexCoord.x, outTexCoord.y)), mixValue);
    FragColor = vec4(FragColor.xyz * lightColor, 1.0);//乘上光照颜色，应该可以这样吧
}