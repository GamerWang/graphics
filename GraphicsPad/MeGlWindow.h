#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>
#include <QtGui\QKeyEvent>
#include <QtCore\qtimer.h>
#include <QtCore\qdatetime.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

using glm::vec2;
using glm::vec3;

class MeGlWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();

	void Update(bool isRed);
	void Draw();

	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

	vec2 offsetR;
	vec2 offsetB;
	float moveSpeed;
public:
};

#endif
