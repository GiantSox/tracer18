#pragma once
#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"

class GLWindow
{
public:
	GLFWwindow* window;
	Shader fsTextureShader;

	GLWindow();
	
	int drawFullscreenQuad(GLuint texture);

	~GLWindow();
private:
	int setupFullscreenQuad();
	GLuint fullscreenQuadVAO;
};

GLWindow::GLWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1280, 720, "Tracer18", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glViewport(0, 0, 1280, 720);

	setupFullscreenQuad();
}

int GLWindow::drawFullscreenQuad(GLuint texture)
{
	glUseProgram(fsTextureShader.shaderProgram);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(fullscreenQuadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	return 0;
}

GLWindow::~GLWindow()
{
}

int GLWindow::setupFullscreenQuad()
{
	fsTextureShader.setup("shaders/textureShader.vert", "shaders/textureShader.frag");

	float vertices[] = {
		// positions        // texture coords
		1.0,  1.0, 0.0,		1.0, 1.0, // top right
		1.0, -1.0, 0.0,		1.0, 0.0, // bottom right
		-1.0, -1.0, 0.0,	0.0, 0.0, // bottom let
		-1.0,  1.0, 0.0,    0.0, 1.0  // top let 
	};
	unsigned int indices[] = {
		0, 1, 3, // irst triangle
		1, 2, 3  // second triangle
	};

	GLuint VBO, EBO;
	glGenVertexArrays(1, &fullscreenQuadVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(fullscreenQuadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	//add vertices component
	glEnableVertexAttribArray(0);	//enable layout location zero

	//add texcoords component
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	return 0;
}


