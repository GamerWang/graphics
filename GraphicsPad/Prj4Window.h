#pragma once
#include <QtOpenGL\qglwidget>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>

using glm::vec3;
using glm::mat4;

class Prj4Window : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void keyPressEvent(QKeyEvent*);

	int lastUpdate;

	vec3 cameraPosition;
	vec3 cameraUp;
	vec3 cubePosition;
	float cubeRotation;

	vec3 pointLightPosition;
	vec3 directionalLightDir;
	vec3 ambientLightColor;

	vec3 teapotDiffuseColor;
	vec3 teapotSpecularColor;
	float teapotGlossiness;
public:
	~Prj4Window();
};

