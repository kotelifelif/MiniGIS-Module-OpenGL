#include "Shader.h"

using namespace std;


// constructor generates the shader on the fly
Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path) :
	attributes(attributes)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertex_code;
	std::string fragment_code;
	std::string geometry_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;
	std::ifstream g_shader_file;
	// ensure ifstream objects can throw exceptions:
	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;
		// read file's buffer contents into streams
		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();
		// close file handlers
		v_shader_file.close();
		f_shader_file.close();
		// convert stream into string
		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometry_path != nullptr)
		{
			g_shader_file.open(geometry_path);
			std::stringstream g_shader_stream;
			g_shader_stream << g_shader_file.rdbuf();
			g_shader_file.close();
			geometry_code = g_shader_stream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* v_shader_code = vertex_code.c_str();
	const char* f_shader_code = fragment_code.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	_check_compile_errors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	_check_compile_errors(fragment, "FRAGMENT");
	// if geometry shader is given, compile geometry shader
	unsigned int geometry;
	if (geometry_path != nullptr)
	{
		const char* g_shader_code = geometry_code.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &g_shader_code, NULL);
		glCompileShader(geometry);
		_check_compile_errors(geometry, "GEOMETRY");
	}
	// shader program
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometry_path != nullptr)
	{
		glAttachShader(program, geometry);
	}

	for (int i = 0; i < attributes.size(); ++i)
	{
		glBindAttribLocation(program, i, attributes[i]);
	}

	glLinkProgram(program);
	_check_compile_errors(program, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometry_path != nullptr)
	{
		glDeleteShader(geometry);
	}
}

// activate the shader
void Shader::use()
{
	glUseProgram(program);
}

// utility uniform functions
void Shader::set_bool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}


void Shader::set_int(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}


void Shader::set_float(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}


void Shader::set_vec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(program, name.c_str()), x, y);
}


void Shader::set_vec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z);
}


void Shader::set_vec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w);
}


void Shader::set_mat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::set_mat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::set_mat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


void Shader::_check_compile_errors(GLuint shader, std::string type)
{
	GLint success;
	GLchar info_log[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

unsigned int Shader::get_program() const
{
	return program;
}