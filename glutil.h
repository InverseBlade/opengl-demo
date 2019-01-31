#ifndef _my_glutil_h_
#define _my_glutil_h_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

char *loadFile(const char *path);

GLuint makeShader(GLenum type, const char *source);

GLuint loadShader(GLenum type, const char *path);

GLuint makeProgram(GLuint vertexShader, GLuint fragmentShader);

GLuint loadProgram(const char *vertexPath, const char *fragmentPath);

GLuint loadTexture(const char *texturePath, GLenum rgbMode);

#endif
