// Test06.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// #pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#ifndef GLAD_IMPLEMENTATION
#define GLAD_IMPLEMENTATION
#include <glad/glad.h>
#endif // GLAD_IMPLEMENTATION

// GLFW_INCLUDE_NONE information
// https://www.glfw.org/docs/latest/build_guide.html
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif // GLFW_INCLUDE_NONE

// 向量数学计算
#include <glm/glm.hpp>
// 暂未使用
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <windows.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>

// window size
const int max_window_width = 600;
const int max_window_height = 600;
const int standard_frame_rate = 300;
const GLfloat PI = 3.141592653;// 5897932384626433832795f;
// -------------------------------------------------------------------------------------------------------

// GLFW event callback
// GLFW error
void SetErrorCallback(int error, const char* description);
// resize window
void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height);
// resize window
void SetFramebufferSize(float max_window_width, float max_window_height);
// key in window
void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
// mouse in window
void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// -------------------------------------------------------------------------------------------------------

// 着色器检查器
void CheckShaderCompileError(unsigned int shader);
void CheckProgramLinkError(unsigned int program);
// -------------------------------------------------------------------------------------------------------

int main()
{
  /* Initialize the library */
  if (glfwInit() == GLFW_FALSE)
  {
    return -1;
  }

  // OpenGL 错误日志回调
  glfwSetErrorCallback(SetErrorCallback);

  // Create a windowed mode window and its OpenGL context
  GLFWwindow* glWindow = glfwCreateWindow(max_window_width, max_window_height, "OpenGL Demo 0.1", NULL, NULL);
  if (glWindow == NULL)
  {
    glfwTerminate();
    return -1;
  }

  // 设置窗体尺寸改变回调
  glfwSetFramebufferSizeCallback(glWindow, SetFramebufferSizeCallback);
  // 设置按钮事件回调
  glfwSetKeyCallback(glWindow, SetKeyCallback);
  // 鼠标按钮事件
  glfwSetMouseButtonCallback(glWindow, SetMouseButtonCallback);
  // Make the window's context current
  glfwMakeContextCurrent(glWindow);

  // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  
  if (!gladLoadGL()) // 用于简化 gladLoadGLLoader 的调用
  {
    return -1;
  }

  // 重设窗口尺寸
  SetFramebufferSize(max_window_width, max_window_height);
  // ---------------------------------------------------------------------------

  // 顶点着色器 (Vertex Shader)
  const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\0";
  // 片段着色器(Fragment Shader) :
  const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.75f, 0.75f, 0.75f, 1.0f);\n" // 灰色
    "}\n\0";

  // 编译着色器
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
#ifdef _DEBUG
  CheckShaderCompileError(vertexShader);
#endif

  // 编译着色器
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
#ifdef _DEBUG
  CheckShaderCompileError(fragmentShader);
#endif

  // 链接着色器程序
  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
#ifdef _DEBUG
  CheckProgramLinkError(shaderProgram);
