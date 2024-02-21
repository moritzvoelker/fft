#version 460 core
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rg32f, binding = 0) uniform image1D fft_data;

#define M_PI 3.1415926535897932384626433832795


float c_abs(vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}
vec2 c_con(vec2 v){
    return vec2(v.x, -v.y);
}
vec2 c_car(vec2 v) {
    return vec2(v.x * cos(v.y), v.x * sin(v.y));
}
vec2 c_pol(vec2 v) {
    float r = c_abs(v);
    if (r == 0.0) {
        return vec2(0.0, 0.0);
    }
    float phi = acos(v.x / r);
    if (v.y < 0.0) {
        phi = -phi;
    }
    return vec2(r, phi);
}
vec2 c_add(vec2 a, vec2 b) {
    return vec2(a.x + b.x, a.y + b.y);
}
vec2 c_sub(vec2 a, vec2 b) {
    return vec2(a.x - b.x, a.y - b.y);
}
vec2 c_mul(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

vec2 c_pow(vec2 v, int exp) {
    v = c_pol(v);
    v.x = pow(v.x, exp);
    v.y *= exp;

    return c_car(v);
}

void butterfly(int base, int j, int elements, vec2 w, int block) {
    vec2 a = imageLoad(fft_data, base + j).xy;
    vec2 b = imageLoad(fft_data, base + j + elements / 2).xy;
    w = c_pow(w, j);
    imageStore(fft_data, base + j, vec4(c_add(a, c_mul(w, b)), 0.0, 0.0));
    imageStore(fft_data, base + j + elements / 2, vec4(c_sub(a, c_mul(w, b)), 0.0, 0.0));
}


void rearrange(void) {
    int self = int(gl_WorkGroupID.x);
    int elements = int(gl_NumWorkGroups.x) * 2;
    int rounds = int(log2(elements)) - 1;

    for (int i = 0; i < rounds; i++) {
        int block = self >> (rounds - i);
        int base = block * elements;
        int offset = self & ~((-1) << (rounds - i));
        elements /= 2;

        vec2 even = imageLoad(fft_data, self * 2).xy;
        vec2 odd = imageLoad(fft_data, self * 2 + 1).xy;
        memoryBarrierImage();
        imageStore(fft_data, base + offset, vec4(even, 0.0, 0.0));
        imageStore(fft_data, base + offset + elements, vec4(odd, 0.0, 0.0));
        memoryBarrierImage();
    }
}

void compute(void) {
    int self = int(gl_WorkGroupID.x);
    int elements = 2;
    int rounds = int(log2(gl_NumWorkGroups.x)) + 1;

    for (int i = 0; i < rounds; i++, elements *= 2) {
        int block = self >> i;
        int base = block * elements;
        int offset = self & ~((-1) << i);
        vec2 w = c_con(c_car(vec2(1.0, 2.0 * M_PI / elements)));

        butterfly(base, offset, elements, w, block);
        memoryBarrierImage();
        if (i == 2) {
            break;
        }
    }
}

void main(void) {
    rearrange();
    compute();
}
