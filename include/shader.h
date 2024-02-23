#ifndef __SHADER_H
#define __SHADER_H

#include <glad/glad.h>

typedef struct Shader {
    GLuint id;
} Shader;

Shader create_shader(GLint stage, const char *source);
void delete_shader(Shader shader);

#endif //__SHADER_H
