#version 330 core
out vec4 FragColor;
in vec2 outTexCoord; //从顶点着色器传来的输入变量(纹理)
in vec3 outNormal; //从顶点着色器传来的输入变量(法向量)
in vec3 outFragPos; //从顶点着色器传来的输入变量(片元在世界空间中的位置)

//采样器(Sampler)，它以纹理类型作为后缀
uniform sampler2D texture1;//声明一个uniform
uniform sampler2D texture2;//声明一个uniform
uniform float mixValue;//纹理颜色混合
uniform vec3 lightColor;//光照颜色
uniform vec3 ambientLightColor;//环境光颜色
uniform float ambientStrength;//环境光强度常量
uniform vec3 lightPos;//光源的位置
uniform vec3 viewPos;//摄像机位置
uniform float specularStrength;//镜面反射常量

void main()
{
    //片元本身的颜色
    vec4 objectColor = mix(texture(texture1, outTexCoord), texture(texture2, vec2(1-outTexCoord.x, outTexCoord.y)), mixValue);
    
    //Phong光照计算
    //计算环境光
    vec3 ambient = ambientStrength * ambientLightColor;
    //计算漫反射光, 法向量和光照方向的夹角的cos作为漫反射因子, 注意需要单位化，否则算出来不是夹角的cos！
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lightPos - outFragPos);
    float diff = max(dot(norm, lightDir), 0.0);//max是因为夹角大于90度会变成负数，此时取0
    vec3 diffuse = diff * lightColor;
    //计算镜面反射光, 出射光线方向和视线方向的夹角作为镜面反射因子
    vec3 reflectDir = reflect(-lightDir, norm);//调用reflect函数求出射方向向量，参数1是入射方向，参数2是法向量，都要求是单位向量
    vec3 viewDir = normalize(viewPos-outFragPos);
    float spec = pow(max(dot(reflectDir, viewDir), 0), 256);//这个32是高光的反光度(Shininess)。
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor.xyz;
    FragColor = vec4(result, 1.0);
}