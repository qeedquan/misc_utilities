#ifndef WIDGET_H
#define WIDGET_H

#include <GL/glew.h>

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>

#include <iostream>

class Widget : public QOpenGLWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

private:
	QOpenGLShaderProgram program;
};

#endif // WIDGET_H
