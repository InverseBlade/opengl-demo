#include "glutil.h"
#include <GLFW/glfw3.h>
#include <conio.h>
#include <glad/glad.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define RENDER_WID 640
#define RENDER_HGT 640
#define WIN_TITLE "旋转方块"

#define PI (3.1415926f)
#define MODE_LINE 1
#define MODE_FILL 2

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int doLoop(GLFWwindow *window);

GLFWwindow *initWindow(void);

void processInput(GLFWwindow *window);

float xangle = .0f, yangle = .0f, zangle = .0f;
int mode = MODE_LINE;

int main(int argc, char const *argv[]) {
    // TO-DO
    GLFWwindow *window = initWindow();
    // process
    if (argc > 1) {
        mode = atoi(argv[1]);
    }
    // do the Game Loop
    doLoop(window);
    // Release Resources
    glfwTerminate();
    return 0;
}

/**
 * Core Program Loop
 */
int doLoop(GLFWwindow *window) {
    //-------------------Create Shaders------------------------//
    GLuint ShaderProgram = loadProgram(
            "shaders/VertexShaderSrc.vs",
            "shaders/FragmentShaderSrc.fs");
    //---------------------------------------------------------//
    // Vertext Data & Create Buffers
    float vertices[][6] = {
            //coordinate r,g,b
            //front
            {-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f}, //0 bottom-left
            {-0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 1.0f}, //1 top-left
            {0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, 1.0f}, //2 bottom-right
            {0.5f,  0.5f,  0.0f, 1.f,  1.f,  1.f}, //3 top-right
            //back
            {-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f}, //4 //bottom-left
            {-0.5f, 0.5f,  1.0f, 1.0f, 1.0f, 1.0f}, //5 //top-left
            {0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, 1.0f}, //6 //bottom-right
            {0.5f,  0.5f,  1.0f, 1.f,  1.f,  1.f}, //7 top-right
    };
    GLuint indices[] = {
            //face1
            0, 1, 2,
            3, 2, 1,
            //face2
            3, 2, 6,
            3, 6, 7,
            //face3
            5, 7, 6,
            4, 5, 6,
            //face4
            1, 4, 5,
            1, 4, 0,
            //face5
            2, 4, 6,
            2, 0, 4,
            //face6
            5, 3, 7,
            5, 1, 3,
    };
    // Vertex Array Object, Vertex Buffer Object, Element Buffer Object
    GLuint VAO, VBO, EBO;
    // Generate Objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind Objects
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Copy Data to Video Card Memory
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices) * sizeof(vertices[0]) * sizeof(float), vertices,
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * sizeof(float),
                 indices, GL_STATIC_DRAW);
    // Assign Data To Vertex Attributes
    // vertex position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *) (sizeof(float) * 0));
    glEnableVertexAttribArray(1);
    // vertex color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    // Unbind VAO
    glBindVertexArray(0);
    // Get Uniforms Location
    GLint xAngleLoc = glGetUniformLocation(ShaderProgram, "xangle");
    GLint yAngleLoc = glGetUniformLocation(ShaderProgram, "yangle");
    GLint zAngleLoc = glGetUniformLocation(ShaderProgram, "zangle");

    switch (mode) {
        case MODE_LINE:
            mode = GL_LINE_LOOP;
            break;
        case MODE_FILL:
            mode = GL_TRIANGLES;
            break;
        default:
            mode = GL_TRIANGLES;
    }
    // Loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // Render
        glClearColor(.0f, .0f, .0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Use Program & Draw
        glUseProgram(ShaderProgram);
        glBindVertexArray(VAO);
        glUniform1f(xAngleLoc, xangle);
        glUniform1f(yAngleLoc, yangle);
        glUniform1f(zAngleLoc, zangle);
        glPolygonMode(GL_FRONT, GL_FILL);
        glDrawElements(mode, (int) sizeof(indices), GL_UNSIGNED_INT, (void *) 0);

        // check events
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return 0;
}

/**
 * Process user key inputs
 */
void processInput(GLFWwindow *window) {
    static double lastTime = 0;
    double t = glfwGetTime();
    const float ratio = 0.05f;
    //
    if (t - lastTime < 0.01) {
        return;
    }
    lastTime = t;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        yangle += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        yangle -= ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        xangle += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        xangle -= ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        zangle += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        zangle -= ratio;
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    int wid, hgt;
    hgt = wid = width < height ? width : height;
    glViewport(0, 0, wid, hgt);
    return;
}

/**
 * Initialize GLFW
 */
GLFWwindow *initWindow(void) {
    if (glfwInit() == GLFW_FALSE) {
        printf("Program init failed!\n");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    // Create window
    GLFWwindow *window = glfwCreateWindow(RENDER_WID, RENDER_HGT, WIN_TITLE, NULL, NULL);
    if (window == NULL) {
        printf("Failed to create window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // Initialize Glad Function Pointer
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("failed to init glad.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glViewport(0, 0, RENDER_WID, RENDER_HGT);
    // set resize callback func
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}
