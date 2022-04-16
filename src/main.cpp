#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

//函数头
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main(){
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

	// glViewport(0,0,800,600);//OpenGL坐标范围只是[-1,1],实际上显示的是时候需要进行坐标映射到800*600的区域

	//Render loop
	while(!glfwWindowShouldClose(window)){
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//清空时的背景色
		glClear(GL_COLOR_BUFFER_BIT);
		processInput(window);
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
