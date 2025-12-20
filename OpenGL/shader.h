#pragma once
#include <corecrt_malloc.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

/*
	Reads a file from given filename (shaders in this case)
*/
char* read_file(const char* filename)
{
	FILE* file;

	fopen_s(&file, filename, "rb");

	if (file == NULL) return NULL;
	fseek(file, 0, SEEK_END);

	long size = ftell(file);
	rewind(file);
	char* buffer = (char*)malloc(sizeof(char) * (size + 1));

	if (buffer == NULL) return NULL;

	long ret = fread(buffer, 1, size, file);

	if (ret != size) return NULL;

	buffer[size] = '\0';
	return buffer;
}

/*
	Compile Shaders for OpenGL Use
*/

GLuint CompileShader(const char* vertexFilename, const char* fragFilename)
{
	int success;
	char infoLog[512];

	// vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char* vertexSource = read_file(vertexFilename);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Vertex comp Fail > %s\n", infoLog);
	}

	// frag shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentSource = read_file(fragFilename);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Fragment comp Fail > %s\n", infoLog);
	}

	// link program
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetShaderiv(program, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(program, 512, NULL, infoLog);
		fprintf(stderr, "Link Fail > %s\n", infoLog);
	}

	//memory cleanup.
	free(fragmentSource);
	free(vertexSource);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;

}