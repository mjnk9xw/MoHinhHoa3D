#pragma once
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
class ShaderLoad
{
public:
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};