#include <GL\glew.h>
#include <iostream>
#include <fstream>

#include "MeGlWindow.h"

using namespace std;

boolean checkStatus(GLuint objectId, PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
	PFNGLGETSHADERINFOLOGPROC objectGetInfoLogFunc, GLenum statusType) {
	GLint status;
	objectPropertyGetterFunc(objectId, statusType, &status);
	if (status != GL_TRUE) {
		GLint infoLogLength;
		objectPropertyGetterFunc(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLsizei bufferSize;
		GLchar* buffer = new GLchar[infoLogLength];
		objectGetInfoLogFunc(objectId, infoLogLength, &bufferSize, buffer);

		std::cout << buffer;

		delete buffer;

		return false;
	}

	return true;
}

bool checkShaderStatus(GLuint shaderId) {
	return checkStatus(shaderId, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLuint programId) {
	return checkStatus(programId, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

string readShaderCode(const char* fileName) {
	ifstream input(fileName);
	if (!input.good()) {
		cout << "File failed to load... " << fileName;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

void installShaders() {
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	const GLchar* adapter[1];
	string code;
	code = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = code.c_str();
	glShaderSource(vertexShaderId, 1, adapter, 0);

	code = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = code.c_str();
	glShaderSource(fragmentShaderId, 1, adapter, 0);

	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);
	if (!(checkShaderStatus(vertexShaderId) && checkShaderStatus(fragmentShaderId))) return;

	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	if (!checkProgramStatus(programId)) return;

	glUseProgram(programId);
}

void draw2Triangles() {
	const GLfloat RED_TRIANGLE_Z = -0.5f;
	const GLfloat GREEN_TRIANGLE_Z = 0.5f;
	GLfloat verts[] = {
		+0.0f, -1.0f, RED_TRIANGLE_Z, +1.0f, +0.0f, +0.0f,
		+1.0f, +1.0f, RED_TRIANGLE_Z,  +1.0f, +0.0f, +0.0f,
		-1.0f, +1.0f, RED_TRIANGLE_Z,  +1.0f, +0.0f, +0.0f,

		+0.0f, +1.0f, -1.0f,  +0.0f, +1.0f, +0.0f,
		-1.0f, -1.0f, GREEN_TRIANGLE_Z,  +0.0f, +1.0f, +0.0f,
		+1.0f, -1.0f, GREEN_TRIANGLE_Z,  +0.0f, +1.0f, +0.0f,
	};
	GLuint vertBufferId;
	glGenBuffers(1, &vertBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	//glBufferSubData() fill the data later
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (char*)(sizeof(GLfloat) * 3));

	GLushort indices[] = { 0, 1, 2, 3, 4, 5 };
	GLuint indexBufferId;
	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

int triCount = 0;
void drawTriangles() {
	GLuint vertBufferId;
	glGenBuffers(1, &vertBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
	glBufferData(GL_ARRAY_BUFFER, 20 * 3 * 6 * sizeof(GLfloat), 0, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (char*)(sizeof(GLfloat) * 3));
}

void sendNextTriangleToOpenGL() {
	if (triCount == 20) return;
	GLfloat verts[] = {
		-1.0f + 0.1f * triCount,		-1.0f, 0, +1.0f, +0.0f, +0.0f,
		-1.0f + 0.1f * (triCount + 1),	-1.0f, 0, +1.0f, +0.0f, +0.0f,
		-1.0f + 0.1f * triCount,		+0.0f, 0, +1.0f, +0.0f, +0.0f,
	};
	glBufferSubData(GL_ARRAY_BUFFER, triCount * sizeof(verts), sizeof(verts), verts);
	triCount++;
}

void MeGlWindow::initializeGL() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	installShaders();

	// ex1
	//draw2Triangles();

	// ex2
	drawTriangles();
}

void MeGlWindow::paintGL() {
	glClearColor(0, 0, 0, 1);
	glViewport(0, 0, width(), height());

	// ex1 depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	// ex2 color buffer
	sendNextTriangleToOpenGL();
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, (triCount - 1) * 3, triCount * 3);
}