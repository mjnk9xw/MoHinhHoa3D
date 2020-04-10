#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sphere.h"
#include <vector>
using namespace glm;

class Cloud {
public: 
	void sendUniformsToCloudShader(GLuint cameraLocation, GLuint cameraDirection, vec3 cameraPosition, vec3 cameraDirectionVector,
		GLuint MatrixID, mat4 MVP, GLuint morphingSpeedLocation, GLuint sphereCenterLocation,
		std::vector<Sphere> spheres, GLuint timeLocation, GLuint expansionDirectionLocation, vec3 expansionDirection,
		GLuint lightDirectionLocation, vec3 lightDirection,
		GLuint lightIntensityLocation, float lightIntensity, float passedTime);
	void sendUniformsToSkydomeShader(GLuint skydomecameraPositionLocation, GLuint skydomeCameraDirectionLocation, GLuint skydomeLightDirectionLocation, GLuint skydomelightIntensityLocation,
		GLuint SkydomeMatrixID, vec3 cameraPosition, vec3 cameraDirectionVector, vec3 lightDirection, float lightIntensity, mat4 MVP);
	void addSphere(vec3 expansionDirection, std::vector<vec3>& offsets, std::vector<GLfloat>& nonses,
		std::vector<GLfloat>& scales, std::vector<vec3>& thresholdNormals, std::vector<vec3>& frontPoints,
		std::vector<vec3>& backPoints);
	void addCloud(vec3 expansionDirection, std::vector<vec3>& offsets, std::vector<GLfloat>& nonses,
		std::vector<GLfloat>& scales, std::vector<vec3>& thresholdNormals, std::vector<vec3>& frontPoints,
		std::vector<vec3>& backPoints, int numberOfSpheres);
};