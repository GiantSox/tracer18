#include <iostream>
#include <Windows.h>
#include "window.h"
#include "texture.h"

int main() {
	GLWindow window;
	Texture renderTex;
	unsigned char *data = new unsigned char[4 * 1280 * 720];
	for (int i = 0; i < 1280 * 720; i++) {
		data[4 * i + 0] = 128;
		data[4 * i + 1] = 255;
		data[4 * i + 2] = 0;
		data[4 * i + 3] = 255;
	}
	renderTex.loadRGBA(data, 1280, 720);

	while (!glfwWindowShouldClose(window.window))
	{
		window.drawFullscreenQuad(renderTex.texture);
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
	return 0;
}