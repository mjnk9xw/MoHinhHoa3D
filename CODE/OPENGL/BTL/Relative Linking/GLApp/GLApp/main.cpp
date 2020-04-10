#include "main.h"

using namespace glm;

Light *light = new Light();
Cloud *cloud = new Cloud();

float calculateDistanceToCamera(int index)
{
	vec3 offset = offsets.at(index);
	return abs(length(cameraPosition - offset));
}

void updateCamera(float passedTime)
{

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, (double)WINDOW_WIDTH / 2, (double)WINDOW_HIGHT / 2);

	// Compute new orientation
	horizontalAngle += MOUSE_SPEED * passedTime * float(WINDOW_WIDTH / 2 - xpos);
	verticalAngle += MOUSE_SPEED * passedTime * float(WINDOW_HIGHT / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
		);

	// Up vector : perpendicular to both direction and right
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		cameraPosition += direction * passedTime * MOVING_SPEED;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		cameraPosition -= direction * passedTime * MOVING_SPEED;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		cameraPosition += right * passedTime * MOVING_SPEED;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		cameraPosition -= right * passedTime * MOVING_SPEED;
	}

	// calculate threshold for cloud rendering with new camera position
	// threshold equation: normal equation with normal towards camera and point on plain
	for (size_t i = 0; i < offsets.size(); i++)
	{
		thresholdNormals.at(i) = cameraPosition - offsets.at(i);
		thresholdNormals.at(i) = normalize(thresholdNormals.at(i));
		frontPoints.at(i) = 0.99f * thresholdNormals.at(i) * spheres.at(0).radius * scales.at(i) + offsets.at(i);
		backPoints.at(i) = 0.01f * thresholdNormals.at(i) * spheres.at(0).radius * scales.at(i) + offsets.at(i);
	}

	vec3 lookAtPosition = cameraPosition + direction;
	cameraDirectionVector = normalize(cameraPosition - lookAtPosition);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		cameraPosition,           // Camera is here
		lookAtPosition, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// change data structures to draw far clouds first
	// linear sorting assumption: camera updates happen more frequently then clouds can flip around
	// data structures: first element is most far away from camera
	float lastDistance = calculateDistanceToCamera(0);
	for (size_t i = 1; i < offsets.size(); i++)
	{
		float currentDistance = calculateDistanceToCamera(i);
		if (lastDistance < currentDistance)
		{
			// swap data
			vec3 offsetTmp = offsets.at(i);
			offsets.at(i) = offsets.at(i - 1);
			offsets.at(i - 1) = offsetTmp;
			float nonseTmp = nonses.at(i);
			nonses.at(i) = nonses.at(i - 1);
			nonses.at(i - 1) = nonseTmp;
			float scaleTmp = scales.at(i);
			scales.at(i) = scales.at(i - 1);
			scales.at(i - 1) = scaleTmp;
			vec3 frontPointTmp = frontPoints.at(i);
			frontPoints.at(i) = frontPoints.at(i - 1);
			frontPoints.at(i - 1) = frontPointTmp;
			vec3 backPointsTmp = backPoints.at(i);
			backPoints.at(i) = backPoints.at(i - 1);
			backPoints.at(i - 1) = backPointsTmp;
			vec3 thresholdNormalTmp = thresholdNormals.at(i);
			thresholdNormals.at(i) = thresholdNormals.at(i - 1);
			thresholdNormals.at(i - 1) = thresholdNormalTmp;
		}

		lastDistance = currentDistance;
	}
}

void sceneSetup()
{
	// add cloud template
	spheres.push_back(Sphere(1, SPHERE_NUMBER_OF_VERTICES, SPHERE_NUMBER_OF_VERTICES));

	// add first cloud
	cloud->addCloud(expansionDirection, offsets, nonses, scales, thresholdNormals,frontPoints,
		 backPoints,SPHERES_PER_CLOUD);
}

