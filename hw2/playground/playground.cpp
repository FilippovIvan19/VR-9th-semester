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
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <vector>
#include <random>

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>
#include "utils/figures.hpp"
#include "utils/controls.hpp"
#include "utils/init.hpp"


class Object_3d {
public:
	static float const speed;

	vec3 coordinates;
	vec3 direction;
	mat4 rotation;

	Object_3d(vec3 coordinates, vec3 direction, mat4 rotation) {
		this->coordinates = coordinates;
		this->direction = direction;
		this->rotation = rotation;
	}

	void move(float deltaTime) {
		coordinates += direction * deltaTime * speed;
	}
};

const float Object_3d::speed = 5.0f;


template <typename T>
GLuint load_buffer(int buffer_size, T* buffer_data) {
	GLuint buffer_id;
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, buffer_data, GL_STATIC_DRAW);
	return buffer_id;
}


void draw_object(
	GLuint vertexbuffer, GLuint colorbuffer, GLuint MatrixID, int polygon_count,
	Object_3d& obj, mat4& View, mat4& Projection, int color_size
) {
	mat4 Translation = translate(obj.coordinates);
	mat4 Scaling = mat4(1.0f);
	mat4 Model = Translation * obj.rotation * Scaling;
	mat4 MVP = Projection * View * Model;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
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
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		color_size,                       // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3 * polygon_count); // 3*n indices starting at 0 -> n triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void draw_all_enemies(
	GLuint vertexbuffer, GLuint colorbuffer, GLuint MatrixID,
	std::vector<Object_3d>& enemies, mat4& View, mat4& Projection
) {
	static int polygon_count = get_oct_vertex_size() / 3 / 3 / sizeof(GLfloat);
	int length = enemies.size();
	for (int i = 0; i < length; i++) {
		draw_object(
			vertexbuffer, colorbuffer, MatrixID, polygon_count,
			enemies[i], View, Projection, 4
		);
	}
}

void draw_all_fireballs(
	GLuint vertexbuffer, GLuint colorbuffer, GLuint MatrixID,
	std::vector<Object_3d>& fireballs, mat4& View, mat4& Projection,
	int polygon_c, GLuint Texture, GLuint TextureID
) {
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	static int polygon_count = polygon_c;
	int length = fireballs.size();
	for (int i = 0; i < length; i++) {
		fireballs[i].move(currentTime - lastTime);
		draw_object(
			vertexbuffer, colorbuffer, MatrixID, polygon_count,
			fireballs[i], View, Projection, 2
		);
	}
	lastTime = currentTime;
}


float get_random_float(float start, float end) {
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_real_distribution<> dist(start, end);
	return dist(gen);
}

vec3 get_random_direction() {
	float verticalAngle = get_random_float(0, 2*std::_Pi);
	float horizontalAngle = get_random_float(0, 2*std::_Pi);
	return vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
}


void create_enemy_by_timer(std::vector<Object_3d>& enemies) {
	static double lastTime = glfwGetTime();
	double currentTime = glfwGetTime();

	if (currentTime - lastTime > 3) {
		//printf("create new enemy\n");
		vec3 new_coord = getCameraPosition() + get_random_direction()*get_random_float(2, 20);
		vec3 new_direction = vec3();
		mat4 new_rot = rotate(get_random_float(0, 2*std::_Pi), get_random_direction());

		enemies.push_back(Object_3d(new_coord, new_direction, new_rot));
		lastTime = currentTime;
		//printf("enemy count = %d\n", coords.size());
	}
}

void create_fireball_by_click(std::vector<Object_3d>& fireballs) {
	static int prev_state = GLFW_RELEASE;

	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state != prev_state) {
		prev_state = state;

		if (state == GLFW_PRESS) {
			//printf("create new fireball\n");
			vec3 new_direction = getCameraDirection();
			vec3 new_coord = getCameraPosition() + new_direction * 3.0f;

			vec3 yAxis(0, 1, 0);
			vec3 rotationAxis = cross(yAxis, new_direction);
			float rotationAngle = acos(dot(yAxis, new_direction));
			mat4 new_rot = rotate(rotationAngle, rotationAxis);

			fireballs.push_back(Object_3d(new_coord, new_direction, new_rot));
			//printf("fireball count = %d\n", coords.size());
		}
	}
}

