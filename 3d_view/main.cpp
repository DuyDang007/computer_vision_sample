#define GLFW_DLL

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"

using namespace std;

#define VIEW_WIDTH 640
#define VIEW_HEIGHT 480

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void getInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        printf("L");
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        printf("R");
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

char *readFromFile(const char *file_name)
{
    // Read text from file to buffer
    size_t filesize;
    ifstream file_stream;
    file_stream.open(file_name, ios::in);
    file_stream.seekg(0, file_stream.end);
    filesize = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);
    
    char *str_out = new char[filesize+1];
    memset(str_out, 0, filesize+1);
    file_stream.read(str_out, filesize);

    file_stream.close();
    return str_out;
}

int main(int argc, char** argv)
{
    GLboolean ret;
    ret = glfwInit();
    if (ret == GL_FALSE)
    {
        printf("GL Init failed: %d\n", ret);
        return ret;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWwindow *window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "Sample window", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to load GLAD: %d\n");
        return -1;
    }

    // Set view port
    glViewport(0, 0, VIEW_WIDTH, VIEW_HEIGHT);
    // Callback function on window size change
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Read vertices from file
    size_t vfilesize;
    ifstream vertex_file;
    vertex_file.open(argv[1], ios::in);
    vertex_file.seekg(0, vertex_file.end);
    vfilesize = vertex_file.tellg();
    vertex_file.seekg(0, vertex_file.beg);
    unsigned int vertices_num = (unsigned int)(vfilesize / (6 * sizeof(float)));

    float *vertices = new float[vertices_num * 6];
    vertex_file.read((char*)vertices, vfilesize);

    vertex_file.close();

    // Generate VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertex_buffer_obj_p;
    glGenBuffers(1, &vertex_buffer_obj_p);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_obj_p);
    glBufferData(GL_ARRAY_BUFFER, vertices_num * 6, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Shader processing
    int success;
    char infoLog[512];

    // Read vertex shader from file
    const char *vertex_shader_source = readFromFile("vertex_shader.txt");

    // Create vertex shader
    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // Read geometry shader from file
    const char *geo_shader_source = readFromFile("geometry_shader.txt");

    // Create geometry shader
    GLuint geo_shader;
    geo_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geo_shader, 1, &geo_shader_source, NULL);
    glCompileShader(geo_shader);

    glGetShaderiv(geo_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geo_shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::GEO::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // Read fragment shader
    const char *fragment_shader_source = readFromFile("fragment_shader.txt");

    // Create fragment shader
    GLuint fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // Create shader program and link
    GLuint shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, geo_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader_program, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
             << infoLog << endl;
    }

    glUseProgram(shader_program);

    // Cleanup shader objects
    // free(vertex_shader_str);
    // free(fragment_shader_str);
    glDeleteShader(vertex_shader);
    glDeleteShader(geo_shader);
    glDeleteShader(fragment_shader);

    /* -------------------------------- Main render ------------------------------*/
    while (!glfwWindowShouldClose(window))
    {
        getInput(window);

        glClearColor(0.9, 0.8, 0.85, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vertices_num);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}