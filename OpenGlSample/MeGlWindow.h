#pragma once

#include <QtOpenGL\qglwidget>

class MeGlWindow : public QGLWidget {
protected:
	void initializeGL();
	void paintGL();
};