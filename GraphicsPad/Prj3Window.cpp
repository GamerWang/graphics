#include <gl\glew.h>

#include "Prj3Window.h"

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
		+0.1f, +0.1f,
		+0.0f, +0.2f,
		-0.1f, +0.1f,

		+0.0f, +1.0f,
		+1.0f, +0.0f,
		+0.0f, -1.0f,
		-1.0f, +0.0f,
	};

	GLuint triangleVertexBufferID;
	glGenBuffers(1, &triangleVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	GLushort indices[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
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
	glewInit();
	sendDataToOpenGL();
	installShaders();

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	deltaTime = 15;
	timer->start(deltaTime);

	speed = 0.1f;

	QTime time = QTime::currentTime();
	int allMsec = time.second() * 1000 + time.msec();

	srand(allMsec);

	float dirx = rand() % 100;
	dirx /= 100;
	float diry = rand() % 100;
	diry /= 100;

	moveDir = vec2(dirx, diry);

	position = vec2(0, 0);

	lastUpdate = allMsec;
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
	position += moveDir * speed * timePassed;
	vec2 offset = position;

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform2fv(positionOffsetUniformLocation, 1, &offset[0]);

	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);

	dominatingColor = vec3(0.0f, 1.0f, 0.0f);
	offset = vec2(0, 0);

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform2fv(positionOffsetUniformLocation, 1, &offset[0]);

	glDrawArrays(GL_LINE_LOOP, 4, 4);
}