void openGlSetup()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


	// tạo và biên dịch glsl từ các shader
	ShaderLoad *ShaderLoadSphere = new ShaderLoad();
	sphereProgramID = ShaderLoadSphere->LoadShaders("Sphere.vertexshader", "Sphere.fragmentshader");
	GLERROR;
	ShaderLoad *ShaderLoadSkyDome = new ShaderLoad();
	skydomeProgramID = ShaderLoadSkyDome->LoadShaders("Skydome.vertexshader", "Skydome.fragmentshader");
	GLERROR;

	// register uniforms
	// -- for the clouds
	cameraLocation = glGetUniformLocation(sphereProgramID, "cameraPosition");
	cameraDirection = glGetUniformLocation(sphereProgramID, "cameraDirection");
	expansionDirectionLocation = glGetUniformLocation(sphereProgramID, "expansionDirection");
	MatrixID = glGetUniformLocation(sphereProgramID, "MVP");
	sphereCenterLocation = glGetUniformLocation(sphereProgramID, "sphereCenter");
	morphingSpeedLocation = glGetUniformLocation(sphereProgramID, "morphingSpeed");
	timeLocation = glGetUniformLocation(sphereProgramID, "time");
	lightDirectionLocation = glGetUniformLocation(sphereProgramID, "lightDirection");
	lightIntensityLocation = glGetUniformLocation(sphereProgramID, "lightIntensity");

	// -- for the skydome
	SkydomeMatrixID = glGetUniformLocation(skydomeProgramID, "skyMVP");
	skydomelightIntensityLocation = glGetUniformLocation(skydomeProgramID, "lightIntensity");
	skydomeLightDirectionLocation = glGetUniformLocation(skydomeProgramID, "skydomeLightDirection");
	skydomeCameraDirectionLocation = glGetUniformLocation(skydomeProgramID, "cameraDirection");
	skydomecameraPositionLocation = glGetUniformLocation(skydomeProgramID, "cameraPosition");
	GLERROR;


	// OpenGL init
	glDisable(GL_CULL_FACE); // culling
	glEnable(GL_DEPTH_TEST); // depth testing
	glEnable(GL_BLEND); // Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 10000 units
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);

}

bool initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HIGHT, "Procedural Clouds", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return true;
}

float lastCloudCreated = 0;
void updateScene(float passedSeconds, float totalPassedTime)
{
	// move the spheres
	for (size_t i = 0; i < offsets.size(); i++)
	{
		// translate the offset of all spheres
		// - floats at same speed no matter which FPS
		offsets.at(i) += expansionDirection * FLOATING_SPEED_PER_SECOND * passedSeconds;

		// remove if it is out of view (2D cloud offset > radius of cloud plane circle)
		float offsetLength = length(vec2(offsets.at(i).x, offsets.at(i).z)); // y is only hight coordinate
		bool invisible = offsetLength > CLOUD_PLANE_RADIUS * 1.2; // add 20% to avoid removing clouds that have just been spawned
		if (invisible)
		{
			offsets.erase(offsets.begin() + i);
			nonses.erase(nonses.begin() + i);
			scales.erase(scales.begin() + i);
			thresholdNormals.erase(thresholdNormals.begin() + i);
			frontPoints.erase(frontPoints.begin() + i);
			backPoints.erase(backPoints.begin() + i);
			i--;
		}
	}

	// add new spheres
	if (offsets.size() < NUMBER_OF_SPHERES - SPHERES_PER_CLOUD && currentFrameRate > 32)
	{

		int numberOfAdditionalClouds = (NUMBER_OF_SPHERES - offsets.size()) / SPHERES_PER_CLOUD;
		for (size_t i = 0; i < numberOfAdditionalClouds; i++)
		{
			float randDecision = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if ((totalPassedTime - lastCloudCreated > MIN_CLOUD_CREATION_DISTANCE_SECONDS && randDecision < SPAWN_CLOUD_LIKELYHOOD)
				|| offsets.size() == 0)
			{
				//addCloud(SPHERES_PER_CLOUD);
				cloud->addCloud(expansionDirection, offsets, nonses, scales, thresholdNormals, frontPoints,
					backPoints, SPHERES_PER_CLOUD);
				lastCloudCreated = totalPassedTime;
			}
		}
	}
}

void makeGenAndBindBuffer() {
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, spheres.at(0).vertices.size() * sizeof(vec3), &spheres.at(0).vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * spheres.at(0).indices.size(), &spheres.at(0).indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, spheres.at(0).normals.size() * sizeof(vec3), &spheres.at(0).normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &textureCoordsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, spheres.at(0).texcoords.size() * sizeof(vec2), &spheres.at(0).texcoords[0], GL_STATIC_DRAW);

	glGenBuffers(1, &offsetsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, offsetsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * offsets.size(), &offsets[0], GL_STATIC_DRAW);

	glGenBuffers(1, &nonseBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, nonseBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nonses.size(), &nonses[0], GL_STATIC_DRAW);

	glGenBuffers(1, &scaleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, scaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * scales.size(), &scales[0], GL_STATIC_DRAW);

	glGenBuffers(1, &thresholdNormalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, thresholdNormalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * thresholdNormals.size(), &thresholdNormals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &frontPointsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, frontPointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * frontPoints.size(), &frontPoints[0], GL_STATIC_DRAW);

	glGenBuffers(1, &backPointsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, backPointsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * backPoints.size(), &backPoints[0], GL_STATIC_DRAW);

	// ------------------- SKYDOME ----------------------
	glGenBuffers(1, &skydomeVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skydomeVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, skydome.vertices.size() * sizeof(vec3), &skydome.vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &skydomeIndexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skydomeIndexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * skydome.indices.size(), &skydome.indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &skyDomeNormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skyDomeNormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, skydome.normals.size() * sizeof(vec3), &skydome.normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &skydomeTextureCoordsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, skydomeTextureCoordsBuffer);
	glBufferData(GL_ARRAY_BUFFER, skydome.texcoords.size() * sizeof(vec2), &skydome.texcoords[0], GL_STATIC_DRAW);
}