#endif

  // 删除着色器对象
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // 创建 VAO 和 VBO
  GLuint VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // 为 VBO 分配足够的空间，容纳大圆和小圆的点数据
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 300000, NULL, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // 存储圆上点的坐标
  std::vector<glm::vec2> bigCirclePoints;
  std::vector<glm::vec2> smallCirclePoints;

  // 大圆角度/半径
  float bigCircleAngle = 0.0f;
  float bigCircleRadius = 0.6f;
  // 小圆角度/半径
  float smallCircleAngle = 0.0f;
  float smallCircleRadius = 0.25f;

  // 大圆小圆的坐标
  float bigCircleX = 0;
  float bigCircleY = 0;
  float smallCircleX = 0;
  float smallCircleY = 0;
  // ---------------------------------------------------------------------------

  // 计时器方式
  double timePrevious = 0;
  double timeLast = 0;
  // 跳帧时间
  double frameTimeLimit = 0.01;
  double timeFramePrevious = -1;
  double timeFrameLast = 0;
  // 帧数
  int fps = 0;

  /* 对控制台进行定行输出 */
  // 获取控制台句柄
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  // 获取控制台屏幕缓冲区信息
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  // 设置要输出的行的坐标
  COORD coord;
  coord.X = 1; // 列坐标 (从0开始)
  coord.Y = 1; // 行坐标 (从0开始)
  // SetConsoleCursorPosition(hConsole, coord);
  // printf("当前帧率：%d\n", fps);
  // std::cout << "输出字符串: " << fps << "  " << std::flush; // 注意：使用flush立即输出
  // ---------------------------------------------------------------------------

  // 检索自系统启动以来已用过的毫秒数
  // float fltTick = GetTickCount64() * 0.001;

  // 调控帧率的浮动系数
  float fltFactor = 0.1;

  // OpenGL 主循环
  while (!glfwWindowShouldClose(glWindow))
  {
    // 获取当前计时器
    timeFramePrevious = glfwGetTime();
    // 每 1 秒检查一次帧率
    if ((timeFramePrevious - timeFrameLast) >= 1.0f)
    {
      // 定行输出
      SetConsoleCursorPosition(hConsole, COORD{ 1,3 });
      std::cout << "当前帧率: " << fps << "   " << std::flush;
      SetConsoleCursorPosition(hConsole, COORD{ 1,4 });
      std::cout << "跳帧时间: " << frameTimeLimit << "   " << std::flush;

      timeFrameLast = timeFramePrevious;

      // 根据当前帧率调整跳帧时间，让帧率趋近 standard_frame_rate
      // 如果帧率 > standard_frame_rate
      // 则增加跳帧时间
      // 增加方式：A = A + (A * N)
      // 如果帧率 < standard_frame_rate
      // 则减少跳帧时间
      // 减少方式: A = A - (A * N)
      if (fps > standard_frame_rate)
      {
        if (fps > (standard_frame_rate * 2)) // 实际帧率大于 std 两倍
        {
          fltFactor = 1;
        }
        else if (fps > (standard_frame_rate * 1.5))
        {
          fltFactor = 0.5;
        }
        else if (fps > (standard_frame_rate * 1.1))
        {
          fltFactor = 0.1;
        }
        else
        {
          fltFactor = 0.01;
        }
        frameTimeLimit = frameTimeLimit + (frameTimeLimit * fltFactor);
      }
      // 低于标准帧率
      else if (fps < standard_frame_rate)
      {
        if (fps < (standard_frame_rate / 1.1))
        {
          fltFactor = 0.1;
        }
        else if (fps < (standard_frame_rate / 1.5))
        {
          fltFactor = 0.5;
        }
        else if (fps < (standard_frame_rate / 2))
        {
          fltFactor = 1;
        }

        frameTimeLimit = frameTimeLimit - (frameTimeLimit * fltFactor);
      }
      fps = 0;

      if (frameTimeLimit < 0.000001)
      {
        frameTimeLimit = 0.1;
      }
    }

    timePrevious = timeFramePrevious;
    // 低于帧率时间 跳帧
    if ((timePrevious - timeLast) < frameTimeLimit)
    {
      continue;
    }
    timeLast = timePrevious;
    fps += 1;

    // ---------------------------------------------------------------------------
    // 窗口底色
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 计算大圆新点的坐标
    bigCircleX = bigCircleRadius * cos(glm::radians(bigCircleAngle));
    bigCircleY = bigCircleRadius * sin(glm::radians(bigCircleAngle));
    // 将大圆新点添加到大圆的点向量中
    bigCirclePoints.push_back(glm::vec2(bigCircleX, bigCircleY));

    // 每帧小圆绘制 i 步 （大圆每帧绘制 1 步)
    for (int i = 0; i < 4; i++)  // 参数A
    {
      if (smallCircleAngle <= 360)
      {
        smallCircleAngle += 0.33;  // 参数B
      }
      else
      {
        smallCircleAngle = 0;
      }
      // 计算小圆新点的坐标 (以大圆新点为圆心)
      float smallCircleX = bigCircleX + smallCircleRadius * cos(glm::radians(smallCircleAngle));
      float smallCircleY = bigCircleY + smallCircleRadius * sin(glm::radians(smallCircleAngle));
      // 将小圆新点添加到小圆的点向量中
      smallCirclePoints.push_back(glm::vec2(smallCircleX, smallCircleY));
    }

    // 使用着色器程序
    glUseProgram(shaderProgram);

    // 更新 VBO 数据 (大圆)
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 更新 VBO 数据 (大圆)
    glBufferSubData(GL_ARRAY_BUFFER, 0, bigCirclePoints.size() * sizeof(glm::vec2), &bigCirclePoints[0]);

    // 绘制大圆
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, 0, bigCirclePoints.size());
    // -----------------------------------------------------------------------

    // 更新 VBO 数据 (小圆)
    glBufferSubData(GL_ARRAY_BUFFER, bigCirclePoints.size() * sizeof(glm::vec2), smallCirclePoints.size() * sizeof(glm::vec2), &smallCirclePoints[0]);
    // 绘制小圆 (设置正确的起始索引)
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, bigCirclePoints.size(), smallCirclePoints.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 更新角度
    bigCircleAngle += 0.5f;
    if (bigCircleAngle >= 360.0f)
    {
      bigCircleAngle = 0.0f;
      bigCirclePoints.clear(); //  如果需要重新绘制圆，可以清除点数据
      // smallCirclePoints.clear(); // 清除小圆的点数据
    }

    glfwSwapBuffers(glWindow);
    glfwPollEvents();
  }

  // 清理资源
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwDestroyWindow(glWindow);
  glfwTerminate();

  return 0;
}
// -------------------------------------------------------------------------------------------------------

