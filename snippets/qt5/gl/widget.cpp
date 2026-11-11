#include "widget.h"

Widget::Widget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

Widget::~Widget()
{
}

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f,
	-1.0f,
	0.0f,
	1.0f,
	-1.0f,
	0.0f,
	0.0f,
	1.0f,
	0.0f,
};

static QString vertex_shader =
	"#version 330 core\n"
	"layout(location = 0) in vec3 vertexPosition_modelspace;"
	"void main(){"
	"gl_Position = vec4(vertexPosition_modelspace, 1);"
	"}";

static QString fragment_shader =
	"#version 330 core\n"
	"out vec3 color;"
	"void main(){"
	"color = vec3(0.4, 0.5, 0.6);"
	"}";

GLuint vertexbuffer;
GLuint VertexArrayID;

void Widget::initializeGL()
{
	makeCurrent();

	// To properly initialize all available OpenGL function pointers
	// and stops from segfaulting
	glewExperimental = true;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW\n";
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	std::cerr << "using OpenGL " << format().majorVersion() << "." << format().minorVersion() << "\n";

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	if (!program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader))
		std::cerr << "unable to compile vertex shader\n";

	if (!program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader))
		std::cerr << "unable to compile fragment shader\n";

	if (!program.link())
		std::cerr << "unable to link shader program\n";

	program.bind();
}

void Widget::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Widget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program.programId());
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void *)0 // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}
