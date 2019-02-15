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
#include "main.h"

#define RENDER_WID 1280
#define RENDER_HGT 1280
#define WIN_TITLE "旋转方块——帧数:"

#define PI (3.14159f)
#define radius(angle) (PI * (angle) / 180.f)
#define MODE_LINE 1
#define MODE_FILL 2

#define CAMERA_SPEED 8

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int doLoop(GLFWwindow *window);

GLFWwindow *initWindow(void);

void processInput(GLFWwindow *window);

float xangle = .0f, yangle = .0f, zangle = .0f;
float xoffset = .0f, yoffset = 0.f, zoffset = 10.f, scale = 0.4f;
vec3 camera = {0.0f, -20.0f, 3.0f},
        direction = {0.0f, 0.0f, -1.0f},
        up = {0.0f, 1.0f, 0.0f};
float delta_time = 0.0f, last_frame = 0.0f;
float yaw = -90.f, pitch = .0f;
float last_x = 0, last_y = 0;

int screen_w = RENDER_WID, screen_h = RENDER_HGT;

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

void gen_sky_buffer(GLuint *array_buffer, GLuint *element_buffer) {

}

void render_sky(ShaderAttrib attrib, Player *player, GLuint array_buffer, GLuint element_buffer);

/**
 * Core Program Loop
 */
int doLoop(GLFWwindow *window) {
    //-------------------Create Shaders------------------------//
    GLuint ShaderProgram = loadProgram(
            "shaders/VertexShaderSrc.glsl",
            "shaders/FragmentShaderSrc.glsl");
    //---------------------------------------------------------//
    //Load Texture
    //Generate Texture
    GLuint texture1 = loadTexture("textures/face.png", GL_RGBA);
    GLuint texture2 = loadTexture("textures/container.jpg", GL_RGB);
    GLuint texture3 = loadTexture("textures/bricks.jpg", GL_RGB);
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
    struct {
        float x, y, z;
    } cube_pos[] = {
            {0.0f,  0.0f,  0.0f},
            {2.0f,  5.0f,  -15.0f},
            {-1.5f, -2.2f, -2.5f},
            {-3.8f, -2.0f, -12.3f},
            {2.4f,  -0.4f, -3.5f},
            {-1.7f, 3.0f,  -7.5f},
            {1.3f,  -2.0f, -2.5f},
            {1.5f,  2.0f,  -2.5f},
            {1.5f,  0.2f,  -1.5f},
            {-1.3f, 1.0f,  -1.5f},
    };
    last_frame = (float) glfwGetTime();
    // Loop
    while (!glfwWindowShouldClose(window)) {
        //delta time process
        float current_frame = (float) glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        //FPS
        int fps = (int) (1.0f / delta_time + 0.5f);
        char title[255];
        sprintf(title, "%s%d", WIN_TITLE, fps);
        glfwSetWindowTitle(window, title);
        // input
        processInput(window);
        // Render
        glClearColor(0.8627f, 0.8627f, 0.8627f, 1.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Use Program & Draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        //Vertex Array
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        //Transform
        mat4x4 mvp;
        mat4x4 model, view, projection;
        //View Matrix
//        mat4x4_translate(view, 0.f, 0.f, -5.0f);
        vec3 target;
        vec3_add(target, camera, direction);
        mat4x4_look_at(view, camera, target, up);
        //Projection Matrix
        float ratio = (float) screen_w / screen_h;
        mat4x4_perspective(projection, 45.f / 180 * PI, ratio, 0.1f, 1000.0f);
        //Draw Cubes
        mat4x4 temp;
        glUniform1i(glGetUniformLocation(ShaderProgram, "texType"), 1);
        for (int i = 0; i < 10; ++i) {
            mat4x4_identity(model);
            mat4x4_rotate_Y(model, model, current_frame);
            mat4x4_translate(temp, cube_pos[i].x, cube_pos[i].y, cube_pos[i].z);
            mat4x4_mul(model, model, temp);
            float angle = 20.0f * i;
            mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, angle);
            mat4x4_rotate_Y(model, model, current_frame);
            mat4x4_scale_aniso(model, model, scale, scale, scale);
            mat4x4_mul(mvp, projection, view);
            mat4x4_mul(mvp, mvp, model);
            glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, (const float *) mvp);
            glDrawElements(GL_TRIANGLES,
                           sizeof(indices) / sizeof(float),
                           GL_UNSIGNED_INT, (void *) 0);
        }
        //Model Matrix
        mat4x4_translate(model, xoffset, yoffset, zoffset);
        mat4x4_rotate_X(model, model, xangle);
        mat4x4_rotate_Y(model, model, yangle);
        mat4x4_rotate_Z(model, model, zangle);
        mat4x4_scale_aniso(model, model, 28.f, 28.f, 28.f);
        //MVP
        mat4x4_mul(mvp, projection, view);
        mat4x4_mul(mvp, mvp, model);
        glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, (const float *) mvp);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glUniform1i(glGetUniformLocation(ShaderProgram, "texType"), 2);
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

