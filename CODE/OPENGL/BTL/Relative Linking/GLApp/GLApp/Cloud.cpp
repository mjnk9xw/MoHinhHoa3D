#include "Cloud.h"

using namespace glm;
const float NONSE_FACTOR = 100;
float MORPHING_SPEED_PER_SECOND = 0.1f;
const float SKYDOME_RADIUS = 500.0f; // skydome radius
const float CLOUD_PLANE_HIGHT = SKYDOME_RADIUS * 0.3f;
const float CLOUD_PLANE_RADIUS = sqrt(SKYDOME_RADIUS * SKYDOME_RADIUS - CLOUD_PLANE_HIGHT * CLOUD_PLANE_HIGHT);
const vec3 CLOUD_PLANE_CENTER = vec3(0, CLOUD_PLANE_HIGHT, 0);
float MAX_CLOUD_SCALE = 20.0f; // size tối đa của cloud 
float MIN_CLOUD_SCALE = 10.0f; // size tối thiểu của cloud

void Cloud::sendUniformsToCloudShader(GLuint cameraLocation, GLuint cameraDirection, vec3 cameraPosition, vec3 cameraDirectionVector,
	GLuint MatrixID, mat4 MVP, GLuint morphingSpeedLocation, GLuint sphereCenterLocation,
	std::vector<Sphere> spheres, GLuint timeLocation, GLuint expansionDirectionLocation, vec3 expansionDirection,
	GLuint lightDirectionLocation, vec3 lightDirection,
	GLuint lightIntensityLocation, float lightIntensity,float passedTime)
{
	std::cout << "sendUniformsToCloudShader begin" << std::endl;
	glUniform3f(cameraLocation, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform3f(cameraDirection, cameraDirectionVector.x, cameraDirectionVector.y, cameraDirectionVector.z); 
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]); 
	glUniform1f(morphingSpeedLocation, MORPHING_SPEED_PER_SECOND); 
	glUniform3f(sphereCenterLocation, spheres.at(0).center.x, spheres.at(0).center.y, spheres.at(0).center.z);
	glUniform1f(timeLocation, passedTime * MORPHING_SPEED_PER_SECOND);
	glUniform3f(expansionDirectionLocation, expansionDirection.x, expansionDirection.y, expansionDirection.z);
	glUniform3f(lightDirectionLocation, lightDirection.x, lightDirection.y, lightDirection.z); 
	glUniform1f(lightIntensityLocation, lightIntensity); 
	std::cout << "sendUniformsToCloudShader end" << std::endl;
}

void Cloud::sendUniformsToSkydomeShader(GLuint skydomecameraPositionLocation, GLuint skydomeCameraDirectionLocation, GLuint skydomeLightDirectionLocation, GLuint skydomelightIntensityLocation,
	GLuint SkydomeMatrixID, vec3 cameraPosition, vec3 cameraDirectionVector, vec3 lightDirection , float lightIntensity, mat4 MVP)
{
	std::cout << "sendUniformsToSkydomeShader begin" << std::endl;
	glUniform3f(skydomecameraPositionLocation, cameraPosition.x, cameraPosition.y, cameraPosition.z); 
	glUniform3f(skydomeCameraDirectionLocation, cameraDirectionVector.x, cameraDirectionVector.y, cameraDirectionVector.z); 
	glUniform3f(skydomeLightDirectionLocation, lightDirection.x, lightDirection.y, lightDirection.z); 
	glUniform1f(skydomelightIntensityLocation, lightIntensity); 
	glUniformMatrix4fv(SkydomeMatrixID, 1, GL_FALSE, &MVP[0][0]); 
	std::cout << "sendUniformsToSkydomeShader end" << std::endl;
}
void Cloud::addSphere(vec3 expansionDirection, std::vector<vec3>& offsets, std::vector<GLfloat>& nonses,
	std::vector<GLfloat>& scales,std::vector<vec3>& thresholdNormals,std::vector<vec3>& frontPoints,
	std::vector<vec3>& backPoints)
{
	std::cout << "addSphere begin" << std::endl;
	// add a sphere on the outline of the circular cloud plane
	float xRand2D = -0.5f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // [-0.5, 0.5]
	float yRand2D = -0.5f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // [-0.5, 0.5]
	// create the plane for the clouds
	vec2 planarExpansionDirection = vec2(expansionDirection.x, expansionDirection.z); // the y coordinate points upward and is thus only the hight
	planarExpansionDirection = normalize(planarExpansionDirection);
	// randomize the start position
	planarExpansionDirection.x += xRand2D;
	planarExpansionDirection.y += yRand2D;
	planarExpansionDirection = normalize(planarExpansionDirection);
	// scale the plane up to the size of the sky (+10% to make clouds float into the scene)
	planarExpansionDirection = -planarExpansionDirection * CLOUD_PLANE_RADIUS * 1.1f;

	float rX = planarExpansionDirection.x;
	float rY = CLOUD_PLANE_HIGHT;
	float rZ = planarExpansionDirection.y;
	offsets.insert(offsets.begin(), vec3(rX, rY, rZ));

	nonses.insert(nonses.begin(), NONSE_FACTOR * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // [0,NONSE_FACTOR]
	scales.insert(scales.begin(), MIN_CLOUD_SCALE + (MAX_CLOUD_SCALE - MIN_CLOUD_SCALE) * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)); // [min,max]
	thresholdNormals.insert(thresholdNormals.begin(), vec3(0)); // add standard value which will be updated
	backPoints.insert(backPoints.begin(), vec3(0)); // add standard value which will be updated
	frontPoints.insert(frontPoints.begin(), vec3(0)); // add standard value which will be updated

	std::cout << "addSphere end" << std::endl;
}

void Cloud::addCloud(vec3 expansionDirection, std::vector<vec3>& offsets, std::vector<GLfloat>& nonses,
	std::vector<GLfloat>& scales, std::vector<vec3>& thresholdNormals, std::vector<vec3>& frontPoints,
	std::vector<vec3>& backPoints,int numberOfSpheres)
{
	std::cout << "addCloud begin" << std::endl;
	vec3 offset(0);
	for (size_t i = 0; i < numberOfSpheres; i++)
	{
		addSphere(expansionDirection, offsets, nonses, scales, thresholdNormals, frontPoints, backPoints);
	}

	std::cout << "addCloud endl" << std::endl;
}