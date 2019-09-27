#ifndef PRJ_3_WINDOW
#define PRJ_3_WINDOW

#include<QtOpenGL\qglwidget>
#include <QtGui\QKeyEvent>
#include <QtCore\qtimer.h>
#include <QtCore\qdatetime.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

using glm::vec2;
using glm::vec3;

class Prj3Window : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();

	virtual void installShaders();
	virtual void sendDataToOpenGL();
	virtual string readShaderCode(const char* fileName);

	float deltaTime;

	vec2 position;
	vec2 moveDir;
	float speed;

	int lastUpdate;

	vec2 topCorner;
	vec2 rightCorner;
	vec2 bottomCorner;
	vec2 leftCorner;

	GLuint programID;
};

#endif