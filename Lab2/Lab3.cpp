// Lab2.cpp : Defines the entry point for the console application.
//


#include <GL\glew.h>
#include "GLObject.h"
#include "GLContext.h"
#include "ShaderProgram.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include <assimp\Importer.hpp>
#include "Model.h"
#include "stdafx.h"
#include <memory>
#include "Camera.h"
#include "Light.h"
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma region Variables
std::unique_ptr<Model> model;
std::unique_ptr<Mesh> triangleMesh;
//std::unique_ptr<ShaderProgram>  shaderProgram;
ShaderProgram* shaderProgram;
GLuint screenWidth = 800, screenHeight = 600;
Camera camera;
const GLfloat cameraZoomSpeed = 0.1f;
bool firstMotion = true;
int deltaTime, oldTimeSinceStart = 0;
Light pointLight, directionalLight;
std::vector<Light> gLights;//��������� �����
float lightMoveSpeed = 0.001f;
bool pointLightIsMoving = false;
bool dirLightIsMoving = false;
float pointLightXMovement = 0.0f;
float pointLightYMovement = 0.0f;
float dirLightXMovement = 0.0f;
float dirLightYMovement = 0.0f;
#pragma endregion


enum MouseEnum
{
	MOUSE_LEFT_BUTTON = 0,
	MOUSE_MIDDLE_BUTTON = 1,
	MOUSE_RIGHT_BUTTON = 2,
	MOUSE_SCROLL_UP = 3,
	MOUSE_SCROLL_DOWN = 4
};



template <typename T>
void SetLightUniform(ShaderProgram* program, const char* propertyName, size_t lightIndex, const T& value) {
	std::ostringstream ss;
	//ss << "allLights[" << lightIndex << "]." << propertyName;
	ss << L_ARR << "[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();

	program->SetUniform(uniformName.c_str(), value);
}

#pragma region Callbacks
void display(void)
{
	
}

void idle()
{
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = timeSinceStart - oldTimeSinceStart;
	oldTimeSinceStart = timeSinceStart;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderProgram->Use();

	glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shaderProgram->SetUniform(M_PROJ, projection);
	shaderProgram->SetUniform(M_VIEW, view);
	// Draw the loaded model
	glm::mat4 modelMatrix;
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.5f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
	shaderProgram->SetUniform(M_MODEL, modelMatrix);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
	shaderProgram->SetUniform(NORM_MATRIX, normalMatrix);
#pragma region Lighting
	//����
	//ShaderProgram* sp = shaderProgram.get();
	ShaderProgram* sp = shaderProgram;
	//if (pointLightIsMoving)
	//{
	//	pointLightXMovement += lightMoveSpeed;
	//	pointLightYMovement += lightMoveSpeed;
	//	/*gLights[0].position.x += cos(10*pointLightXMovement)/80.0f;
	//	gLights[0].position.y += sin(8 * pointLightYMovement) / 50.0f;
	//	gLights[1].position.x += cos(10 * pointLightXMovement) / 80.0f;
	//	gLights[1].position.y += sin(8 * pointLightYMovement) / 50.0f;*/
	//	/*for (int i = 0; i < 4; i++)
	//	{
	//		gLights[i].position.x += cos(10 * pointLightXMovement) / 80.0f;
	//		gLights[i].position.y += sin(8 * pointLightYMovement) / 50.0f;
	//	}*/
	//	
	//	
	//}

	if (pointLightIsMoving)
	{
		pointLightXMovement += lightMoveSpeed;
		pointLightYMovement += lightMoveSpeed;
	}
	if (dirLightIsMoving)
	{
		dirLightXMovement += lightMoveSpeed;
		dirLightYMovement += lightMoveSpeed;
	}
	for (size_t i = 0; i < gLights.size(); ++i) 
	{
		if (gLights[i].lightType == DIR_LIGHT&&dirLightIsMoving)
		{
				gLights[i].position.x += cos(5 * dirLightXMovement) / 70.0f;
				gLights[i].position.y += sin(8 * dirLightYMovement) / 50.0f;
		}
		else if (gLights[i].lightType == POINT_LIGHT&&pointLightIsMoving)
		{
						
				gLights[i].position.x += cos(10 * pointLightXMovement) / 80.0f;
				gLights[i].position.y += sin(8 * pointLightYMovement) / 50.0f;
		}
		//SetLightUniform(shaderProgram.get())
		//SetLightUniform(shaderProgram.get(), L_POS, i, gLights[i].position);
		////gLights[i].intensities = glm::vec3(0, 1, 1);
		//SetLightUniform(shaderProgram.get(), L_INTENS, i, gLights[i].intensities);
		//SetLightUniform(shaderProgram.get(), L_ATTEN, i, gLights[i].attenuation);
		//SetLightUniform(shaderProgram.get(), L_AMBIENTCOEFF, i, gLights[i].ambientCoefficient);
		//SetLightUniform(shaderProgram.get(), "coneAngle", i, gLights[i].coneAngle);
		//SetLightUniform(shaderProgram.get(), "coneDirection", i, gLights[i].coneDirection);
		SetLightUniform(shaderProgram, L_POS, i, gLights[i].position);
		//gLights[i].intensities = glm::vec3(0, 1, 1);
		SetLightUniform(shaderProgram, L_INTENS, i, gLights[i].intensities);
		SetLightUniform(shaderProgram, L_ATTEN, i, gLights[i].attenuation);
		SetLightUniform(shaderProgram, L_AMBIENTCOEFF, i, gLights[i].ambientCoefficient);
		SetLightUniform(shaderProgram, "coneAngle", i, gLights[i].coneAngle);
		SetLightUniform(shaderProgram, "coneDirection", i, gLights[i].coneDirection);
	}
#pragma endregion
	
	shaderProgram->SetUniform(CAM_POS, camera.Position);
	model->Draw(shaderProgram);

	glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if ((button == MouseEnum::MOUSE_SCROLL_DOWN) || (button == MouseEnum::MOUSE_SCROLL_UP)) // It's a wheel event
	{
		// Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
		if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
		//printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);

		GLfloat yOffset = button == MOUSE_SCROLL_DOWN ? -cameraZoomSpeed : cameraZoomSpeed;
		camera.ProcessMouseScroll(yOffset);
	}
	else if (button == MOUSE_LEFT_BUTTON || button == MOUSE_RIGHT_BUTTON || button == MOUSE_MIDDLE_BUTTON&&state==GLUT_UP)
		firstMotion = true;
	else {  // normal button event
		//printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
	}
}


