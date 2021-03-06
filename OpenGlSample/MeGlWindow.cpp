#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <iostream>
#include <fstream>
#include <QtGui\qmouseevent>
#include <QtGui\qkeyevent>

#include "MeGlWindow.h"
#include "Vertex.h"
#include "ShapeData.h"
#include "ShapeGenerator.h"
#include "Camera.h"

using namespace std;
using namespace glm;

int triCount = 0;
GLuint programId;
int numIndices;
Camera camera;

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

	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	if (!checkProgramStatus(programId)) return;

	glUseProgram(programId);
}

MeGlWindow::~MeGlWindow() {
	glUseProgram(0);
	glDeleteProgram(programId);
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
	Vertex verts[] = {
		glm::vec3(-1.0f + 0.1f * triCount,			-1.0f, 0.0f), glm::vec3(+1.0f, +0.0f, +0.0f),
		glm::vec3(-1.0f + 0.1f * (triCount + 1),	-1.0f, 0.0f), glm::vec3(+1.0f, +0.0f, +0.0f),
		glm::vec3(-1.0f + 0.1f * triCount,			+0.0f, 0.0f), glm::vec3(+1.0f, +0.0f, +0.0f),
	};
	glBufferSubData(GL_ARRAY_BUFFER, triCount * sizeof(verts), sizeof(verts), verts);
	triCount++;
}

void drawTriangle() {
	ShapeData triangle = ShapeGenerator::makeTriangle();

	GLuint vertBufferId;
	glGenBuffers(1, &vertBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
	glBufferData(GL_ARRAY_BUFFER, triangle.vertexBufferSize(), triangle.vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(sizeof(GLfloat) * 3));

	GLuint indexArrayBufferID;
	glGenBuffers(1, &indexArrayBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle.indexBufferSize(), triangle.indices, GL_STATIC_DRAW);

	numIndices = triangle.numIndices;
	triangle.cleanup();
}

void drawShape(ShapeData shape) {

	GLuint vertBufferId;
	glGenBuffers(1, &vertBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferId);
	glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(sizeof(GLfloat) * 3));

	GLuint indexArrayBufferID;
	glGenBuffers(1, &indexArrayBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexArrayBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);

	numIndices = shape.numIndices;
	shape.cleanup();
}

void drawInstanced() {
	drawShape(ShapeGenerator::makeArrow());

	//GLint matrixUniformLocation = glGetUniformLocation(programId, "matrix");
	//mat4 projectionMatrix = perspective(20.0f, width / height, 0.1f, 10.0f);
	//mat4 matrix[] = {
	//	// ex6 camera
	//	projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(-1.0f, 0.0f, -3.0f)) * glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f)),
	//	projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(1.0f, 0.0f, -3.75f)) * glm::rotate(125.0f, vec3(0.0f, 1.0f, 0.0f)),
	//};

	//GLuint transformationMatrixBufferId;
	//glGenBuffers(1, &transformationMatrixBufferId);
	//glBindBuffer(GL_ARRAY_BUFFER, transformationMatrixBufferId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(matrix), matrix, GL_STATIC_DRAW);

	GLuint transformationMatrixBufferId;
	glGenBuffers(1, &transformationMatrixBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, transformationMatrixBufferId);

	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * 2, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(GL_FLOAT) * 0));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(GL_FLOAT) * 4));
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(GL_FLOAT) * 8));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(GL_FLOAT) * 12));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
}

GLuint vertextBufferId;
GLuint indexBufferId;
GLuint cubeNumIndices;
GLuint arrowNumIndices;
GLuint cubeVertexArrayObjectId;
GLuint arrowVertexArrayObjectId;
GLuint arrowIndexDataByteOffset;
void drawDifferentInstances() {
	ShapeData cube = ShapeGenerator::makeCube();
	ShapeData arrow = ShapeGenerator::makeArrow();

	glGenBuffers(1, &vertextBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertextBufferId);
	glBufferData(GL_ARRAY_BUFFER, cube.vertexBufferSize() + arrow.vertexBufferSize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, cube.vertexBufferSize(), cube.vertices);
	glBufferSubData(GL_ARRAY_BUFFER, cube.vertexBufferSize(), arrow.vertexBufferSize(), arrow.vertices);

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize() + arrow.indexBufferSize(), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, cube.indexBufferSize(), cube.indices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, cube.indexBufferSize(), arrow.indexBufferSize(), arrow.indices);

	cubeNumIndices = cube.numIndices;
	arrowNumIndices = arrow.numIndices;

	arrowIndexDataByteOffset = cube.indexBufferSize();

	glGenVertexArrays(1, &cubeVertexArrayObjectId);
	glGenVertexArrays(1, &arrowVertexArrayObjectId);

	glBindVertexArray(cubeVertexArrayObjectId);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertextBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GL_FLOAT) * 0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(GL_FLOAT) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	glBindVertexArray(arrowVertexArrayObjectId);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertextBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(cube.vertexBufferSize() + sizeof(GL_FLOAT) * 0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(cube.vertexBufferSize() + sizeof(GL_FLOAT) * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	cube.cleanup();
	arrow.cleanup();
}

void MeGlWindow::initializeGL() {
	glewInit();
	glEnable(GL_DEPTH_TEST);
	installShaders();

	// ex1
	//draw2Triangles();

	// ex2
	//drawTriangles();

	// ex3
	//drawTriangle();

	// ex4
	//drawCube();

	// ex5    // ex6 camera
	setMouseTracking(true);
	//drawInstanced();

	// ex7
	drawDifferentInstances();
}

void paintDepthBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
}

