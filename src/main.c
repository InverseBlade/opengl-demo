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

#define RENDER_WID 480
#define RENDER_HGT 480
#define WIN_TITLE "旋转方块——帧数:"

#define PI (3.14159f)
#define radius(angle) (PI * (angle) / 180.f)
#define MODE_LINE 1
#define MODE_FILL 2

#define CAMERA_SPEED 8

typedef struct {
    vec3 position;
    vec3 direction;
    vec3 up;
} Camera;

typedef struct {
    GLuint id;
    Camera camera;
} Player;

typedef struct {
    GLuint program;
    GLuint position;
    GLuint color;
    GLuint uv;
    GLint matrix;
    GLint sampler1;
    GLint sampler2;
    GLuint texture1;
    GLuint texture2;
    GLuint texture_type;
} ShaderAttrib;

typedef struct {

} Global;

GLuint gen_sky_buffer();

GLuint gen_sprite_buffer();

void render_sprite(ShaderAttrib *attrib, Player *player, GLuint vao);

void render_sky(ShaderAttrib *attrib, Player *player, GLuint vao);

void draw_triangles_3d(ShaderAttrib *attrib, GLuint vao, int count);

void make_cube(float *vertices, unsigned int *indices);

void get_vision_matrix(mat4x4 M, Camera *camera);

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

/**
 * Main
 * @param argc
 * @param argv
 * @return
 */
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
    ShaderAttrib attrib = {0};
    //-------------------Create Shaders------------------------//
    attrib.program = loadProgram(
            "shaders/VertexShaderSrc.glsl",
            "shaders/FragmentShaderSrc.glsl");
    //---------------------------------------------------------//
    //Load Texture
    //Generate Texture
    attrib.texture1 = loadTexture("textures/face.png", GL_RGBA);
    attrib.texture2 = loadTexture("textures/bricks.jpg", GL_RGB);
    attrib.sampler1 = glGetUniformLocation(attrib.program, "myTexture1");
    attrib.sampler2 = glGetUniformLocation(attrib.program, "myTexture2");
    // Get Uniforms Location
    attrib.matrix = glGetUniformLocation(attrib.program, "MVP");
    attrib.position = 1;
    attrib.color = 2;
    attrib.uv = 3;

    ShaderAttrib attrib_sprite;
    memcpy(&attrib_sprite, &attrib, sizeof(ShaderAttrib));
    attrib_sprite.texture1 = loadTexture("textures/container.jpg", GL_RGB);
    attrib_sprite.texture2 = loadTexture("textures/container.jpg", GL_RGB);

    GLuint sky_vao = gen_sky_buffer();
    GLuint sprite_vao = gen_sprite_buffer();
    //
    Player player;
    last_frame = (float) glfwGetTime();
    float fps_update = (float) glfwGetTime(), fps = 0;
    int fps_count = 0;
    // Loop
    while (!glfwWindowShouldClose(window)) {
        //delta time process
        float current_frame = (float) glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
        //FPS
        fps_count++, fps += 1.0f / delta_time;
        if (current_frame - fps_update > 0.01f) {
            char title[255];
            sprintf(title, "%s%d", WIN_TITLE, (int) (fps / fps_count + 0.5f));
            glfwSetWindowTitle(window, title);
            fps_update = current_frame;
            fps = fps_count = 0;
        }
        // input
        processInput(window);
        // Render
        glClearColor(0.8627f, 0.8627f, 0.8627f, 1.f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_DEPTH_TEST);

        Camera *eye = &player.camera;
        vec3_scale(eye->position, camera, 1.0f);
        vec3_scale(eye->direction, direction, 1.0f);
        vec3_scale(eye->up, up, 1.0f);
        //render scene
        render_sky(&attrib, &player, sky_vao);
        render_sprite(&attrib_sprite, &player, sprite_vao);

        // check events
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    return 0;
}

