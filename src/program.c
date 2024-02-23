#include <glad/glad.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>

#include "program.h"
#include "shader.h"

char *read_shader_source(const char *path) {
    FILE *file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = malloc(length + 1);
    fread(source, 1, length, file);
    source[length] = 0;

    fclose(file);
    return source;
}

Program create_compute_program(const char *compute_shader_path) {
    Program program;
    program.id = glCreateProgram();

    char *compute_shader_source = read_shader_source(compute_shader_path);
    Shader compute_shader = create_shader(GL_COMPUTE_SHADER, compute_shader_source);
    free(compute_shader_source);
    glAttachShader(program.id, compute_shader.id);
    delete_shader(compute_shader);

    glLinkProgram(program.id);
    glUseProgram(program.id);
    return program;
}

Program create_render_program(const char *vertex_shader_path, const char *fragment_shader_path) {
    Program program;
    program.id = glCreateProgram();

    char *vertex_shader_source = read_shader_source(vertex_shader_path);
    char *fragment_shader_source = read_shader_source(fragment_shader_path);
    Shader vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_source);
    Shader fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    free(vertex_shader_source);
    free(fragment_shader_source);
    glAttachShader(program.id, vertex_shader.id);
    glAttachShader(program.id, fragment_shader.id);
    delete_shader(vertex_shader);
    delete_shader(fragment_shader);

    glLinkProgram(program.id);
    glUseProgram(program.id);
    return program;
}

void use_program(Program *program) {
    glUseProgram(program->id);
}

void delete_program(Program program) {
    glDeleteProgram(program.id);
}
