#include "glutil.h"
#include "stb_image.h"

char *loadFile(const char *path) {
    FILE *fp;
    if (NULL == (fp = fopen(path, "rb"))) {
        fprintf(stderr, "ERROR:failed to open file path: %s", path);
        system("pause");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char *content = (char *) calloc(size + 1, sizeof(char));
    content[size] = 0;
    fread(content, size, 1, fp);
    fprintf(stdout, "%s\n", content);
    fclose(fp);
    return content;
}

GLuint makeShader(GLenum type, const char *source) {
    GLuint shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        GLchar *info = (GLchar *) calloc(length, sizeof(GLchar));
        glGetShaderInfoLog(shader, length, NULL, info);
        fprintf(stderr, "glCompileShader failed:\n%s\n", info);
        free(info);
    }
    return shader;
}

GLuint loadShader(GLenum type, const char *path) {
    GLuint shader;
    const char *source;
    source = loadFile(path);
    shader = makeShader(type, source);
    free((void *) source);
    return shader;
}

GLuint makeProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program;
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        GLchar *info = (GLchar *) calloc(length, sizeof(GLchar));
        glGetProgramInfoLog(program, length, NULL, info);
        fprintf(stderr, "glLinkProgram failed: %s\n", info);
        free(info);
    }
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

GLuint loadProgram(const char *vertexPath, const char *fragmentPath) {
    GLuint vertexShader, fragmentShader;
    vertexShader = loadShader(GL_VERTEX_SHADER, vertexPath);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentPath);
    return makeProgram(vertexShader, fragmentShader);
}

GLuint loadTexture(const char *texturePath, GLenum rgbMode) {
    GLuint texture;
    int text_width, text_height, text_nrChannels;
    unsigned char *text_data = stbi_load(
            texturePath, &text_width, &text_height, &text_nrChannels, 0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB,
            text_width, text_height, 0,
            rgbMode, GL_UNSIGNED_BYTE, text_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(text_data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}







