int prevX, prevY;

void motion(int x, int y)
{
	if (firstMotion)
	{
		prevX = x;
		prevY = y;
		firstMotion = false;
	}
	else {
		int dx = x - prevX;
		int dy = y - prevY;
		//printf("dx:%i dy:%i", dx,dy);
		camera.ProcessMouseMovement(dx, -dy);
		prevX = x;
		prevY = y;
	}
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_RIGHT:
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	case GLUT_KEY_LEFT:
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case 's':
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case 'a':
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case 'd':
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	case 'q':
		pointLightIsMoving = !pointLightIsMoving;
		break;
	case 'e':
		dirLightIsMoving = !dirLightIsMoving;
		break;
	}
}
#pragma endregion


void InitLights()
{
	
	/*Light pointLight;
	pointLight.lightType = 0;
	pointLight.position = glm::vec4(0, 0, 1, 1);
	pointLight.intensities = glm::vec3(0, 0.5, 0); 
	pointLight.attenuation = 0.5f;
	pointLight.ambientCoefficient = 0.005f; 
	
	gLights.push_back(pointLight);
	Light pointLight2;
	pointLight2 = pointLight;
	pointLight2.position = glm::vec4(0, 0, 1.2, 1);
	gLights.push_back(pointLight2);

	Light pointLight3;
	pointLight3 = pointLight2;
	pointLight3.position = glm::vec4(0.2, 0, 1.2, 1);
	gLights.push_back(pointLight3);

	Light pointLight4;
	pointLight4 = pointLight3;
	pointLight4.position.y -= 0.2;
	gLights.push_back(pointLight4);*/

	directionalLight.position = glm::vec4(1, 0.8, 0.6, 0); //w == 0 indications a directional light
	directionalLight.intensities = glm::vec3(1, 1, 1); 
	directionalLight.ambientCoefficient = 0.06f;
	directionalLight.lightType = DIR_LIGHT;
	gLights.push_back(directionalLight);
	shaderProgram->SetUniform(NUM_LIGHTS, (GLint)gLights.size());//������������� ���������� ���������� �����
}


int main(int argc, char** argv)
{
	Window wnd(screenWidth,screenHeight,10,10,"Lab2");
	GLContext::Initialize(argc, argv, wnd, display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(special);
	glutKeyboardFunc(keyboard);
	//shaderProgram = std::make_unique<ShaderProgram>();
	shaderProgram = new ShaderProgram();
	model = std::unique_ptr<Model>(new Model("Models\\room\\room.obj"));

	VertexShader vertexShader("Shaders\\shadows.vert");
	FragmentShader fragmentShader("Shaders\\shadows.frag");
	shaderProgram->AttachShader(vertexShader);
	shaderProgram->AttachShader(fragmentShader);
	shaderProgram->Use();
	
	InitLights();
	
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	
    return 0;
}
