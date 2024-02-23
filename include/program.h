#ifndef __PROGRAM_H
#define __PROGRAM_H

#include <glad/glad.h>

typedef struct Program {
    GLuint id;
} Program;


Program create_compute_program(const char *compute_shader_path);
Program create_render_program(const char *vertex_shader_path, const char *fragment_shader_path);
void use_program(Program *program);
void delete_program(Program program);

#endif //__PROGRAM_H
