#pragma once


#include <soil.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Advancing_front_surface_reconstruction.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Shader.h"
#include "Camera.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <windows.h>
#include <Commdlg.h>

#undef max();
#undef min();


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Advancing_front_surface_reconstruction<> Reconstruction;
typedef Reconstruction::Triangulation_3 Triangulation_3;
typedef Reconstruction::Triangulation_data_structure_2 TDS_2;
typedef K::Point_3 Point_3;
typedef K::Vector_3 Vector_3;


using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
unsigned int load_texture(const char* path);
vector<float> read_from_file();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// lighting
glm::vec3 light_pos(0.0f, 0.0f, 3.0f);

extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

float lr_angle = 360.0f;
float ud_angle = 360.0f;
bool lr_direction = true;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if (error != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glewInit();


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    Shader lighting_shader("colors_vs.glsl", "colors_fs.glsl"); 

    vector<float> vertices = read_from_file();

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cube_VAO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(cube_VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        // input
        process_input(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lighting_shader.use();
        lighting_shader.set_vec3("objectColor", 1.0f, 0.5f, 0.31f);
        lighting_shader.set_vec3("lightColor", 1.0f, 1.0f, 1.0f);
        lighting_shader.set_vec3("lightPos", light_pos);
        lighting_shader.set_vec3("viewPos", camera.get_position());

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.get_zoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.get_view_matrix();
        lighting_shader.set_mat4("projection", projection);
        lighting_shader.set_mat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, lr_angle, glm::vec3(0, 1, 0));
        model = glm::rotate(model, ud_angle, glm::vec3(1, 0, 0));
        lighting_shader.set_mat4("model", model);

        // render the cube
        glBindVertexArray(cube_VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        lr_angle += 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        lr_angle -= 0.01;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        ud_angle += 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        ud_angle -= 0.01;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.process_keyboard(Camera_Movement::FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.process_keyboard(Camera_Movement::BACKWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.process_keyboard(Camera_Movement::LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.process_keyboard(Camera_Movement::RIGHT, delta_time);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    if (first_mouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

    last_x = x_pos;
    last_y = y_pos;

    camera.process_mouse_movement(x_offset, y_offset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    camera.process_mouse_scroll(y_offset);
}

// utility function for loading a 2D texture from file
unsigned int load_texture(char const* path)
{
    unsigned int texture_ID;
    glGenTextures(1, &texture_ID);

    int width, height, nr_components;
    unsigned char* data = SOIL_load_image(path, &width, &height, &nr_components, 0);
    if (data)
    {
        GLenum format;
        if (nr_components == 1)
        {
            format = GL_RED;
        }
        else if (nr_components == 3)
        {
            format = GL_RGB;
        }
        else if (nr_components == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        SOIL_free_image_data(data);
    }

    return texture_ID;
}

vector<float> read_from_file()
{
    // Open file
    wchar_t file_size[1024];
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = file_size;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(file_size);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileName(&ofn);

    // mesh generation
    // https://cgal.geometryfactory.com/CGAL/doc/master/Advancing_front_surface_reconstruction/index.html#Chapter_Advancing_Front_Surface_Reconstruction
    // File Advancing_front_surface_reconstruction/reconstruction_class.cpp
    vector<float> vertices;
    float max_x = 0, max_y = 0, max_z = 0;
    float min_x = 1e9, min_y = 1e9, min_z = 1e9;
    vertices.clear();
    std::ifstream in(ofn.lpstrFile);
    std::istream_iterator<Point_3> begin(in);
    std::istream_iterator<Point_3> end;
    Triangulation_3 dt(begin, end);
    Reconstruction reconstruction(dt);
    reconstruction.run();
    const TDS_2& tds = reconstruction.triangulation_data_structure_2();
    std::cout << "solid produced with CGAL::Advancing_front_surface_reconstruction\n";
    int count = 0;
    for (TDS_2::Face_iterator fit = tds.faces_begin();
        fit != tds.faces_end();
        ++fit) {
        if (reconstruction.has_on_surface(fit)) {
            Triangulation_3::Facet f = fit->facet();
            Triangulation_3::Cell_handle ch = f.first;
            int ci = f.second;
            Point_3 points[3];
            for (int i = 0, j = 0; i < 4; i++) {
                if (ci != i) {
                    points[j] = ch->vertex(i)->point();
                    j++;
                }
            }
            // calculating normal
            glm::vec3 v1(points[0].x(), points[0].y(), points[0].z());
            glm::vec3 v2(points[1].x(), points[1].y(), points[1].z());
            glm::vec3 v3(points[2].x(), points[2].y(), points[2].z());
            glm::vec3 f1 = v2 - v1;
            glm::vec3 f2 = v3 - v1;
            glm::vec3 n = glm::cross(f1, f2);
            n = glm::normalize(n);

            for (int i = 0; i < 3; ++i)
            {
                vertices.push_back(points[i].x());
                if (vertices[count] > max_x) max_x = vertices[count];
                if (vertices[count] < min_x) min_x = vertices[count];
                count++;
                vertices.push_back(points[i].y());
                if (vertices[count] > max_y) max_y = vertices[count];
                if (vertices[count] < min_y) min_y = vertices[count];
                count++;
                vertices.push_back(points[i].z());
                if (vertices[count] > max_z) max_z = vertices[count];
                if (vertices[count] < min_z) min_z = vertices[count];
                count++;
                vertices.push_back(n.x);
                vertices.push_back(n.y);
                vertices.push_back(n.z);
                count += 3;
            }

        }
    }

    int a = -1, b = 1;
    for (int i = 0; i < vertices.size(); i += 3)
    {
        vertices[i] = (vertices[i] - min_x) * (b - a) / (max_x - min_x) + a;
        vertices[i + 1] = (vertices[i + 1] - min_y) * (b - a) / (max_y - min_y) + a;
        vertices[i + 2] = (vertices[i + 2] - min_z) * (b - a) / (max_z - min_z) + a;
    }
    return vertices;
}
