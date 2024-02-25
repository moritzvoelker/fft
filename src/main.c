#include <glad/glad.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string.h>
#include <unistd.h>

#include "complex.h"
#include "program.h"

typedef struct Array {
    GLuint buffer;
    size_t len;
} Array;

Array create_array(size_t len) {
    Array array;
    glGenBuffers(1, &array.buffer);
    array.len = len;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, array.buffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, len * sizeof(Complex), NULL, GL_DYNAMIC_STORAGE_BIT);

    return array;
}

void update_array(Array *array, Complex *data) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, array->buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, array->len * sizeof(Complex), data);
}

void delete_array(Array array) {
    glDeleteBuffers(1, &array.buffer);
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
    Complex compute_shader_data[] = {
        { 0.0,0.0 },
        { 1.0,0.0 },
        { 2.0,0.0 },
        { 3.0,0.0 },
        { 4.0,0.0 },
        { 5.0,0.0 },
        { 6.0,0.0 },
        { 7.0,0.0 },
    };
    size_t compute_shader_data_size = sizeof(compute_shader_data);
    size_t compute_shader_data_len = compute_shader_data_size / sizeof(Complex);
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
        glClearColor(0.0, 0.0, 0.0, 1.0);
        use_program(&render_program);
        glDrawArrays(GL_LINE_STRIP, 0, compute_shader_data_len);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // present result
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, array.buffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, compute_shader_data_size, compute_shader_data);
    for (size_t i = 0; i < compute_shader_data_len; i++) {
        printf("(%f,%f),\n", compute_shader_data[i].real, compute_shader_data[i].imaginary);
    }
    printf("\n");

    // cleanup
    delete_array(array);
    delete_program(compute_program);
    glfwTerminate();
    return 0;
}
