#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include "learnopengl/shader_s.h"
using namespace std;

//函数头
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

	//-------------------三角形原始数据相关----------------------
	//三角形坐标. 3d坐标
	//顶点数组对象：Vertex Array Object，VAO
	float vertices[] = {
		//位置              //颜色
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
	};


	//---------------------内存/显存对象相关------------------------------------------
	//VBO：Vertex Buffer Object，VBO, VBO用来管理显存
	//VAO：Vertex Array Object，可以像VBO那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
	//VBO组成一个名为VAO的数组. 以数组的形式方便管理VBO
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);//产生1个VBO
	glGenVertexArrays(1, &VAO);//产生1个VAO
	glBindVertexArray(VAO);//先绑定VAO，之后再绑定VBO，这一步究竟是不是把VAO和GL_ARRAY_BUFFER绑在一起？
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO绑定到GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				//把用户数据复制到当前的数组缓冲，第四个参数有三种形式
				// GL_STATIC_DRAW ：数据不会或几乎不会改变。
				// GL_DYNAMIC_DRAW：数据会被改变很多。
				// GL_STREAM_DRAW ：数据每次绘制时都会改变。

	//链接顶点的位置属性(其实就是增加顶点的信息)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);//设置顶点属性指针所要访问的内存布局(理解成定义一种访存用的指针)
						//0:顶点属性位置值，与glsl的layout(location=0)对应
						//3：顶点属性大小因为有三个坐标
						//GLfloat：数据类型
						//GL_FALSE：是否标准化
						//stride(步长)
						//(void*)0：位置数据在缓存中起始位置的偏移量
	glEnableVertexAttribArray(0);//以顶点属性位置值作为参数，启用顶点属性
	//链接顶点的颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);


	//-----------------------创建一个封装好的着色器对象-------------------------
	Shader ourShader("../res/shader/FragmentShader.frag", "../res/shader/VertexShader.vert");


	//---------------------可以开始画图啦！---------------------------
	//Render loop
	while(!glfwWindowShouldClose(window)){
		//清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//清空时的背景色
		glClear(GL_COLOR_BUFFER_BIT);		

		//绘制三角形
		ourShader.use();//启用ourShader
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		//esc关闭窗口
		processInput(window);
		

		//glfw的swap buffer以及poll IO events
		glfwSwapBuffers(window);//交换颜色buffer, 双缓冲：当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来
		glfwPollEvents();//检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
	}
	
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
}