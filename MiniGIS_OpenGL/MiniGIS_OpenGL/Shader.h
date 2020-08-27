#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int get_program() const;

	// constructor generates the shader on the fly
	Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
	
	// activate the shader
	void use();

	// utility uniform functions
	void set_bool(const std::string& name, bool value) const;

	void set_int(const std::string& name, int value) const;

	void set_float(const std::string& name, float value) const;

	void set_vec2(const std::string& name, const glm::vec2& value) const;

	void set_vec2(const std::string& name, float x, float y) const;

	void set_vec3(const std::string& name, const glm::vec3& value) const;

	void set_vec3(const std::string& name, float x, float y, float z) const;

	void set_vec4(const std::string& name, const glm::vec4& value) const;

	void set_vec4(const std::string& name, float x, float y, float z, float w);

	void set_mat2(const std::string& name, const glm::mat2& mat) const;

	void set_mat3(const std::string& name, const glm::mat3& mat) const;

	void set_mat4(const std::string& name, const glm::mat4& mat) const;


private:
	unsigned int program;
	// utility function for checking shader compilation/linking errors.
	std::vector<const char*> attributes;
	void _check_compile_errors(GLuint shader, std::string type);
};