void paintColorBuffer() {
	sendNextTriangleToOpenGL();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, (triCount - 1) * 3, triCount * 3);
}

void paintUniform() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glm::vec3 dominatingColor(0.0f, 1.0f, 0.0f);
	GLint dominatingColorUniformLocation = glGetUniformLocation(programId, "dominatingColor");
	GLint yFlipUniformLocation = glGetUniformLocation(programId, "yFlip");

	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform1f(yFlipUniformLocation, 1.0f);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

	dominatingColor.g = 0.0f;
	dominatingColor.b = 1.0f;
	glUniform3fv(dominatingColorUniformLocation, 1, &dominatingColor[0]);
	glUniform1f(yFlipUniformLocation, -1.0f);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
}

void paintCube(GLfloat width, GLfloat height) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//mat4 projectionMatrix = perspective(20.0f, (GLfloat)width() / height(), 0.1f, 10.0f);
	//mat4 transformMatrix = translate(mat4(), vec3(0.0f, 0.0f, -3.0f));
	//mat4 rotationMatrix = rotate(mat4(), 54.0f, vec3(1.0f, 0.0f, 0.0f));
	//mat4 matrix = projectionMatrix * transformMatrix * rotationMatrix;

	//GLint projectionMatrixUniformLocation = glGetUniformLocation(programId, "projectionMatrix");
	//GLint transformMatrixUniformLocation = glGetUniformLocation(programId, "transformMatrix");
	//GLint rotationMatrixUniformLocation = glGetUniformLocation(programId, "rotationMatrix");
	//glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	//glUniformMatrix4fv(transformMatrixUniformLocation, 1, GL_FALSE, &transformMatrix[0][0]);
	//glUniformMatrix4fv(rotationMatrixUniformLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
	//GLint matrixUniformLocation = glGetUniformLocation(programId, "matrix");
	//glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &matrix[0][0]);

	mat4 projectionMatrix = perspective(20.0f, width / height, 0.1f, 10.0f);
	mat4 projectionTransformMatrix = translate(projectionMatrix, vec3(0.0f, 0.0f, -3.0f));
	mat4 projectionTransformRotationMatrix = rotate(projectionTransformMatrix, 45.0f, vec3(1.0f, 0.0f, 0.0f));

	GLint matrixUniformLocation = glGetUniformLocation(programId, "matrix");
	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &projectionTransformRotationMatrix[0][0]);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
}

void paintInstanced(GLfloat width, GLfloat height) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	mat4 projectionMatrix = perspective(20.0f, width / height, 0.1f, 10.0f);
	mat4 matrix[] = {
		// ex6 camera
		projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(-1.0f, 0.0f, -3.0f)) * glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f)),
		projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(1.0f, 0.0f, -3.75f)) * glm::rotate(125.0f, vec3(0.0f, 1.0f, 0.0f)),
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(matrix), matrix, GL_DYNAMIC_DRAW);
	glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0, 2);
}

void paintDifferentInstances(GLfloat width, GLfloat height) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 projectionMatrix = perspective(20.0f, width / height, 0.1f, 10.0f);
	mat4 matrix;
	GLint matrixUniformLocation = glGetUniformLocation(programId, "matrix");

	glBindVertexArray(cubeVertexArrayObjectId);
	matrix = projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(-3.5f, 0.0f, -3.0f)) * glm::rotate(45.0f, vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &matrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, 0);

	matrix = projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(3.5f, 0.0f, -3.75f)) * glm::rotate(125.0f, vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &matrix[0][0]);
	glDrawElements(GL_TRIANGLES, cubeNumIndices, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(arrowVertexArrayObjectId);
	matrix = projectionMatrix * camera.getWorldToViewMatrix() * glm::translate(vec3(0.0f, 0.0f, -2.5f)) * glm::rotate(125.0f, vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(matrixUniformLocation, 1, GL_FALSE, &matrix[0][0]);
	glDrawElements(GL_TRIANGLES, arrowNumIndices, GL_UNSIGNED_SHORT, (void*)arrowIndexDataByteOffset);
}

void MeGlWindow::paintGL() {
	glClearColor(0, 0, 0, 1);
	glViewport(0, 0, width(), height());

	// ex1 depth buffer
	//paintDepthBuffer();

	// ex2 color buffer
	//paintColorBuffer();

	// ex3 uniform
	//paintUniform();

	// ex4 draw cube
	//paintCube(width(), height());

	// ex5 draw instance
	//paintInstanced(width(), height());

	// ex7 draw differnt instance
	paintDifferentInstances(width(), height());
}

void MeGlWindow::mouseMoveEvent(QMouseEvent* qMouseEvent) {
	camera.mouseUpdate(glm::vec2(qMouseEvent->x(), qMouseEvent->y()));
	repaint();
}

void MeGlWindow::keyPressEvent(QKeyEvent* qKeyEvent) {
	switch (qKeyEvent->key()) {
	case Qt::Key::Key_W:
	case Qt::Key::Key_Up:
		camera.moveUp();
		break;

	case Qt::Key::Key_S:
	case Qt::Key::Key_Down:
		camera.moveDown();
		break;

	case Qt::Key::Key_A:
	case Qt::Key::Key_Left:
		camera.strafeLeft();
		break;

	case Qt::Key::Key_D:
	case Qt::Key::Key_Right:
		camera.strafeRight();
		break;

	case Qt::Key::Key_E:
		camera.moveForward();
		break;

	case Qt::Key::Key_F:
		camera.moveBackward();
		break;
	}
	repaint();
}