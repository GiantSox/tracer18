#pragma once
#include "glad/glad.h"
#include "glfw/glfw3.h"
#include <iostream>


class Texture
{
public:
	GLuint texture;

	void loadRGBA(unsigned char* imgData, int width, int height);
	Texture();
	~Texture();

private:

};

void Texture::loadRGBA(unsigned char* imgData, int width, int height)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//if we try to access a texcoord that isn't on the image, repeat/tile the image
	//S is x axis, T is y axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Specify if we want to use bilinear or nearest center filtering for upscaling and downscaling textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (imgData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "failed to load texture from memory" << std::endl;
	}

}

Texture::Texture()
{
}

Texture::~Texture()
{
}