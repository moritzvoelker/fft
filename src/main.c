#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string.h>

typedef struct Shader {
    GLuint id;
} Shader;

typedef struct Program {
    GLuint id;
    Shader shaders[3];
} Program;

typedef struct Array {
    GLuint id;
} Array;

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

Program create_program(const char *vertex_shader, const char *fragment_shader, const char *compute_shader) {
    Program program = {0};
    program.id = glCreateProgram();
    if (vertex_shader) {
        program.shaders[0] = create_shader(GL_VERTEX_SHADER, vertex_shader);
        glAttachShader(program.id, program.shaders[0].id);
    }
    if (fragment_shader) {
        program.shaders[1] = create_shader(GL_FRAGMENT_SHADER, fragment_shader);
        glAttachShader(program.id, program.shaders[1].id);
    }
    if (compute_shader) {
        program.shaders[2] = create_shader(GL_COMPUTE_SHADER, compute_shader);
        glAttachShader(program.id, program.shaders[2].id);
    }
    glLinkProgram(program.id);
    glUseProgram(program.id);

    return program;
}

void delete_program(Program program) {
    for (int i = 0; i < 3; i++) {
        if (program.shaders[i].id) {
            glDetachShader(program.id, program.shaders[i].id);
            delete_shader(program.shaders[i]);
        }
    }
    glDeleteProgram(program.id);
}

Array create_array(float *data, size_t len, GLuint binding) {
    Array array;
    glGenTextures(1, &array.id);
    glBindTexture(GL_TEXTURE_1D, array.id);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, len, 0, GL_RG, GL_FLOAT, data);
    glBindImageTexture(binding, array.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
    return array;
}

void delete_array(Array array) {
    glBindTexture(0, 0);
    glDeleteTextures(1, &array.id);
}

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

int main(void) {
    // init OpenGL
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(100, 100, "", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // init program with shaders
    char *compute_shader_source = read_shader_source("src/compute_shader.glsl");
    float compute_shader_data[] = {0.0,0.0, 1.0,0.0 , 2.0,0.0, 3.0,0.0, 4.0,0.0, 5.0,0.0, 6.0,0.0, 7.0,0.0,};
    size_t compute_shader_data_len = sizeof(compute_shader_data) / sizeof(float) / 2;
    Program program = create_program(NULL, NULL, compute_shader_source);
    free(compute_shader_source);
    Array array = create_array(compute_shader_data, compute_shader_data_len, 0);

    // run compute shader
    glDispatchCompute(compute_shader_data_len / 2, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // present result
    glBindTexture(GL_TEXTURE_1D, array.id);
    glGetTexImage(GL_TEXTURE_1D, 0, GL_RG, GL_FLOAT, compute_shader_data);
    for (size_t i = 0; i < compute_shader_data_len; i++) {
        printf("(%f,%f),\n", compute_shader_data[i * 2], compute_shader_data[i * 2 + 1]);
    }
    printf("\n");

    // cleanup
    delete_array(array);
    delete_program(program);
}
