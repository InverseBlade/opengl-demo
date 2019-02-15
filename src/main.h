//
// Created by Administrator on 2019/2/15.
//

#ifndef OPENGL_DEMO_MAIN_H
#define OPENGL_DEMO_MAIN_H

#include "linmath.h"
#include "GLFW/glfw3.h"

typedef struct _Camera {
    vec3 position;
    vec3 direction;
    vec3 up;
} Camera;

typedef struct _Player {
    GLuint id;
    Camera camera;
} Player;

typedef struct _ShaderAttrib {
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

typedef struct _Global {

} Global;

void gen_sky_buffer(GLuint *array_buffer, GLuint *element_buffer);

void render_sky(ShaderAttrib *attrib, Player *player, GLuint array_buffer, GLuint element_buffer);

void draw_triangles_3d(ShaderAttrib *attrib, GLuint buffer, int count);

#endif //OPENGL_DEMO_MAIN_H
