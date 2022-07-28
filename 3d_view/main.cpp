#define GLFW_DLL

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <math.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

#define VIEW_WIDTH 800
#define VIEW_HEIGHT 600

float movex, movey, movez;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void getInput(GLFWwindow *window)
{
    movex = 0;
    movey = 0;
    movez = 0;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        movex = 0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        movex = -0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        movey = 0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        movey = -0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movez = 0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movez = -0.02;
    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

char *readBinFromFile(const char *file_name)
{
    // Read text from file to buffer
    char *str_out;
    size_t filesize;
    ifstream file_stream;
    file_stream.open(file_name, ifstream::in);
    file_stream.seekg(0, file_stream.end);
    filesize = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);

    printf("Bin file size: %u\n", filesize);

    str_out = new char[filesize];
    file_stream.read(str_out, filesize);
    
    file_stream.close();
    return str_out;
}

char *readTextFromFile(const char *file_name)
{
    // Read text from file to buffer
    char *str_out;
    size_t filesize;
    ifstream file_stream;
    file_stream.open(file_name, ifstream::in);
    file_stream.seekg(0, file_stream.end);
    filesize = file_stream.tellg();
    file_stream.seekg(0, file_stream.beg);

    str_out = (char*) malloc(filesize+1);
    memset(str_out, 0, filesize+1);
    file_stream.read(str_out, filesize);
    
    file_stream.close();
    return str_out;
}

int main(int argc, char **argv)
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

    float *vertices = (float*)readBinFromFile(argv[1]);
    uint32_t vertices_num = (*(&vertices + 1) - vertices) / 6;

    // Generate VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertex_buffer_obj_p;
    glGenBuffers(1, &vertex_buffer_obj_p);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_obj_p);
    glBufferData(GL_ARRAY_BUFFER, 76800, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *) 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void *)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Shader processing
    int success;
    char infoLog[512];

    // Read vertex shader from file
    const char *vertex_shader_source = readTextFromFile("vertex_shader.glsl");

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

    // Read geometry shader
    const char *geo_shader_source = readTextFromFile("geometry_shader.glsl");

    // Create geometry shader
    GLuint geo_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geo_shader, 1, &geo_shader_source, NULL);
    glCompileShader(geo_shader);

    glGetShaderiv(geo_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geo_shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"
             << infoLog << endl;
    }

    // Read fragment shader
    const char *fragment_shader_source = readTextFromFile("fragment_shader.glsl");

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

    // Cleanup shader objects
    glDeleteShader(vertex_shader);
    glDeleteShader(geo_shader);
    glDeleteShader(fragment_shader);

    glEnable(GL_DEPTH_TEST);

    // Prepare transform matrices
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Camera initialize
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

    float camx = 0;
    float camy = 0;
    float camz = 0;
    /* -------------------------------- Main render ------------------------------*/
    while (!glfwWindowShouldClose(window))
    {
        getInput(window);

        glClearColor(0.8, 0.8, 0.85, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        // Camera
        // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        camx += movex;
        camy += movey;
        camz += movez;
        // view = glm::lookAt(cameraPos + glm::vec3(camx, camy, camz),    // Position
  		//                    cameraTarget + glm::vec3(camx, camy, 0.0f), // Target
  		//                    cameraUp);    // Vector up
        view = glm::translate(view, glm::vec3(movex, movey, -movez));

        int modelLoc = glGetUniformLocation(shader_program, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewMat = glGetUniformLocation(shader_program, "view");
        glUniformMatrix4fv(viewMat, 1, GL_FALSE, glm::value_ptr(view));
        int projMat = glGetUniformLocation(shader_program, "projection");
        glUniformMatrix4fv(projMat, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 76800);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}