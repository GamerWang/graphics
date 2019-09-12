#include <gl\glew.h>
#include <glm\glm.hpp>

#include <iostream>
#include <fstream>
#include "MeGlWindow.h"

using namespace std;
using glm::vec2;
using glm::vec3;

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

GLuint programID;

string readShaderCode(const char* fileName) {
	ifstream meInput(fileName);
	if (!meInput.good()) {
		cout << "File failed to load..." << fileName << endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(meInput),
		std::istreambuf_iterator<char>()
	);
}

void sendDataToOpenGL() {
	GLfloat verts[] =
	{
		+0.0f, +0.0f,
		+1.0f, +0.0f, +0.0f,
		+0.2f, +0.2f,
		+0.0f, +1.0f, +0.0f,
		-0.2f, +0.2f,
		+0.0f, +0.0f, +1.0f,
	};
	GLuint vertexBufferID;
	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts),
		verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (char*)(sizeof(float) * 2));

	GLushort indices[] = { 0, 1, 2};
	GLuint indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),
		indices, GL_STATIC_DRAW);
}

void installShaders() {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapter[1];
	string temp = readShaderCode("./VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	temp = readShaderCode("./FragmentShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	GLint compileStatus;
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GLU_TRUE) {
		GLint infoLogLength;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		glGetShaderInfoLog(vertexShaderID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glUseProgram(programID);
}

void MeGlWindow::Update(bool isRed) {
	GLint dominatingColorUniformLocation = glGetUniformLocation(programID, "dominatingColor");
	GLint yFlipUniformLocation = glGetUniformLocation(programID, "yFlip");
	GLint positionOffsetUniformLocation = glGetUniformLocation(programID, "positionOffset");
	
	vec3 dominatingColor(1.0f, 0.0f, 0.0f);
	float yFlip = 1;
	vec2 offset = vec2(offsetR);

	if (!isRed) {
		dominatingColor.r = 0;
		dominatingColor.b = 1;
		yFlip = -1;
		offset = vec2(offsetB);
	}

	offset.x /= width();
	offset.y /= height();

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform1f(yFlipUniformLocation, yFlip);
	glUniform2fv(positionOffsetUniformLocation, 1, &offset[0]);
}

void MeGlWindow::Draw() {
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
}


void MeGlWindow::initializeGL()
{
	glewInit();
	sendDataToOpenGL();
	installShaders();

	offsetR = vec2(0, 0);
	offsetB = vec2(0, 0);
	moveSpeed = 2;
}

void MeGlWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	Update(true);
	Draw();

	Update(false);
	Draw();
}

void MeGlWindow::keyPressEvent(QKeyEvent *event) {
	switch (event->key())
	{
	case Qt::Key_W:
		offsetR.y += moveSpeed;
		break;
	case Qt::Key_S:
		offsetR.y += -moveSpeed;
		break;
	case Qt::Key_A:
		offsetR.x += -moveSpeed;
		break;
	case Qt::Key_D:
		offsetR.x += moveSpeed;
		break;
	case Qt::Key_Up:
		offsetB.y += moveSpeed;
		break;
	case Qt::Key_Down:
		offsetB.y += -moveSpeed;
		break;
	case Qt::Key_Left:
		offsetB.x += -moveSpeed;
		break;
	case Qt::Key_Right:
		offsetB.x += moveSpeed;
		break;
	default:
		break;
	}
	updateGL();
}

void MeGlWindow::keyReleaseEvent(QKeyEvent *event) {
}