int main(void)
{
	// setup Window, OpenGL and scene
	if (!initWindow()) return -1;
	sceneSetup();
	openGlSetup();

	makeGenAndBindBuffer();

	double startTime, currentTime;
	float passedTime = 0;
	startTime = (double)clock();

	do{
		// calculate the time
		currentTime = (double)clock();
		float lastFramePassedTime = passedTime;
		passedTime = (currentTime - startTime) / (CLOCKS_PER_SEC);
		float passedTimePerFrame = passedTime - lastFramePassedTime;

		// update
		updateScene(passedTimePerFrame, passedTime); GLERROR;
		updateCamera(passedTimePerFrame); GLERROR;
		light->updateLight(&lightIntensity,&lightDirection,&daytime,passedTimePerFrame); GLERROR;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw skydome
		// now use the second program
		glUseProgram(skydomeProgramID);
		GLERROR;

		cloud -> sendUniformsToSkydomeShader(skydomecameraPositionLocation,skydomeCameraDirectionLocation,skydomeLightDirectionLocation, skydomelightIntensityLocation,
		 SkydomeMatrixID, cameraPosition, cameraDirectionVector, lightDirection, lightIntensity, MVP);

		// 6th attribute buffer : vertices
		glEnableVertexAttribArray(10);
		glBindBuffer(GL_ARRAY_BUFFER, skydomeVertexbuffer);
		glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		GLERROR;

		// 7th attribute buffer : normals
		glEnableVertexAttribArray(11);
		glBindBuffer(GL_ARRAY_BUFFER, skyDomeNormalbuffer);
		glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 8th attribute buffer : 2D texture coordinates
		glEnableVertexAttribArray(12);
		glBindBuffer(GL_ARRAY_BUFFER, skydomeTextureCoordsBuffer);
		glVertexAttribPointer(12, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skydomeIndexbuffer);

		glDrawElements(GL_TRIANGLES, skydome.indices.size(), GL_UNSIGNED_INT, NULL);

		glDisableVertexAttribArray(10);
		glDisableVertexAttribArray(11);
		glDisableVertexAttribArray(12);

		// draw cloud
		// Use our shader
		glUseProgram(sphereProgramID);
		GLERROR;

		cloud ->sendUniformsToCloudShader(cameraLocation, cameraDirection, cameraPosition, cameraDirectionVector,
			 MatrixID,  MVP,  morphingSpeedLocation,  sphereCenterLocation,
			 spheres,  timeLocation,  expansionDirectionLocation,  expansionDirection,
			 lightDirectionLocation,  lightDirection,
			 lightIntensityLocation,  lightIntensity, passedTime);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);
		GLERROR;

		// 2nd attribute buffer : normals
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		GLERROR;

		// 3rd attribute buffer : 2D texture coordinates
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, textureCoordsBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		GLERROR;

		// 4th attribute buffer : 3D offset (instanced for each sphere object)
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, offsetsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * offsets.size(), &offsets[0], GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(3, 1);
		GLERROR;

		// 5th attribute buffer : single random value per sphere (instanced for each sphere object)
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, nonseBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nonses.size(), &nonses[0], GL_STATIC_DRAW);
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(4, 1);
		GLERROR;

		// 6th attribute buffer : single random value per sphere (instanced for each sphere object)
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, scaleBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * scales.size(), &scales[0], GL_STATIC_DRAW);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(5, 1);
		GLERROR;


		// 7th attribute buffer : single random value per sphere (instanced for each sphere object)
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, thresholdNormalsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * thresholdNormals.size(), &thresholdNormals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(6, 1);
		GLERROR;

		// 8th attribute buffer : single random value per sphere (instanced for each sphere object)
		glEnableVertexAttribArray(7);
		glBindBuffer(GL_ARRAY_BUFFER, frontPointsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * frontPoints.size(), &frontPoints[0], GL_STATIC_DRAW);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(7, 1);
		GLERROR;

		// 9th attribute buffer : single random value per sphere (instanced for each sphere object)
		glEnableVertexAttribArray(8);
		glBindBuffer(GL_ARRAY_BUFFER, backPointsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * backPoints.size(), &backPoints[0], GL_STATIC_DRAW);
		glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(8, 1);
		GLERROR;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		glDrawElementsInstanced(GL_TRIANGLES, spheres.at(0).indices.size(), GL_UNSIGNED_INT, NULL, offsets.size());
		GLERROR;

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
		glDisableVertexAttribArray(6);
		glDisableVertexAttribArray(7);
		glDisableVertexAttribArray(8);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	// close opengle and done glfw
	glfwTerminate();

	return 0;
}

