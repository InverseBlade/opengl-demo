#include "glutil.h"
#include <GLFW/glfw3.h>
#include <conio.h>
#include <glad/glad.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "stb_image.h"
#include "linmath.h"

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
float xoffset = .0f, yoffset = 0.f, zoffset = 0.f, scale = 0.5f;
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
    //Load Texture
    //Generate Texture
    GLuint texture1 = loadTexture("textures/face.png", GL_RGBA);
    GLuint texture2 = loadTexture("textures/bricks.jpg", GL_RGB);
    // Vertext Data & Create Buffers
    float vertices[][8] = {
            //front
            //coordinate         //color           //texture-coordinates
            {-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.f, 0.f}, //0 bottom-left
            {-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.f, 1.f}, //1 top-left
            {1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.f, 0.f}, //2 bottom-right
            {1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.f, 1.f}, //3 top-right
            //back
            //coordinate         //color           //texture-coordinates
            {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f}, //4 bottom-left
            {-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 1.f}, //5 top-left
            {1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f}, //6 bottom-right
            {1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 1.f}, //7 top-right
            // left
            //coordinate         //color           //texture-coordinates
            {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f}, //8 bottom-left
            {-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.f, 1.f}, //9 top-left
            {-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f}, //10 bottom-right
            {-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.f, 1.f}, //11 top-right
            // right
            //coordinate         //color           //texture-coordinates
            {1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f}, //12 bottom-left
            {1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.f, 1.f}, //13 top-left
            {1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f}, //14 bottom-right
            {1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.f, 1.f}, //15 top-right
            // top
            //coordinate         //color           //texture-coordinates
            {-1.0f, 1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f}, //16 bottom-left
            {-1.0f, 1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 0.f, 1.f}, //17 top-left
            {1.0f,  1.0f,  -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f}, //18 bottom-right
            {1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 1.0f, 1.f, 1.f}, //19 top-right
            // front
            //coordinate         //color           //texture-coordinates
            {-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.f, 0.f}, //20 bottom-left
            {-1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.f, 1.f}, //21 top-left
            {1.0f,  -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.f, 0.f}, //22 bottom-right
            {1.0f,  -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.f, 1.f}, //23 top-right
    };
    GLuint indices[] = {
            //face1
            0, 1, 2,
            3, 2, 1,
            //face2
            4, 5, 6,
            5, 6, 7,
            //face3
            8, 9, 10,
            9, 10, 11,
            //face4
            12, 13, 14,
            13, 14, 15,
            //face5
            16, 17, 18,
            17, 18, 19,
            //face6
            20, 21, 22,
            21, 22, 23
    };
    unsigned int vex_array_col = sizeof(vertices[0]) / sizeof(float),
            vex_array_row = sizeof(vertices) / sizeof(float) / vex_array_col;
    printf("%d\n", sizeof(indices));
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
                 vex_array_row * vex_array_col * sizeof(float),
                 vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices) / sizeof(float) * sizeof(float),
                 indices, GL_STATIC_DRAW);
    // Assign Data To Vertex Attributes
    // vertex position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          vex_array_col * sizeof(float),
                          (void *) (sizeof(float) * 0));
    glEnableVertexAttribArray(1);
    // vertex color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
                          vex_array_col * sizeof(float),
                          (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    //texture coord
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE,
                          vex_array_col * sizeof(float),
                          (void *) (sizeof(float) * 6));
    glEnableVertexAttribArray(3);
    // Unbind VAO
    glBindVertexArray(0);
    //Use ShaderProgram
    glUseProgram(ShaderProgram);
    //texture & sampler
    glUniform1i(glGetUniformLocation(ShaderProgram, "myTexture1"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram, "myTexture2"), 1);
    // Get Uniforms Location
    int matrixLoc = glGetUniformLocation(ShaderProgram, "MVP");
    //Render Mode
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
    int flag = 1;
    // Loop
    while (!glfwWindowShouldClose(window)) {
        // input
        processInput(window);
        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use Program & Draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //Transform
        mat4x4 mvp;
        mat4x4 model, view, projection;
        //Model Matrix
        mat4x4_translate(model, xoffset, yoffset, zoffset);
        mat4x4_rotate_X(model, model, xangle);
        mat4x4_rotate_Y(model, model, yangle);
        mat4x4_rotate_Z(model, model, zangle);
        mat4x4_scale_aniso(model, model, scale, scale, scale);
        //View Matrix
        mat4x4_translate(view, 0.f, 0.f, -3.0f);
        //Projection Matrix
        float ratio = 1.0f / 1.0f;
        mat4x4_perspective(projection, 45.f / 180 * PI, ratio, 0.1f, 100.0f);
        //MVP
        mat4x4_mul(mvp, projection, view);
        mat4x4_mul(mvp, mvp, model);
        glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, (const float *) mvp);
        //Vertex Array
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES,
                       sizeof(indices) / sizeof(float),
                       GL_UNSIGNED_INT, (void *) 0);
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
    static float delta1 = 0.001f, delta2 = 1e-3f;
    double t = glfwGetTime();
    const float ratio = 0.025f;
    //
    if (t - lastTime < 0.005) {
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        zangle += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        zangle -= ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        xoffset -= ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        xoffset += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        yoffset += ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        yoffset -= ratio;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        scale += delta1;
        delta1 *= 1.01f;
    } else {
        delta1 = 1e-3f;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        scale -= delta2;
        delta2 *= 1.01f;
    } else {
        delta2 = 1e-3f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        zoffset += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        zoffset -= 0.1f;
    }
    scale = scale <= 1e-2f ? 1e-2f : scale;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    int wid, hgt;
    hgt = wid = width < height ? width : height;
    glViewport(0, 0, wid, hgt);
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

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
    //set keypress callback

    return window;
}
