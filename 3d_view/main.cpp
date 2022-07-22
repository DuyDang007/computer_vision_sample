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

string readFromFile(const char *file_name)
{
    // Read text from file to buffer
    string str_out;
    ifstream file_stream;
    file_stream.open(file_name, ios::in);
    
    while (file_stream.good())
    {
        string line;
        getline(file_stream, line);
        str_out.append(line + "\n");
    }
    printf("read done\n");
    
    file_stream.close();
    return str_out;
}

int main()
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

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f};

    // Generate VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertex_buffer_obj_p;
    glGenBuffers(1, &vertex_buffer_obj_p);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_obj_p);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
    glEnableVertexAttribArray(0);

    // Shader processing
    int success;
    char infoLog[512];

    // Read vertex shader from file
    const char *vertex_shader_source = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

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

    // Read fragment shader
    const char *fragment_shader_source = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n";

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
    glDeleteShader(fragment_shader);

    // Drawing preparation
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    /* -------------------------------- Main render ------------------------------*/
    while (!glfwWindowShouldClose(window))
    {
        getInput(window);

        glClearColor(0.1, 0.1, 0.15, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}