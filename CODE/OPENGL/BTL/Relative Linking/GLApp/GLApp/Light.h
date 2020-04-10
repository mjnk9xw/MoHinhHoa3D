#pragma once

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#define PI 3.14159265

class Light {
public:
	vec3 lightDirectionFromTime(float lightIntensity,float hours);
	float lightIntensityFromTime(float hours);
	void updateLight(float *lightIntensity, vec3 *lightDirection, float *daytime, float passedTime);
};