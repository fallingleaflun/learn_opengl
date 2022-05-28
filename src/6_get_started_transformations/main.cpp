#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <iostream>
#include "learnopengl/shader_s.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <direct.h>
#include <stdio.h>
#include <string>
#include <geometry/PlaneGeometry.h>
#include <geometry/BoxGeometry.h>
#include <geometry/SphereGeometry.h>
#include <learnopengl/camera.h>
using namespace std;

//函数头
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xpos, double ypos);

// 窗口大小
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// processInput回调改变的全局变量
float mixValue = 0.2f; //改变shader的一个名为mixValue的uniform

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));				   //其余选择默认参数
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f; //鼠标位置
bool firstMouse = true;

// frame time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

//环境光的颜色以及光强因子
glm::vec3 ambientLightColor = glm::vec3(1.0f, 1.0f, 1.0f);//环境光的颜色
float ambientStrength = 0.1f;//环境光强因子

// 光源的位置和颜色
glm::vec3 lightPosition = glm::vec3(1.0, 1.5, 0.0); //光源位置
glm::vec3 lightColor = glm::vec3(0.0f, 1.0f, 0.0f);//光源颜色
float specularStrength;//镜面反射常量

int main()
{
	//------------------基本固定的代码----------------------------------
	//初始化glfw并预先对窗口进行配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//创建窗口对象
	GLFWwindow *window = glfwCreateWindow(800, 600, "Hello OpenGL", NULL, NULL);
	if (!window)
	{
		cout << "Fail creating window" << endl;
		return -1;
	}
	//需要为窗口创建上下文
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//设置Viewport大小，前两个参数是左下角，第3,4个是宽高
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //注册窗口大小改变时的回调
	// GLAD用于管理OpenGL函数指针，在调用任何OpenGL函数前需要初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Fail initialize GLAD" << endl;
		return -1;
	}
	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	//----------------------直接使用geometry来构造简单的基础几何体---------------
	BoxGeometry box_ary[10];
	for (int i = 0; i < 10; i++)
	{
		box_ary[i] = BoxGeometry(1.0f, 1.0f, 1.0f);
	}
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)};
	SphereGeometry lightSphere=SphereGeometry(0.1, 10.0, 10.0);//光源显示为球形

	//-----------------------创建封装好的着色器对象-------------------------
	std::string current_working_dir = "./src/6_get_started_transformations"; //记得改这个路径，真是麻烦得要死
	std::string ver_path = current_working_dir + "/shader/vertex.glsl";
	std::string frag_path = current_working_dir + "/shader/frag.glsl";
	std::string ver_path2 = current_working_dir + "/shader/light_object_vertex.glsl";
	std::string frag_path2 = current_working_dir + "/shader/light_object_frag.glsl";
	Shader ourShader(ver_path.c_str(), frag_path.c_str());
	Shader lightObjectShader(ver_path2.c_str(), frag_path2.c_str());

	//------------------------------------创建纹理---------------------------
	unsigned int texture1, texture2; //纹理的ID引用
	// texture 1
	glGenTextures(1, &texture1);			//产生1个纹理
	glBindTexture(GL_TEXTURE_2D, texture1); //绑定这个纹理
	// 为当前绑定的纹理对象设置环绕模式(texture wrap)
	// 纹理坐标的原点在左下角，横轴是s轴，纵轴是t轴
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 为当前绑定的纹理对象设置过滤方式(text_xxx_filter)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //被缩小时使用线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //被放大时使用线性过滤
	// 加载图像，创建纹理，并生成mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);																			//因为OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部, 调用这个函数翻转y轴
	unsigned char *data = stbi_load("E:/code/learn_opengl/res/img/container.jpg", &width, &height, &nrChannels, 0); //加载图像, 路径需要是stb_image.h为根的相对路径
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//使用图像数据生成纹理
		// GLTEXTURE_2D：指定Target，GLTEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
		// 0: mipmaps的的级别，0是默认
		// GL_RGB：纹理存储格式
		//纹理宽度和高度
		// 0：历史遗留问题
		// GL_UNSIGNED_BYTE: 源图的格式和数据类型，加载图像RGB值加载图像并存储为char(byte)数组
		// data: 图像数据
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		cout << stbi_failure_reason() << endl;
	}
	stbi_image_free(data); //释放空间！
	// texture 2
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	data = stbi_load("E:/code/learn_opengl/res/img/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		cout << stbi_failure_reason() << endl;
	}
	stbi_image_free(data);
	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	//矩阵先这样初始化
	glm::mat4 projection = glm::mat4(1.0f);																	//投影矩阵
	projection = glm::perspective(glm::radians(45.0f), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f); //透视投影矩阵
	ourShader.setMat4("projection", projection);															//基本不变，不放入循环

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //不显示光标并且不离开窗口
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//---------------------可以开始画图啦！---------------------------
	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		//清屏
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				//清空时的背景色
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除深度缓存位

		//在调用glDrawElements之前为纹理单元绑定纹理
		//纹理单元(Texture Unit): 一个纹理的位置值
		//纹理单元的主要目的是让我们在着色器中可以使用多于一个的纹理
		//一个纹理的默认纹理单元是0
		glActiveTexture(GL_TEXTURE0);			//纹理单元0被激活
		glBindTexture(GL_TEXTURE_2D, texture1); //绑定texture1到GL_TEXTURE0上
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		//混合纹理
		ourShader.setFloat("mixValue", mixValue);

		// 在该帧，摄像机的旋转平移
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.use();//记得在修改前use()一下，否则改的是另一个shader
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		ourShader.setVec3("lightColor", lightColor);
		ourShader.setVec3("lightPos", lightPosition);
		ourShader.setVec3("ambientLightColor", ambientLightColor);
		ourShader.setFloat("ambientStrength", ambientStrength);
		ourShader.setFloat("specularStrength", specularStrength);

		for (int i = 0; i < 10; i++)
		{
			// 在该帧模型自身旋转，没有用四元数，下面这段代码要放在draw之前才有用
			glm::mat4 model = glm::mat4(1.0f);//模型矩阵, make sure to initialize matrix to identity matrix first
			model = glm::translate(model, cubePositions[i]);
			//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
			ourShader.use();
			ourShader.setMat4("model", model);

			//绘制图形
			glBindVertexArray(box_ary[i].VAO); //使用封装好的Geometry对象
			// glDrawElements(GL_POINTS, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);
			// glDrawElements(GL_LINE_LOOP, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);
			glDrawElements(GL_TRIANGLES, box_ary[i].indices.size(), GL_UNSIGNED_INT, 0);
		}

		// 绘制灯光物体
		lightObjectShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		lightPosition = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()), lightPosition.y, lightPosition.z);//改变一下光源位置
		model = glm::translate(model, lightPosition);//灯光物体的模型平移至光源所在位置
		lightObjectShader.setMat4("model", model);
		lightObjectShader.setMat4("view", view);
		lightObjectShader.setMat4("projection", projection);
		lightObjectShader.setVec3("lightColor", lightColor);
		glBindVertexArray(lightSphere.VAO);
		glDrawElements(GL_TRIANGLES, lightSphere.indices.size(), GL_UNSIGNED_INT, 0);

		//输入交互
		processInput(window);
		// glfw的swap buffer以及poll IO events
		glfwSwapBuffers(window); //交换颜色buffer, 双缓冲：当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来
		glfwPollEvents();		 //检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。

		//衡量渲染速度以动态改变速度变化速率
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	//释放资源！
	for (int i = 0; i < 10; i++)
	{
		box_ary[i].dispose();
	}
	lightSphere.dispose();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{									 //让Viewport跟随窗口大小改变而改变的回调
									 //设置Viewport大小，前两个参数是左下角，第3,4个是宽高
	window = window;				 //老是报warning，烦死了
	glViewport(0, 0, width, height); // OpenGL坐标范围只是[-1,1],实际上显示的是时候需要进行坐标映射到width*height的区域
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.0001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.0001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	window = window; // warning煩死了
	if (firstMouse)	 // 这个bool变量初始时是设定为true的
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	//防止用力過猛
	if (-7 < xoffset && xoffset < 7)
	{
		xoffset = xoffset > 0 ? exp(xoffset) - 1.0 : 1 - exp(-xoffset);
	}
	if (-7 < yoffset && yoffset < 7)
	{
		yoffset = yoffset > 0 ? exp(yoffset) - 1.0 : 1 - exp(-yoffset);
	}
	float sensitivity = 0.005f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}