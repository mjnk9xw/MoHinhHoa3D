#pragma once
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include "Sphere.h"
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std;


class ShaderLoad
{
public:
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
};