#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/controls.hpp>
#include "Cloud.h"
#include "SharedLoad.h"
#include "Light.h"
#include <windows.h>

const int NUMBER_OF_SPHERES = 1000000;
int SPHERE_NUMBER_OF_VERTICES = 25;
//float MORPHING_SPEED_PER_SECOND = 0.1f;
float FLOATING_SPEED_PER_SECOND = 2.0f;
const int WINDOW_WIDTH = 1024;
const int WINDOW_HIGHT = 768;
const float SKYDOME_RADIUS = 500.0f; // skydome radius
const float CLOUD_PLANE_HIGHT = SKYDOME_RADIUS * 0.3f;
const float CLOUD_PLANE_RADIUS = sqrt(SKYDOME_RADIUS * SKYDOME_RADIUS - CLOUD_PLANE_HIGHT * CLOUD_PLANE_HIGHT);
const int SPHERES_PER_CLOUD = 30; // số cloud
const float SPAWN_CLOUD_LIKELYHOOD = 0.35f * SPHERES_PER_CLOUD / NUMBER_OF_SPHERES;
const float START_DAYTIME = 10.0f;
float MIN_CLOUD_CREATION_DISTANCE_SECONDS = 0.15f;
//const float NONSE_FACTOR = 100;
const float MOVING_SPEED = 100.0f; // tốc độ thay đổi
const float MOUSE_SPEED = 0.1f;

// khởi tạo các thuộc tính khác nhau
GLFWwindow* window;
vec3 lightDirection(1, -0.5f, -1);
float lightIntensity = 0;
float rotateAngle = 0;
vec3 cameraPosition(0, 0, 0);
glm::mat4 Projection;
std::vector<Sphere> spheres;
Sphere skydome(SKYDOME_RADIUS, 100, 100);
vec3 expansionDirection = vec3(1.0f, 0.0f, 1.0f);
std::vector<vec3> offsets;
std::vector<GLfloat> nonses;
std::vector<GLfloat> scales;
std::vector<vec3> thresholdNormals;
std::vector<vec3> frontPoints;
std::vector<vec3> backPoints;
float currentFrameRate = 0;
float daytime = START_DAYTIME;
float horizontalAngle = PI;
float verticalAngle = 0.0f;
vec3 cameraDirectionVector;
mat4 MVP;

// khởi tạo cho các biến sử dụng
GLuint sphereProgramID;
GLuint skydomeProgramID;
GLuint cameraLocation;
GLuint cameraDirection;
GLuint expansionDirectionLocation;
GLuint MatrixID;
GLuint SkydomeMatrixID;
GLuint sphereCenterLocation;
GLuint morphingSpeedLocation;
GLuint timeLocation;
GLuint lightDirectionLocation;
GLuint skydomelightIntensityLocation;
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint indexbuffer;
GLuint normalbuffer;
GLuint textureCoordsBuffer;
GLuint offsetsBuffer;
GLuint nonseBuffer;
GLuint scaleBuffer;
GLuint skydomeVertexbuffer;
GLuint skydomeIndexbuffer;
GLuint skyDomeNormalbuffer;
GLuint skydomeTextureCoordsBuffer;
GLuint skydomeLightDirectionLocation;
GLuint skydomeCameraDirectionLocation;
GLuint skydomecameraPositionLocation;
GLuint thresholdNormalsBuffer;
GLuint frontPointsBuffer;
GLuint backPointsBuffer;
GLuint lightIntensityLocation;
