#include <glad/glad.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string.h>
#include <unistd.h>

#include "program.h"

typedef struct Array {
    GLuint buffer;
    GLuint elements;
    GLuint dummy;
    GLuint array;
    size_t len;
} Array;

Array create_array(size_t len) {
    Array array;
    glGenBuffers(3, &array.buffer);
    glGenVertexArrays(1, &array.array);
    glBindVertexArray(array.array);
    array.len = len;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, array.buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, array.elements);
    GLuint *indices = malloc(sizeof(GLuint) * len);
    for (size_t i = 0; i < len; i++) {
        indices[i] = i;
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, len * sizeof(GLuint), indices, GL_STATIC_DRAW);
    free(indices);

    glBindBuffer(GL_ARRAY_BUFFER, array.dummy);

    return array;
}

void update_array(Array *array, float *data) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, array->buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, array->len * sizeof(float) * 2, data, GL_DYNAMIC_DRAW);
}

void delete_array(Array array) {
    glDeleteBuffers(3, &array.buffer);
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}


int main(void) {
    // init OpenGL
    glfwInit();
    GLFWwindow *window = glfwCreateWindow(1000, 1000, "GPU FFT", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glViewport(0, 0, 1000, 1000);

    // init program with shaders
    float compute_shader_data[] = {0.0,0.0, 1.0,0.0, 2.0,0.0, 3.0,0.0, 4.0,0.0, 5.0,0.0, 6.0,0.0, 7.0,0.0,};
    size_t compute_shader_data_size = sizeof(compute_shader_data);
    size_t compute_shader_data_len = compute_shader_data_size / sizeof(float) / 2;
    Program compute_program = create_compute_program("src/compute_shader.glsl");
    Program render_program = create_render_program("src/vertex_shader.glsl", "src/fragment_shader.glsl");
    glUniform1ui(glGetUniformLocation(render_program.id, "n"), compute_shader_data_len);
    Array array = create_array(compute_shader_data_len);

    // main loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // read input data
        update_array(&array, compute_shader_data);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // run compute shader
        use_program(&compute_program);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, array.buffer);
        glDispatchCompute(compute_shader_data_len / 2, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // draw result
        use_program(&render_program);
        glDrawElements(GL_LINE_STRIP, compute_shader_data_len, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // present result
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, array.buffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, compute_shader_data_size, compute_shader_data);
    for (size_t i = 0; i < compute_shader_data_len; i++) {
        printf("(%f,%f),\n", compute_shader_data[i * 2], compute_shader_data[i * 2 + 1]);
    }
    printf("\n");

    // cleanup
    delete_array(array);
    delete_program(compute_program);
    glfwTerminate();
    return 0;
}
