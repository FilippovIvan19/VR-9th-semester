// Include GLEW
#include <GL/glew.h>


static const GLfloat vertex_buffer_octahedron_data[] = {
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 1.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 0.0f, -1.0f,
	-1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	-1.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, -1.0f, 0.0f
};

static const GLfloat color_buffer_octahedron_data[] = {
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
};


const GLfloat* get_oct_vertex() {
	return vertex_buffer_octahedron_data;
}

int get_oct_vertex_size() {
	return sizeof(vertex_buffer_octahedron_data);
}

const GLfloat* get_oct_color() {
	return color_buffer_octahedron_data;
}

int get_oct_color_size() {
	return sizeof(color_buffer_octahedron_data);
}
