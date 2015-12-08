#pragma once

#include <QtOpenGL\qglwidget>

class MeGlWindow : public QGLWidget {
public:
	~MeGlWindow();
protected:
	void initializeGL();
	void paintGL();
};