#include "Camera.h"




// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : 
	front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
{
	this->position = position;
	this->world_up = up;
	this->yaw = yaw;
	this->pitch = pitch;
	_update_camera_vectors();
}

// Constructor with scalar values
Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
	front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = glm::vec3(pos_x, pos_y, pos_z);
	world_up = glm::vec3(up_x, up_y, up_z);
	this->yaw = yaw;
	this->pitch = pitch;
	_update_camera_vectors();
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::get_view_matrix()
{
	return glm::lookAt(position, position + front, up);
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::process_keyboard(Camera_Movement direction, float delta_time)
{
	float velocity = movement_speed * delta_time;
	if (direction == Camera_Movement::FORWARD)
	{
		position += front * velocity;
	}
	if (direction == Camera_Movement::BACKWARD)
	{
		position -= front * velocity;
	}
	if (direction == Camera_Movement::LEFT)
	{
		position -= right * velocity;
	}
	if (direction == Camera_Movement::RIGHT)
	{
		position += right * velocity;
	}
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::process_mouse_movement(float x_offset, float y_offset, GLboolean constrain_pitch)
{
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;
	yaw += x_offset;
	pitch += y_offset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrain_pitch)
	{
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}

	// Update front, right and Up Vectors using the updated Euler angles
	_update_camera_vectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::process_mouse_scroll(float y_offset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
	{
		zoom -= y_offset;
	}
	if (zoom <= 1.0f)
	{
		zoom = 1.0f;
	}
	if (zoom >= 45.0f)
	{
		zoom = 45.0f;
	}
}

glm::vec3 Camera::get_position() const
{
	return position;
}

glm::vec3 Camera::get_front() const
{
	return front;
}

glm::vec3 Camera::get_up() const
{
	return up;
}

glm::vec3 Camera::get_right() const
{
	return right;
}

glm::vec3 Camera::get_world_up() const
{
	return world_up;
}

float Camera::get_yaw() const
{
	return yaw;
}

float Camera::get_pitch() const
{
	return pitch;
}

float Camera::get_movement_speed() const
{
	return movement_speed;
}

float Camera::get_mouse_sensitivity() const
{
	return mouse_sensitivity;
}

float Camera::get_zoom() const
{
	return zoom;
}

void Camera::_update_camera_vectors()
{
	// Calculate the new front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	// Also re-calculate the right and Up vector
	right = glm::normalize(glm::cross(front, world_up));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}
