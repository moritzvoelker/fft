#version 460 core

layout(std430, binding = 0) buffer Buffer {
    vec2 data[];
} fft_data;
uniform uint n;

float c_abs(vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

void main()
{
    vec2 fft = fft_data.data[gl_VertexID];
    gl_Position = vec4(float(gl_VertexID) / float(n - 1) * 2.0 - 1.0, c_abs(fft) / 28.0, 0.0, 1.0);
}
