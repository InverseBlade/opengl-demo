#include "glutil.h"

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
	char *content = (char *)calloc(size + 1, sizeof(char));
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
		GLchar *info = (GLchar *)calloc(length, sizeof(GLchar));
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
	free((void *)source);
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
		GLchar *info = (GLchar *)calloc(length, sizeof(GLchar));
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
