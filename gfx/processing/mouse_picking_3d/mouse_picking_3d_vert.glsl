uniform mat4 transform;

attribute vec4 position;
attribute vec4 color;

varying vec4 ex_color;

void main() {
	gl_Position = transform * position;
	ex_color = color;
}

