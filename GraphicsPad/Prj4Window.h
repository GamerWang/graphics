#pragma once
#include <QtOpenGL\qglwidget>

class Prj4Window : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();

	int lastUpdate;

	float rotation;
public:
};

