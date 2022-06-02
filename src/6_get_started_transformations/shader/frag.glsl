#version 330 core
out vec4 FragColor;
in vec2 outTexCoord; //从顶点着色器传来的输入变量(纹理)
in vec3 outNormal; //从顶点着色器传来的输入变量(法向量)
in vec3 outFragPos; //从顶点着色器传来的输入变量(片元在世界空间中的位置)

//采样器(Sampler)，它以纹理类型作为后缀
uniform sampler2D texture1;//声明一个uniform
uniform sampler2D texture2;//声明一个uniform
uniform float mixValue;//纹理颜色混合
uniform vec3 viewPos;//摄像机位置


//通过(光照贴图和材质属性)来决定片元的反射的光
//材质属性结构体
struct Material{
    sampler2D diffuse;//漫反射贴图
    sampler2D specular;//镜面反射贴图
    float shininess;//高光指数
};
uniform Material material;

//光照属性结构体，不是很懂为什么会把三个系数放在这里，系数是和光源有关的吗?
struct Light{
    vec3 lightColor;//光照颜色
    vec3 lightPos;//光源的位置
    vec3 ambient;//环境光系数
    vec3 diffuse;//漫反射系数
    vec3 specular;//镜面反射系数
};
uniform Light light;

void main()
{
    //片元本身的颜色
    // vec4 objectColor = mix(texture(texture1, outTexCoord), texture(texture2, vec2(1-outTexCoord.x, outTexCoord.y)), mixValue);
    vec4 objectColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //光照贴图
    vec3 diffuseTexture = vec3(texture(material.diffuse, outTexCoord));
    vec3 specularTexture = vec3(texture(material.specular, outTexCoord));
    
    //Phong光照计算
    //计算环境光
    vec3 ambient = light.ambient * diffuseTexture;//这里仅使用光的环境光系数乘上光照贴图, 忽略了环境光颜色，因为认为环境光颜色在几乎所有情况下都等于漫反射颜色
    //计算漫反射光, 法向量和光照方向的夹角的cos作为漫反射因子, 注意需要单位化，否则算出来不是夹角的cos！
    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(light.lightPos - outFragPos);
    float diff = max(dot(norm, lightDir), 0.0);//max是因为夹角大于90度会变成负数，此时取0
    vec3 diffuse = light.diffuse * diff * diffuseTexture;
    //计算镜面反射光, 出射光线方向和视线方向的夹角作为镜面反射因子
    vec3 reflectDir = reflect(-lightDir, norm);//调用reflect函数求出射方向向量，参数1是入射方向，参数2是法向量，都要求是单位向量
    vec3 viewDir = normalize(viewPos-outFragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * specularTexture;

    vec3 result = (ambient + diffuse + specular) * light.lightColor * objectColor.xyz;
    FragColor = vec4(result, 1.0);
}