void walk_along_direction(vec3 camera_pos, vec3 const direction, float const speed) {
    vec3 temp;
    vec3_scale(temp, direction, speed);
    vec3_add(camera_pos, camera_pos, temp);
}

void walk_along_on_ground(vec3 camera_pos,
                          vec3 const front,
                          vec3 const up,
                          float const speed) {
    //figure out walk vector
    vec3 temp_front;
    vec3_norm(temp_front, front);
    vec3_scale(temp_front, temp_front, speed);
    //scale up_unit vector properly for the next process
    vec3 up_unit;
    vec3_norm(up_unit, up);
    vec3_scale(up_unit, up_unit, vec3_mul_inner(temp_front, up_unit));
    //figure out projection of walk vector on the level of camera
    vec3 step;
    vec3_sub(step, temp_front, up_unit);
    //calculate the camera position
    vec3_add(camera_pos, camera_pos, step);
}

void walk_sideways(vec3 camera_pos, vec3 const direction, vec3 const up, float const speed) {
    vec3 temp;
    vec3_mul_cross(temp, direction, up);
    vec3_norm(temp, temp);
    vec3_scale(temp, temp, speed);
    vec3_add(camera_pos, camera_pos, temp);
}

/**
 * Process user key inputs
 */
inline void processInput(GLFWwindow *window) {
    static double lastTime = 0;
    static float delta1 = 0.001f, delta2 = 1e-3f;
    static int speed_up = 0;
    double t = glfwGetTime();
    float camera_speed = (speed_up ? 10.f : 1.f) * CAMERA_SPEED * delta_time;
    const float ratio = 0.025f;
    static int camera_mode = 1;
    //
    if (t - lastTime < 0.005) {
        //return;
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
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (camera_mode == 1) {
            walk_along_on_ground(camera, direction, up, camera_speed);
        } else {
            walk_along_direction(camera, direction, camera_speed);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (camera_mode == 1) {
            walk_along_on_ground(camera, direction, up, -camera_speed);
        } else {
            walk_along_direction(camera, direction, -camera_speed);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        walk_sideways(camera, direction, up, -camera_speed);
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        walk_sideways(camera, direction, up, camera_speed);
    }
    //jump
    static int jump = 0;
    if (jump || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        static float pos, v0 = 9, g = -10.f, v;
        vec3 h = {0, 1, 0};
        float height = 0;
        if (!jump) {
            //first jump
            pos = camera[1];
            v = v0;
            jump = 1;
        } else {
            v += delta_time * g;
            height = v * delta_time;
            if (camera[1] < pos) {
                jump = 0;
            } else {
                walk_along_direction(camera, h, height);
            }
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed_up = 1;
    } else {
        speed_up = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera_mode = !camera_mode;
    }
    scale = scale <= 1e-2f ? 1e-2f : scale;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    screen_w = width, screen_h = height;
    glViewport(0, 0, width, height);
}

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    static int first = 1;

    if (first) {
        last_x = (float) xpos;
        last_y = (float) ypos;
        first = 0;
    }

    float offset_x = (float) xpos - last_x;
    float offset_y = last_y - (float) ypos;
    last_x = (float) xpos;
    last_y = (float) ypos;

    float sensitivity = 5e-2f;
    offset_x *= sensitivity;
    offset_y *= sensitivity;

    yaw += offset_x;
    pitch += offset_y;

    if (pitch >= 89.9f) pitch = 89.9f;
    if (pitch <= -89.9f) pitch = -89.9f;

    vec3 front;
    front[0] = cosf(radius(yaw));
    front[1] = tanf(radius(pitch));
    front[2] = sinf(radius(yaw));
    vec3_norm(direction, front);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    //set mouse callback
    glfwSetCursorPosCallback(window, mouse_callback);
    return window;
}