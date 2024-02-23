#include <glad/glad.h>
#include <stdio.h>

#include "shader.h"

Shader create_shader(GLint stage, const char *source) {
    Shader shader;
    shader.id = glCreateShader(stage);
    glShaderSource(shader.id, 1, &source, NULL);
    glCompileShader(shader.id);
    GLint compilation_status;
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &compilation_status);
    if (compilation_status != GL_TRUE) {
        char log[4096];
        glGetShaderInfoLog(shader.id, sizeof(log), NULL, log);
        printf("Compilation failed: %s\n", log);
    }
    return shader;
}

void delete_shader(Shader shader) {
    glDeleteShader(shader.id);
}
