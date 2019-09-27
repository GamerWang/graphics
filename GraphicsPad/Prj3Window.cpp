#include <gl\glew.h>

#include "Prj3Window.h"

float Vec2Cross(vec2 v1, vec2 v2) {
	return (v1.x * v2.y - v2.x * v1.y);
}

float Vec2Dot(vec2 v1, vec2 v2) {
	return (v1.x * v2.x + v1.y * v2.y);
}

string Prj3Window::readShaderCode(const char* fileName) {
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

void Prj3Window::sendDataToOpenGL() {

	GLfloat verts[] = {
		+0.0f, +0.0f,
		+0.01f, +0.01f,
		+0.0f, +0.02f,
		-0.01f, +0.01f,
		+0.0f, -0.01f,
		+0.02f, +0.01f,
		+0.0f, +0.03f,
		-0.02f, +0.01f,

		//+0.0f, +1.0f,
		topCorner.x, topCorner.y,
		//+1.0f, +0.0f,
		rightCorner.x, rightCorner.y,
		//+0.0f, -1.0f,
		bottomCorner.x, bottomCorner.y,
		//-1.0f, +0.0f,
		leftCorner.x, leftCorner.y,
	};

	GLuint triangleVertexBufferID;
	glGenBuffers(1, &triangleVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	GLushort indices[] = { 
		0, 5, 1, 
		0, 4, 5,
		1, 6, 2,
		1, 5, 6,
		2, 6, 7,
		2, 7, 3,
		3, 7, 4,
		3, 4, 0,
	};
	GLuint indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void Prj3Window::installShaders() {
	GLuint vertID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID = glCreateShader(GL_FRAGMENT_SHADER);

	const char* adapter[1];
	string temp = readShaderCode("./prj3_vert.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertID, 1, adapter, 0);
	temp = readShaderCode("./prj3_frag.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(fragID, 1, adapter, 0);

	glCompileShader(vertID);
	glCompileShader(fragID);

	GLint compileStatus;
	glGetShaderiv(vertID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GLU_TRUE) {
		GLint infoLogLength;
		glGetShaderiv(vertID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		glGetShaderInfoLog(vertID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
	}
	glGetShaderiv(fragID, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GLU_TRUE) {
		GLint infoLogLength;
		glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		glGetShaderInfoLog(fragID, infoLogLength, &bufferSize, buffer);
		cout << buffer << endl;

		delete[] buffer;
	}

	programID = glCreateProgram();
	glAttachShader(programID, vertID);
	glAttachShader(programID, fragID);
	glLinkProgram(programID);

	glUseProgram(programID);
}

void Prj3Window::initializeGL() {
	// set core data
	topCorner = vec2(0.0, 1.0);
	rightCorner = vec2(1.0, 0.0);
	bottomCorner = vec2(0.0, -1.0);
	leftCorner = vec2(-1.0, 0.0);

	vec2* normals;
	normals[0] = vec2(-1.0f, -1.0f);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	deltaTime = 15;
	timer->start(deltaTime);

	speed = 0.8f;

	QTime time = QTime::currentTime();
	int allMsec = time.second() * 1000 + time.msec();

	srand(allMsec);

	float dirx = rand() % 100;
	dirx /= 100;
	dirx -= 0.5f;
	float diry = rand() % 100;
	diry /= 100;
	diry -= 0.5f;

	moveDir = vec2(dirx, diry);

	position = vec2(0, 0);

	lastUpdate = allMsec;
	
	// initilize gl functions
	glewInit();
	sendDataToOpenGL();
	installShaders();
}

void Prj3Window::paintGL() {
	QTime time = QTime::currentTime();
	int currentUpdate = time.second() * 1000 + time.msec();
	float timePassed = currentUpdate - lastUpdate;
	timePassed /= 1000;
	lastUpdate = currentUpdate;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width(), height());

	GLint dominatingColorUniformLocation = glGetUniformLocation(programID, "dominatingColor");
	GLint positionOffsetUniformLocation = glGetUniformLocation(programID, "positionOffset");

	vec3 dominatingColor(1.0f, 0.0f, 0.0f);

	vec2 centerPos = position + vec2(0, 0.01f);
	vec2 normal;
	bool hit = false;
	// check the object hit using cross product
	if (Vec2Cross(rightCorner - centerPos, topCorner - centerPos) < 0) {
		hit = true;
	}
	else if (Vec2Cross(topCorner - centerPos, leftCorner - centerPos) < 0) {
		hit = true;
	}
	else if (Vec2Cross(leftCorner - centerPos, bottomCorner - centerPos) < 0) {
		hit = true;
	}
	else if (Vec2Cross(bottomCorner - centerPos, rightCorner - centerPos) < 0) {
		hit = true;
	}
	// check the object hit using dot product


	if (hit) {
		speed = 0;
	}
	

	position += moveDir * speed * timePassed;
	vec2 offset = position;

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform2fv(positionOffsetUniformLocation, 1, &offset[0]);

	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, 0);

	dominatingColor = vec3(0.0f, 1.0f, 0.0f);
	offset = vec2(0, 0);

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform2fv(positionOffsetUniformLocation, 1, &offset[0]);

	glDrawArrays(GL_LINE_LOOP, 8, 4);
}