#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include "learnopengl/shader_s.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm/glm.hpp>
using namespace std;

//函数头
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//processInput回调改变的全局变量
float mixValue = 0.2f;//改变shader的一个名为mixValue的uniform

int main(){
	//------------------基本固定的代码----------------------------------
	//初始化glfw并预先对窗口进行配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建窗口对象
	GLFWwindow* window = glfwCreateWindow(800,600,"Hello OpenGL", NULL, NULL);
	if(!window){
		cout<<"Fail creating window"<<endl;
		return -1;
	}
	//需要为窗口创建上下文
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//设置Viewport大小，前两个参数是左下角，第3,4个是宽高
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//注册窗口大小改变时的回调
	//GLAD用于管理OpenGL函数指针，在调用任何OpenGL函数前需要初始化GLAD
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
		cout<<"Fail initialize GLAD"<<endl;
		return -1;
	}

	//-------------------查询glsl能支持(声明)多少个顶点属性,一般是16个----------------------
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	//-------------------原始数据相关----------------------
	float vertices[] = {//点
		//位置                //颜色               //纹理坐标
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {//索引
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };


	//---------------------内存/显存对象相关------------------------------------------
	//VBO：Vertex Buffer Object，VBO, VBO用来管理显存
	//VAO：Vertex Array Object，可以像VBO那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
	//EBO: Element Buffer Object, 指定构成元素(比如三角形）的索引
	//VBO组成一个名为VAO的数组. 以数组的形式方便管理VBO
	unsigned int VBO, VAO, EBO;
	glGenBuffers(1, &VBO);//产生1个VBO
	glGenVertexArrays(1, &VAO);//产生1个VAO.使用的是glGenVertexArrays
	glGenBuffers(1, &EBO);//产生1个EBO
	
	glBindVertexArray(VAO);//先绑定VAO，之后再绑定VBO，这一步究竟是不是把VAO和GL_ARRAY_BUFFER绑在一起？
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO绑定到GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				//把用户数据复制到当前的数组缓冲，第四个参数有三种形式
				// GL_STATIC_DRAW ：数据不会或几乎不会改变。
				// GL_DYNAMIC_DRAW：数据会被改变很多。
				// GL_STREAM_DRAW ：数据每次绘制时都会改变。
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//链接顶点的位置属性(其实就是增加顶点的信息)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);//设置顶点属性指针所要访问的内存布局(理解成定义一种访存用的指针)
						//0:顶点属性位置值，与glsl的layout(location=0)对应
						//3：顶点属性大小因为有三个坐标
						//GLfloat：数据类型
						//GL_FALSE：是否标准化
						//stride(步长)
						//(void*)0：位置数据在缓存中起始位置的偏移量
	glEnableVertexAttribArray(0);//以顶点属性位置值作为参数，启用顶点属性
	//链接顶点的颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	//链接顶点的纹理属性
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	
	//------------------------------------创建纹理---------------------------
    unsigned int texture1, texture2; //纹理的ID引用
    // texture 1
    glGenTextures(1, &texture1);//产生1个纹理
    glBindTexture(GL_TEXTURE_2D, texture1);//绑定这个纹理
    // 为当前绑定的纹理对象设置环绕模式(texture wrap)
	// 纹理坐标的原点在左下角，横轴是s轴，纵轴是t轴
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 为当前绑定的纹理对象设置过滤方式(text_xxx_filter)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//被缩小时使用线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//被放大时使用线性过滤
    // 加载图像，创建纹理，并生成mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); //因为OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部, 调用这个函数翻转y轴
	// unsigned char *data = stbi_load("./res/img/container.jpg", &width, &height, &nrChannels, 0);//加载图像
	unsigned char *data = stbi_load("E:/code/learn_opengl/res/img/container.jpg", &width, &height, &nrChannels, 0);//加载图像, 路径需要是stb_image.h为根的相对路径
    cout << nrChannels << endl;
	cout<<"debug1"<<endl;
	if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//使用图像数据生成纹理
			//GLTEXTURE_2D：指定Target，GLTEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
			//0: mipmaps的的级别，0是默认
			//GL_RGB：纹理存储格式
			//纹理宽度和高度
			//0：历史遗留问题
			//GL_UNSIGNED_BYTE: 源图的格式和数据类型，加载图像RGB值加载图像并存储为char(byte)数组
			//data: 图像数据
        glGenerateMipmap(GL_TEXTURE_2D);
    }
	else{
        std::cout << "Failed to load texture" << std::endl;
		cout << stbi_failure_reason() << endl;
    }
    stbi_image_free(data);//释放空间！
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("E:/code/learn_opengl/res/img/awesomeface.png", &width, &height, &nrChannels, 0);
	cout << nrChannels << endl;
	if (data){
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//怎么知道有没有A通道
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load texture" << std::endl;
		cout << stbi_failure_reason() << endl;
    }
    stbi_image_free(data);
	//-----------------------创建一个封装好的着色器对象-------------------------
	Shader ourShader("./res/shader/VertexShader.vert", "./res/shader/FragmentShader.frag");
	
	
	//-----------------------传递Uniform(别问了要先use这个shader)-------------------
	ourShader.use();
	glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);//可以这样设置
	ourShader.setInt("texture2", 1);//也可以利用封装好的Shader类的函数这样设置

	//---------------------可以开始画图啦！---------------------------
	//Render loop
	while(!glfwWindowShouldClose(window)){
		//清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//清空时的背景色
		glClear(GL_COLOR_BUFFER_BIT);

		//在调用glDrawElements之前为纹理单元绑定纹理
		//纹理单元(Texture Unit): 一个纹理的位置值
		//纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理
		//一个纹理的默认纹理单元是0
		glActiveTexture(GL_TEXTURE0);//纹理单元0被激活
		glBindTexture(GL_TEXTURE_2D, texture1);//绑定texture1到GL_TEXTURE0上
		glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

		//绘制长方形
		ourShader.setFloat("mixValue", mixValue);
		ourShader.use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		//esc关闭窗口
		processInput(window);
		

		//glfw的swap buffer以及poll IO events
		glfwSwapBuffers(window);//交换颜色buffer, 双缓冲：当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来
		glfwPollEvents();//检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
	}
	
	//释放资源！
	glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

	//释放资源！
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){//让Viewport跟随窗口大小改变而改变的回调
	//设置Viewport大小，前两个参数是左下角，第3,4个是宽高
    glViewport(0, 0, width, height);//OpenGL坐标范围只是[-1,1],实际上显示的是时候需要进行坐标映射到width*height的区域
}

void processInput(GLFWwindow* window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS){
		//看函数注释glfwGetKey: Returns the last reported state of a keyboard key for the specified window
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.0001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.0001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }
}