void draw_triangles_3d(ShaderAttrib *attrib, GLuint vao, int count) {
    glBindVertexArray(vao);
    // Assign Data To Vertex Attributes
    // vertex position
    glVertexAttribPointer(attrib->position, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void *) (sizeof(float) * 0));
    glEnableVertexAttribArray(attrib->position);
    // vertex color
    glVertexAttribPointer(attrib->color, 3, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void *) (sizeof(float) * 3));
    glEnableVertexAttribArray(attrib->color);
    //texture coord
    glVertexAttribPointer(attrib->uv, 2, GL_FLOAT, GL_FALSE,
                          8 * sizeof(float),
                          (void *) (sizeof(float) * 6));
    glEnableVertexAttribArray(attrib->uv);
    //Draw
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void *) 0);
    //
    glDisableVertexAttribArray(attrib->position);
    glDisableVertexAttribArray(attrib->color);
    glDisableVertexAttribArray(attrib->uv);
    glBindVertexArray(0);
}

void make_cube(float *vertices, unsigned int *indices) {
    float v[][8] = {
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
    GLuint i[] = {
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
    memcpy(vertices, v, sizeof(v));
    memcpy(indices, i, sizeof(i));
}

GLuint gen_sky_buffer() {
    float vertices[8 * 24];
    GLuint indices[36];
    GLuint vao;

    make_cube(vertices, indices);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, gen_buffer(8 * 24 * sizeof(float), (float *) vertices));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gen_element_buffer(36 * sizeof(GLuint), indices));
    glBindVertexArray(0);
    return vao;
}

void get_vision_matrix(mat4x4 M, Camera *camera) {
    mat4x4 view, projection;
    //View Matrix
    vec3 target;
    vec3_add(target, camera->position, camera->direction);
    mat4x4_look_at(view, camera->position, target, camera->up);
    //Projection Matrix
    float ratio = (float) screen_w / screen_h;
    mat4x4_perspective(projection, 45.f / 180 * PI, ratio, 0.1f, 1000.0f);
    mat4x4_mul(M, projection, view);
}

void render_sky(ShaderAttrib *attrib, Player *player, GLuint vao) {
    //
    glUseProgram(attrib->program);
    //mvp
    mat4x4 mvp, model, vp;
    //model
    mat4x4_identity(model);
    mat4x4_scale_aniso(model, model, 28.f, 28.f, 28.f);
    //MVP
    get_vision_matrix(vp, &(player->camera));
    mat4x4_mul(mvp, vp, model);
    glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, (const float *) mvp);
    //
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, attrib->texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, attrib->texture2);
    glUniform1i(attrib->sampler1, 0);
    glUniform1i(attrib->sampler2, 1);
    glUniform1i(glGetUniformLocation(attrib->program, "texType"), 2);
    draw_triangles_3d(attrib, vao, 36);
}

GLuint gen_sprite_buffer() {
    float vertices[8 * 24];
    GLuint indices[36];
    GLuint vao;

    make_cube(vertices, indices);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, gen_buffer(8 * 24 * sizeof(float), vertices));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gen_element_buffer(36 * sizeof(unsigned int), indices));
    glBindVertexArray(0);
    return vao;
}

void render_sprite(ShaderAttrib *attrib, Player *player, GLuint vao) {
    glUseProgram(attrib->program);
    //matrix
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
    mat4x4 mvp;
    mat4x4 model, vp;
    //View Matrix
    get_vision_matrix(vp, &(player->camera));
    //Draw Cubes
    mat4x4 temp;
    glUniform1i(glGetUniformLocation(attrib->program, "texType"), 1);
    for (int i = 0; i < 10; ++i) {
        mat4x4_identity(model);
        mat4x4_rotate_Y(model, model, (float) glfwGetTime());
        mat4x4_translate(temp, cube_pos[i].x, cube_pos[i].y, cube_pos[i].z);
        mat4x4_mul(model, model, temp);
        float angle = 20.0f * i;
        mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, angle);
        mat4x4_scale_aniso(model, model, scale, scale, scale);
        mat4x4_mul(mvp, vp, model);
        glUniformMatrix4fv(attrib->matrix, 1, GL_FALSE, (const float *) mvp);
        //
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, attrib->texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, attrib->texture2);
        glUniform1i(attrib->sampler1, 0);
        glUniform1i(attrib->sampler2, 1);
        draw_triangles_3d(attrib, vao, 36);
    }
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

    yaw = yaw <= -360.f ? 0.f : yaw;
    yaw = yaw >= 360.f ? 0.f : yaw;
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