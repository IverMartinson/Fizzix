#ifndef FZ_STRUCTS
#define FZ_STRUCTS

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct {
    double x, y;
} FZ_vector_2;

enum {
    FZ_SHAPE_IS_STATIC = 1 << 0,
    FZ_SHAPE_IS_COLLIDING = 1 << 1,
};

typedef struct {
    FZ_vector_2* points;
    FZ_vector_2* transformed_points;
    uint16_t point_count;
    FZ_vector_2 scale;
    FZ_vector_2 position;
    FZ_vector_2 velocity;
    double angle;
    double angular_veclocity;
    double mass;
    double moment_of_intertia;
    void (*tick_transform_function)(FZ_vector_2 position, double angle);
    void (*tick_points_function)(FZ_vector_2* points, uint16_t point_count);
    uint16_t flags; 
} FZ_shape;

typedef struct {
    double penetration;
    FZ_vector_2 contact_point;
    int reference_line_index;
    int incident_line_index;
    FZ_shape* reference_shape;
    FZ_shape* incident_shape;
} projected_points_data;

typedef struct {
    FZ_shape** shapes;
    uint16_t shape_count;
    FZ_vector_2 gravity;
} FZ_scene; 

typedef struct {
    uint16_t width, height, half_width, half_height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* frame_buffer_texture;
    uint32_t* frame_buffer;
} FZ_SDL;

typedef struct {
    uint16_t flags;
    FZ_SDL sdl;
    uint8_t is_running;
} FZ_context;

#endif 