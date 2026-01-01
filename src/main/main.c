#include "../headers/fizzix.h"
#include <stdio.h>

FZ_context context;

FZ_shape* FZ_new_shape(){
    FZ_shape* new_shape = malloc(sizeof(FZ_shape));

    // default transforms
    new_shape->position = (FZ_vector_2){0, 0};
    new_shape->velocity = (FZ_vector_2){0, 0};
    new_shape->angle = 0;
    new_shape->angular_veclocity = 0;

    // default otha stuff
    new_shape->flags = 0;
    new_shape->mass = 1;
    new_shape->moment_of_intertia = 1;
    new_shape->point_count = 4;
    new_shape->points = malloc(sizeof(FZ_vector_2) * 4);
    new_shape->transformed_points = malloc(sizeof(FZ_vector_2) * 4);
    new_shape->points[0] = (FZ_vector_2){-1, -1};
    new_shape->points[1] = (FZ_vector_2){-1, 1};
    new_shape->points[2] = (FZ_vector_2){1, 1};
    new_shape->points[3] = (FZ_vector_2){1, -1};
    new_shape->scale = (FZ_vector_2){1, 1};
    new_shape->tick_transform_function = NULL;
    new_shape->tick_points_function = NULL;

    return new_shape;
}

FZ_scene* FZ_new_scene(){
    FZ_scene* new_scene = malloc(sizeof(FZ_scene));

    new_scene->shape_count = 0;
    new_scene->shapes = NULL;
    new_scene->gravity = (FZ_vector_2){0, -9.81};

    return new_scene;
}

int FZ_tick(FZ_scene* scene, double deltatime){
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];

        if (!(shape->flags & FZ_SHAPE_IS_STATIC)){
            shape->velocity = v2_ew_add(shape->velocity, v2_times(scene->gravity, deltatime));

            shape->position = v2_ew_add(shape->position, v2_times(shape->velocity, deltatime));
            shape->angle += shape->angular_veclocity * deltatime;
        }

        for (int j = 0; j < shape->point_count; j++){
            shape->transformed_points[j] = v2_ew_add(
                v2_rot(v2_ew_mul(shape->points[j], shape->scale), (FZ_vector_2){0, 0}, shape->angle),
                shape->position
            );
        }

        // printf("%f %f\n", shape->transformed_points[0].x, shape->transformed_points[0].y);
    }

    
    return 0;
}

void draw_line(FZ_vector_2 point_a, FZ_vector_2 point_b, uint32_t color){
    int num_pixels = (int)distance_2(point_a, point_b);

    for (int i = 0; i < num_pixels; ++i){
        FZ_vector_2 point_2_draw = vector_2_lerp(point_a, point_b, (double)i / (double)num_pixels);

        point_2_draw.x += (double)context.sdl.half_width;
        point_2_draw.y += (double)context.sdl.half_height;

        if (point_2_draw.x < 0 || point_2_draw.x >= (double)context.sdl.width || point_2_draw.y < 0 || point_2_draw.y >= (double)context.sdl.height) continue;

        context.sdl.frame_buffer[(int)((int)(context.sdl.height - point_2_draw.y) * (double)context.sdl.width + point_2_draw.x)] = color;
    }
}

int FZ_render_debug(FZ_scene* scene){
    for (int i = 0; i < context.sdl.width * context.sdl.height; i++){
        context.sdl.frame_buffer[i] = 0x333333FF;
    }
    
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];

        
        for (int j = 0; j < shape->point_count; j++){
            draw_line(shape->transformed_points[j], j < shape->point_count - 1 ? shape->transformed_points[j + 1] : shape->transformed_points[0], 0x00FF00FF);
        }
        
        for (int j = 0; j < shape->point_count; j++){
            double screen_x = shape->transformed_points[j].x + context.sdl.half_width;
            double screen_y = shape->transformed_points[j].y + context.sdl.half_height;
            
            if (screen_x < 0 || screen_x >= context.sdl.width || 
                screen_y < 0 || screen_y >= context.sdl.height) continue;
            
            int index = (int)(context.sdl.width * (int)(context.sdl.height - screen_y) + screen_x);
            context.sdl.frame_buffer[index] = 0xFFFFFFFF;
        }
    }

    SDL_Event event;

    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT: {
                context.is_running = 0;

                break;
            }

            default: {
                break;
            }
        }
    }

    int pitch = 500;

    SDL_LockTexture(
        context.sdl.frame_buffer_texture, 
        NULL, 
        (void*)&context.sdl.frame_buffer, 
        &pitch
    );

    SDL_UnlockTexture(context.sdl.frame_buffer_texture);

    SDL_RenderCopy(context.sdl.renderer, context.sdl.frame_buffer_texture, NULL, NULL);
    SDL_RenderPresent(context.sdl.renderer);

    return 0;
}

FZ_context* FZ_get_context(){
    return &context;
}

int FZ_init(){
    SDL_Init(0);

    context.flags = 0;

    context.sdl.width = 500;
    context.sdl.height = 500;

    context.sdl.half_width = context.sdl.width / 2;
    context.sdl.half_height = context.sdl.height / 2;
    
    // init SDL
    context.sdl.window = SDL_CreateWindow(
        "FIZZIX DEBUG", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        context.sdl.width, 
        context.sdl.height, 0
    );
    context.sdl.renderer = SDL_CreateRenderer(context.sdl.window, -1, SDL_RENDERER_ACCELERATED);
    context.sdl.frame_buffer_texture = SDL_CreateTexture(
        context.sdl.renderer, 
        SDL_PIXELFORMAT_BGRA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        context.sdl.width, 
        context.sdl.height
    );

    context.sdl.frame_buffer = malloc(
        sizeof(uint32_t) * context.sdl.width * context.sdl.height
    );

    context.is_running = 1;

    return 0;
}
