#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "glad/glad.h"
#include "GLFW\glfw3.h"

const std::string vtxShaderSrc = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec4 vertexColor;

void main(){

	float vtColor = (gl_VertexID);
	vertexColor = vec4(vtColor/3,vtColor/4,vtColor/5,1);
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const std::string fragShaderSrc = R"(
#version 330 core
out vec4 FragColor;
in vec4 vertexColor;

void main()
{

    //FragColor = vec4(1,0.5,0,1);
	FragColor = vertexColor;
}
)";

class Shader
{
public:
	GLuint shaderProgram;
	std::string vertShaderPath;
	std::string fragShaderPath;
	void setup(std::string vtxShaderFilepath, std::string fragShaderFilepath);
	void reload();
	Shader();
	~Shader();

private:

};

void Shader::setup(std::string vtxShaderFilepath, std::string fragShaderFilepath)
{
	vertShaderPath = vtxShaderFilepath;
	fragShaderPath = fragShaderFilepath;
	std::ifstream vsFileStream(vtxShaderFilepath);
	if (!vsFileStream) {
		std::cout << "Failed to load vertex shader" << std::endl;
		return;
	}
	std::ifstream fsFileStream(fragShaderFilepath);
	if (!fsFileStream) {
		std::cout << "Failed to load fragment shader" << std::endl;
		return;
	}

	std::stringstream vsStrStream, fsStrStream;
	vsStrStream << vsFileStream.rdbuf();
	fsStrStream << fsFileStream.rdbuf();
	vsFileStream.close();
	fsFileStream.close();
	std::string vsStr = vsStrStream.str();
	std::string fsStr = fsStrStream.str();

	//create a vertex shader object and store its ID in vtxShaderID
	GLuint vtxShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char* vcstr = vsStr.c_str();
	//Load the shader source into the vertex shader object
	glShaderSource(vtxShaderID, 1, &vcstr, NULL);
	//Compile the source
	glCompileShader(vtxShaderID);

	//Error checking
	int vtxSuccess;
	glGetShaderiv(vtxShaderID, GL_COMPILE_STATUS, &vtxSuccess);
	if (!vtxSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(vtxShaderID, 512, NULL, infoLog);
		std::cout << "VERTEX SHADER COMPILATION FAILED" << std::endl;
		std::cout << infoLog << std::endl;
	}


	//Same stuff but for fragment shader
	GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fcstr = fsStr.c_str();
	glShaderSource(fragShaderID, 1, &fcstr, NULL);
	glCompileShader(fragShaderID);

	int fragSuccess;
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &fragSuccess);
	if (!fragSuccess) {
		char infoLog[512];
		glGetShaderInfoLog(fragShaderID, 512, NULL, infoLog);
		std::cout << "FRAGMENT SHADER COMPILATION FAILED" << std::endl;
		std::cout << infoLog << std::endl;
	}


	//Create a shader program object, which will be comprised of both the fragment and vertex shaders
	shaderProgram = glCreateProgram();
	//Attatch our compiled frag and vert shaders to the shader program
	glAttachShader(shaderProgram, vtxShaderID);
	glAttachShader(shaderProgram, fragShaderID);
	//Link everything together
	glLinkProgram(shaderProgram);

	//Error checking
	int shaderProgramSuccess;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramSuccess);
	if (!shaderProgramSuccess) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "SHADER PROGRAM FAILED TO LINK" << std::endl;
		std::cout << infoLog << std::endl;
	}


	//Cleanup unused objects -- we don't need the compiled shader objects once they're linked to a shader program
	glDeleteShader(vtxShaderID);
	glDeleteShader(fragShaderID);
}

void Shader::reload()
{
	setup(vertShaderPath, fragShaderPath);
}

Shader::Shader()
{
}

Shader::~Shader()
{
}