// 检查着色器编译错误
void CheckShaderCompileError(unsigned int shader)
{
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }
}
// -------------------------------------------------------------------------------------------------------

// 检查着色器程序链接错误
void CheckProgramLinkError(unsigned int program)
{
  int success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
}
// -------------------------------------------------------------------------------------------------------

void SetFramebufferSize(float window_width, float window_height)
{
  /*
  // 像素、正交坐标系
  // 设置正交投影，用像素进编写
  // 设置视口
  glViewport(0, 0, window_width, window_height);
  // 设置正交 投影矩阵
  glMatrixMode(GL_PROJECTION);
  // 加载单位矩阵
  glLoadIdentity();
  // 创建正交投影矩阵, 在这个视景体内的物体不会随着距离的远近而改变大小，而是保持相同的比例
  // 左上角作为 0,0
  // glOrtho(0, window_width, window_height, 0, -1, 1);
  // 左下角作为 0,0
  // glOrtho(0, window_width, 0, window_height, -1, 1);
  // 中心点作为 0,0
  glOrtho(-window_width / 2.0f, window_width / 2.0f, -window_height / 2.0f, window_height / 2.0f, -1.0f, 1.0f);

  // 设置当前矩阵模式为 模型视图矩阵
  glMatrixMode(GL_MODELVIEW);
  */

  // 默认的投影坐标系
  // 设置视口（无需修改，保持与窗口大小一致）
  glViewport(0, 0, window_width, window_height);

  // 设置投影矩阵
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // 中心点为坐标原点
  // glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

  // 设置模型视图矩阵
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
// -------------------------------------------------------------------------------------------------------

// OpenGL 内部错误反馈
void SetErrorCallback(int error, const char* description)
{
  printf("%s", description);
}
// -------------------------------------------------------------------------------------------------------

// OpenGL 窗体尺寸发生改变
void SetFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  SetFramebufferSize(width, height);
}
// -------------------------------------------------------------------------------------------------------

// glfw 发生键盘事件后调用
void SetKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // 按下 ESC 键时关闭窗口
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
  {

  }

}
// -------------------------------------------------------------------------------------------------------

// 鼠标事件
void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}
// -------------------------------------------------------------------------------------------------------
