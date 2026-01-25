#ifndef FZ_MATH_H
#define FZ_MATH_H

#include "FZ_types.h"

double distance_2(FZ_vector_2 a, FZ_vector_2 b){
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double squared_distance_2(FZ_vector_2 a, FZ_vector_2 b){
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

FZ_vector_2 v2_mul(FZ_vector_2 vector, double value){
    vector.x *= value;
    vector.y *= value;

    return vector;
}

// element-wise add
FZ_vector_2 v2_ew_add(FZ_vector_2 vector_a, FZ_vector_2 vector_b){
    vector_a.x += vector_b.x;
    vector_a.y += vector_b.y;

    return vector_a;
}

// element-wise subtract
FZ_vector_2 v2_ew_sub(FZ_vector_2 vector_a, FZ_vector_2 vector_b){
    vector_a.x -= vector_b.x;
    vector_a.y -= vector_b.y;

    return vector_a;
}

// element-wise multiply
FZ_vector_2 v2_ew_mul(FZ_vector_2 vector_a, FZ_vector_2 vector_b){
    vector_a.x *= vector_b.x;
    vector_a.y *= vector_b.y;

    return vector_a;
}

FZ_vector_2 vector_2_lerp(FZ_vector_2 vector_a, FZ_vector_2 vector_b, double w1){
    double w0 = 1.0 - w1;

    FZ_vector_2 result = (FZ_vector_2){0, 0};

    vector_a = v2_mul(vector_a, w0);
    vector_b = v2_mul(vector_b, w1);

    result = v2_ew_add(result, vector_a);
    result = v2_ew_add(result, vector_b);

    return result;
}

FZ_vector_2 v2_rot(FZ_vector_2 vector, FZ_vector_2 origin, double angle){
    FZ_vector_2 new_vec = vector;

    new_vec = v2_ew_sub(new_vec, origin);

    new_vec.x = vector.x * cos(angle) - vector.y * sin(angle);
    new_vec.y = vector.x * sin(angle) + vector.y * cos(angle);

    new_vec = v2_ew_add(new_vec, origin);

    return new_vec;
}

double v2_dot(FZ_vector_2 vector_a, FZ_vector_2 vector_b){
    double result = vector_a.x * vector_b.x + vector_a.y * vector_b.y;

    return result;
}

FZ_vector_2 v2_normalize(FZ_vector_2 vector){
    return v2_mul(vector, 1 / sqrt(v2_dot(vector, vector)));
}

FZ_vector_2 v2_rotate_neg_90(FZ_vector_2 vector){
    return (FZ_vector_2){-vector.y, vector.x};
}

FZ_vector_2 v2_rotate_90(FZ_vector_2 vector){
    return (FZ_vector_2){vector.y, -vector.x};
}

// where a and b form line a, project c onto line a, return the scalar
double project_along_vectors_normal(FZ_vector_2 a, FZ_vector_2 b, FZ_vector_2 c){
    b = (FZ_vector_2){b.y - a.y, -(b.x - a.x)};
    
    c = v2_ew_sub(c, a);

    return (v2_dot(c, b) / v2_dot(b, b));
}

FZ_vector_2 v2_scalar_cross(FZ_vector_2 vector, double value){
    return v2_mul(v2_rotate_neg_90(vector), value);
}

double v2_v2_cross(FZ_vector_2 a, FZ_vector_2 b) {
    return a.x * b.y - a.y * b.x;
}

#endif