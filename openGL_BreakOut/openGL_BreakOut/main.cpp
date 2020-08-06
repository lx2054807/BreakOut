#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

#include <iostream>

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, int button, int action, int mode);

Game BreakOut(SCREEN_HEIGHT, SCREEN_WIDTH);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BreakOut", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		cout << "Failed to load GLAD" << endl;
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BreakOut.Init();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	BreakOut.State = GAME_MENU;

	while (!glfwWindowShouldClose(window)) 
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		BreakOut.ProcessInput(deltaTime);

		BreakOut.Update(deltaTime);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		BreakOut.Render();
		glfwSwapBuffers(window);
	}

	ResourceManager::Clear();
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_ENTER) 
	{
		if (action == GLFW_PRESS && BreakOut.State == GAME_MENU)
			BreakOut.State = GAME_ACTIVE;
		else if (action == GLFW_PRESS && BreakOut.State == GAME_ACTIVE)
			BreakOut.State = GAME_MENU;
	}

	if (key >= 0 && key < 1024) 
	{
		if (action == GLFW_PRESS) 
		{
			BreakOut.Keys[key] = true;
		}
		if (action == GLFW_RELEASE) 
		{
			BreakOut.Keys[key] = false;
		}
	}
}

void mouse_callback(GLFWwindow* window, int button, int action, int mode) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS){}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	glViewport(0, 0, width, height);
}