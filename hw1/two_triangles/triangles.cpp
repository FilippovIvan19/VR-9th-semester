// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "HW1 triangles", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programIDred = LoadShaders("TransformVertexShader_red.vertexshader", "ColorFragmentShader_both.fragmentshader");
	GLuint programIDyellow = LoadShaders("TransformVertexShader_yellow.vertexshader", "ColorFragmentShader_both.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixIDred = glGetUniformLocation(programIDred, "MVP");
	GLuint MatrixIDyellow = glGetUniformLocation(programIDyellow, "MVP");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP; // Remember, matrix multiplication is the other way around

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	static const GLfloat g_vertex_buffer_red_data[] = {
		-1.0f, -0.5f, 0.0f,
		1.0f, 0.5f, 1.0f,
		1.0f, 0.5f, -1.0f,
	};
	static const GLfloat g_vertex_buffer_yellow_data[] = {
		1.0f, -0.5f, 0.0f,
		-1.0f, 0.5f, 1.0f,
		-1.0f, 0.5f, -1.0f,
	};

	// One color for each vertex.
	static const GLfloat g_color_buffer_red_data[] = {
		1.0f, 0.0f, 0.0f, 0.4f,
		1.0f, 0.0f, 0.0f, 0.4f,
		1.0f, 0.0f, 0.0f, 0.4f,
	};
	static const GLfloat g_color_buffer_yellow_data[] = {
		1.0f, 1.0f, 0.0f, 0.7f,
		1.0f, 1.0f, 0.0f, 0.7f,
		1.0f, 1.0f, 0.0f, 0.7f,
	};


	GLuint vertexbuffer_red;
	glGenBuffers(1, &vertexbuffer_red);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_red);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_red_data), g_vertex_buffer_red_data, GL_STATIC_DRAW);

	GLuint vertexbuffer_yellow;
	glGenBuffers(1, &vertexbuffer_yellow);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_yellow);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_yellow_data), g_vertex_buffer_yellow_data, GL_STATIC_DRAW);

	GLuint colorbuffer_red;
	glGenBuffers(1, &colorbuffer_red);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_red);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_red_data), g_color_buffer_red_data, GL_STATIC_DRAW);

	GLuint colorbuffer_yellow;
	glGenBuffers(1, &colorbuffer_yellow);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_yellow);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_yellow_data), g_color_buffer_yellow_data, GL_STATIC_DRAW);

	double t = 0.0f;
	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		t += 0.001;
		glm::mat4 View = glm::lookAt(
			glm::vec3(3 * sin(t), 1, 3 * cos(t)), // Camera in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		MVP = Projection * View * Model;


		// Use our shader
		glUseProgram(programIDred);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixIDred, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_red);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_red);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3 * 1); // 3*n indices starting at 0 -> n triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		glUseProgram(programIDyellow);
		glUniformMatrix4fv(MatrixIDyellow, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_yellow);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer_yellow);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3 * 1); // 3*n indices starting at 0 -> n triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer_red);
	glDeleteBuffers(1, &colorbuffer_red);
	glDeleteBuffers(1, &vertexbuffer_yellow);
	glDeleteBuffers(1, &colorbuffer_yellow);
	glDeleteProgram(programIDred);
	glDeleteProgram(programIDyellow);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

