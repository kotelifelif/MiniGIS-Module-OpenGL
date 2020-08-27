#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	// Constructor with scalar values
	Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch);

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 get_view_matrix();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void process_keyboard(Camera_Movement direction, float delta_time);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void process_mouse_scroll(float y_offset);

	glm::vec3 get_position() const;
	glm::vec3 get_front() const;
	glm::vec3 get_up() const;
	glm::vec3 get_right() const;
	glm::vec3 get_world_up() const;
	float get_yaw() const;
	float get_pitch() const;
	float get_movement_speed() const;
	float get_mouse_sensitivity() const;
	float get_zoom() const;

private:
	// Camera Attributes
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	// Euler Angles
	float yaw;
	float pitch;
	// Camera options
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void _update_camera_vectors();
};