void delete_collided(std::vector<Object_3d>& enemies, std::vector<Object_3d>& fireballs) {
	for (int i = 0; i < enemies.size();) {
		bool inner_breaked = false;
		for (int j = 0; j < fireballs.size();) {
			if (glm::length(enemies[i].coordinates - fireballs[j].coordinates) < 1.5) {
				enemies.erase(enemies.begin() + i);
				fireballs.erase(fireballs.begin() + j);
				inner_breaked = true;
				break;
			} else {
				j++;
			}
		}
		if (!inner_breaked) {
			i++;
		}
	}
}


int main() {
	int init_res = init_all();
	if (init_res != 0)
		return init_res;


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	// Create and compile our GLSL program from the shaders
	GLuint programIDhardcoded = LoadShaders("TransformVertexShader_forHardcoded.vertexshader",
		"ColorFragmentShader_forHardcoded.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixIDhardcoded = glGetUniformLocation(programIDhardcoded, "MVP");

	// Create and compile our GLSL program from the shaders
	GLuint programIDobj = LoadShaders("TransformVertexShader_obj.vertexshader",
		"TextureFragmentShader_obj.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixIDobj = glGetUniformLocation(programIDobj, "MVP");

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(programIDobj, "myTextureSampler");


	// Load the texture
	GLuint FireballTexture = loadDDS("fireball.DDS");

	// Read our .obj file
	std::vector<glm::vec3> fireball_vertices;
	std::vector<glm::vec2> fireball_uvs;
	std::vector<glm::vec3> fireball_normals; // Won't be used at the moment.
	bool load_res = loadOBJ("fireball.obj", fireball_vertices, fireball_uvs, fireball_normals);
	if (!load_res)
		return 1;

	GLuint enemy_vertex_buffer = load_buffer(get_oct_vertex_size(), get_oct_vertex());
	GLuint enemy_color_buffer = load_buffer(get_oct_color_size(), get_oct_color());

	GLuint fireball_vertex_buffer = load_buffer(fireball_vertices.size() * sizeof(glm::vec3), &fireball_vertices[0]);
	GLuint fireball_uv_buffer = load_buffer(fireball_uvs.size() * sizeof(glm::vec2), &fireball_uvs[0]);


	std::vector<Object_3d> enemies;
	std::vector<Object_3d> fireballs;

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear the screen

		// Вычислить MVP-матрицу в зависимости от положения мыши и нажатых клавиш
		computeMatricesFromInputs();
		glm::mat4 View = getViewMatrix();

		create_enemy_by_timer(enemies);
		create_fireball_by_click(fireballs);
		delete_collided(enemies, fireballs);

		glUseProgram(programIDhardcoded);
		draw_all_enemies(
			enemy_vertex_buffer, enemy_color_buffer, MatrixIDhardcoded,
			enemies, View, Projection
		);

		glUseProgram(programIDobj);
		draw_all_fireballs(
			fireball_vertex_buffer, fireball_uv_buffer, MatrixIDobj,
			fireballs, View, Projection, fireball_vertices.size() / 3,
			FireballTexture, TextureID
		);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	// Cleanup VBO and shader
	glDeleteBuffers(1, &enemy_vertex_buffer);
	glDeleteBuffers(1, &enemy_color_buffer);
	glDeleteProgram(programIDhardcoded);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &fireball_vertex_buffer);
	glDeleteBuffers(1, &fireball_uv_buffer);
	glDeleteProgram(programIDobj);
	glDeleteTextures(1, &FireballTexture);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	enemies.clear();
	fireballs.clear();

	return 0;
}
