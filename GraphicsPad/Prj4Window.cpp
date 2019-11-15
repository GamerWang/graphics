#include <gl\glew.h>
#include <iostream>
#include <fstream>
#include <QtGui\QKeyEvent>
#include <QtCore\qtimer.h>
#include <QtCore\qdatetime.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Prj4Window.h"
#include "Primitives\Vertex.h"
#include "Primitives\ShapeGenerator.h"
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);
GLuint programID;
GLuint numIndices;
Camera camera;

#define ROTATION_SPEED 10.0f
#define CAMERA_MOVE_STEP 0.2f
#define CAMERA_ROTATION_STEP 1.0f

void sendDataToOpenGL()
{
	ShapeData shape = ShapeGenerator::makeTeapot();

	GLuint vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (char*)(sizeof(float) * 3));

	GLuint indexArrayBufferID;
	glGenBuffers(1, &indexArrayBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
	numIndices = shape.numIndices;
	shape.cleanup();
}

void Prj4Window::paintGL()
{
	QTime time = QTime::currentTime();
	int currentUpdate = time.second() * 1000 + time.msec();
	float timePassed = currentUpdate - lastUpdate;
	timePassed /= 1000;
	lastUpdate = currentUpdate;

	vec3 rotationAxis = vec3(0, 1, 0);

	cubeRotation += (timePassed * ROTATION_SPEED);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	mat4 translationMatrix = glm::translate(mat4(), cubePosition);
	mat4 rotationMatrixOrigin = glm::rotate(mat4(), -90.0f, vec3(1, 0, 0));
	mat4 rotationMatrix1 = glm::rotate(mat4(), cubeRotation, rotationAxis);

	mat4 objectToWorldMatrix = translationMatrix * rotationMatrix1 * rotationMatrixOrigin;
	//mat4 worldToViewMatrix = glm::lookAt(cameraPosition, cubePosition, cameraUp);
	mat4 worldToViewMatrix = glm::lookAt(cameraPosition, cameraPosition + vec3(0, 0, -10.0f), cameraUp);
	
	mat4 projectionMatrix = glm::perspective(60.0f, ((float)width()) / height(), 0.1f, 1000.0f);
	mat4 fullTransformMatrix = projectionMatrix * worldToViewMatrix * objectToWorldMatrix;

	GLint fullTransformMatrixUniformLocation =
		glGetUniformLocation(programID, "fullTransformMatrix");

	glUniformMatrix4fv(fullTransformMatrixUniformLocation, 1,
		GL_FALSE, &fullTransformMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
}

void Prj4Window::keyPressEvent(QKeyEvent* e) {
	vec4 cameraPositionVec4 = vec4(cameraPosition, 1);
	vec4 cameraUpVec4 = vec4(cameraUp, 1);

	switch (e->key())
	{
	case Qt::Key::Key_Q:
		cameraPositionVec4 += (vec4(0, 0, -1,0) * CAMERA_MOVE_STEP);
		break;
	case Qt::Key::Key_E:
		cameraPositionVec4 += (vec4(0, 0, 1, 0) * CAMERA_MOVE_STEP);
		break;
	case Qt::Key::Key_A:
		cameraPositionVec4 += (vec4(-1, 0, 0, 0) * CAMERA_MOVE_STEP);
		break;
	case Qt::Key::Key_D:
		cameraPositionVec4 += (vec4(1, 0, 0, 0) * CAMERA_MOVE_STEP);
		break;
	case Qt::Key::Key_W:
		cameraPositionVec4 += (vec4(0, -1, 0, 0) * CAMERA_MOVE_STEP);
		break;
	case Qt::Key::Key_S:
		cameraPositionVec4 += (vec4(0, 1, 0, 0) * CAMERA_MOVE_STEP);
		break;
	}

	cameraPosition = vec3(cameraPositionVec4.x, cameraPositionVec4.y, cameraPositionVec4.z);
	cameraUp = vec3(cameraUpVec4.x, cameraUpVec4.y, cameraUpVec4.z);

	repaint();
}

bool checkStatus(
	GLuint objectID,
	PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
	GLenum statusType)
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool checkShaderStatus(GLuint shaderID)
{
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programID)
{
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName)
{
	ifstream meInput(fileName);
	if (!meInput.good())
	{
		cout << "File failed to load..." << fileName;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>());
}

void installShaders()
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string temp = readShaderCode("prj4_vert.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("prj4_frag.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID))
		return;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	if (!checkProgramStatus(programID))
		return;

	glUseProgram(programID);
}

void Prj4Window::initializeGL()
{
	resize(600, 600);

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	int deltaTime = 15;
	timer->start(deltaTime);

	QTime time = QTime::currentTime();
	int allMsec = time.second() * 1000 + time.msec();
	lastUpdate = allMsec;

	cubePosition = vec3(0, 0, -10.0f);
	cameraPosition = vec3(0, 1.2f, 0);
	cameraUp = vec3(0, 1, 0);

	cubeRotation = 0;

	setMouseTracking(true);

	glewInit();
	glEnable(GL_DEPTH_TEST);
	sendDataToOpenGL();
	installShaders();
}

Prj4Window::~Prj4Window()
{
	glUseProgram(0);
	glDeleteProgram(programID);
}