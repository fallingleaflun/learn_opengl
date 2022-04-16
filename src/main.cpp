#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
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

	//-------------------三角形原始数据相关----------------------
	//三角形坐标. 3d坐标
	//顶点数组对象：Vertex Array Object，VAO
	// 以下是用两个三角形组成一个矩形，需要六个顶点
	// float vertices[] = {
	// 	// 第一个三角形
	// 	0.5f, 0.5f, 0.0f,   // 右上角
	// 	0.5f, -0.5f, 0.0f,  // 右下角
	// 	-0.5f, 0.5f, 0.0f,  // 左上角
	// 	// 第二个三角形
	// 	0.5f, -0.5f, 0.0f,  // 右下角
	// 	-0.5f, -0.5f, 0.0f, // 左下角
	// 	-0.5f, 0.5f, 0.0f   // 左上角
	// };
	// 以下是使用索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object
	// 之后，只需要四个顶点，为此需要EBO来指定那些节点组成三角形
	float vertices[] = {
		0.5f, 0.5f, 0.0f,   // 右上角
		0.5f, -0.5f, 0.0f,  // 右下角
		-0.5f, -0.5f, 0.0f, // 左下角
		-0.5f, 0.5f, 0.0f   // 左上角
	};
	unsigned int indices[] = { // 注意索引从0开始! 
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	//VBO可以组成一个数组名为VAO. 数组的形式方便管理VBO
	unsigned int VAO;//顶点数组对象(Vertex Array Object, VAO)可以像VBO那样被绑定，任何随后的顶点属性调用都会储存在这个VAO中。
	glGenVertexArrays(1, &VAO);//产生一个VAO
	glBindVertexArray(VAO);//先绑定VAO，之后再绑定VBO，这一步究竟是不是把VAO和GL_ARRAY_BUFFER绑在一起？

	unsigned int VBO;//顶点缓冲对象：Vertex Buffer Object，VBO, VBO用来管理显存
	glGenBuffers(1, &VBO);//产生一个VBO，第一个参数是OpenGL对象的id
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO绑定到数组缓冲
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				//把用户数据复制到当前的数组缓冲，第四个参数有三种形式
				// GL_STATIC_DRAW ：数据不会或几乎不会改变。
				// GL_DYNAMIC_DRAW：数据会被改变很多。
				// GL_STREAM_DRAW ：数据每次绘制时都会改变。

	unsigned int EBO;//索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//这次是绑定到Element数组缓冲
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//链接顶点属性(其实就是增加顶点的信息)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);//设置顶点属性指针所要访问的内存布局(理解成定义一种访存用的指针)
						//0:顶点属性位置值，与glsl的layout(location=0)对应
						//3：顶点属性大小因为有三个坐标
						//GLfloat：数据类型
						//GL_FALSE：是否标准化
						//stride(步长)
						//(void*)0：位置数据在缓存中起始位置的偏移量
	glEnableVertexAttribArray(0);//以顶点属性位置值作为参数，启用顶点属性


	//-----------------------顶点着色器-------------------------
	//vertex shader的glsl代码
	const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
	//创建一个GL_VERTEX_SHADER
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//编译glsl代码
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//检测glsl编译错误的代码，抄就完事了
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	

	//------------------------片段着色器-------------------------
	//glsl代码
	const char* fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main(){\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n";

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//检测glsl编译错误的代码，抄就完事了
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	//------------------------链接两种着色器----------------------
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//检查链接错误
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//链接完可以删掉shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Render loop
	while(!glfwWindowShouldClose(window)){
		//清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//清空时的背景色
		glClear(GL_COLOR_BUFFER_BIT);

		//线框模式(Wireframe Mode)开启
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//绘制物体
		glUseProgram(shaderProgram);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(float), GL_UNSIGNED_INT, 0);
		//线框模式(Wireframe Mode)关闭
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//esc关闭窗口
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
