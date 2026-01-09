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
    new_scene->gravity = (FZ_vector_2){0, -300};

    return new_scene;
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

projected_points_data project_points(FZ_shape* shape_a, FZ_shape* shape_b){
    projected_points_data data;

    data.penetration = INFINITY;

    // the normal of the line that we are comparing the smallest penetration to
    FZ_vector_2 smallest_penetrations_normal;

    // project points along shape a's normals
    for (int i = 0; i < shape_a->point_count; i++){
        uint16_t point_a = i;
        uint16_t point_b = (i + 1) % shape_a->point_count;

        FZ_vector_2 line_normal = v2_normalize(v2_rotate_90(v2_ew_sub(shape_a->transformed_points[point_a], shape_a->transformed_points[point_b])));

        double a_max = -INFINITY;
        double a_min = INFINITY;
        double b_max = -INFINITY;
        double b_min = INFINITY;

        for (int j = 0; j < shape_a->point_count; j++){
            double value = project_along_vectors_normal(shape_a->transformed_points[point_a], shape_a->transformed_points[point_b], shape_a->transformed_points[j]);

            if (value < a_min)
                a_min = value;
            
            if (value > a_max)
                a_max = value;
        }
        
        for (int j = 0; j < shape_b->point_count; j++){
            double value = project_along_vectors_normal(shape_a->transformed_points[point_a], shape_a->transformed_points[point_b], shape_b->transformed_points[j]);
            
            if (value < b_min)
                b_min = value;
            
            if (value > b_max)
                b_max = value;
        }
        
        if (!(b_min <= a_max && b_max >= a_min)){
            data.penetration = 0;
            return data;
        }

        double overlap = distance_2(shape_a->transformed_points[point_a], shape_a->transformed_points[point_b]) * (fmin(a_max, b_max) - fmax(a_min, b_min));

        FZ_vector_2 direction = v2_ew_sub(shape_b->position, shape_a->position);

        // found new smallest valid penetration
        if (v2_dot(direction, line_normal) > 0 && overlap < data.penetration){
            data.penetration = overlap;
            data.reference_line_index = i;
            data.reference_shape = shape_a;
            data.incident_shape = shape_b;
            smallest_penetrations_normal = line_normal;
        }
    }

    // project points along shape b's normals
    for (int i = 0; i < shape_b->point_count; i++){
        uint16_t point_a = i;
        uint16_t point_b = (i + 1) % shape_b->point_count;

        FZ_vector_2 line_normal = v2_normalize(v2_rotate_90(v2_ew_sub(shape_a->transformed_points[point_a], shape_a->transformed_points[point_b])));

        double a_max = -INFINITY;
        double a_min = INFINITY;
        double b_max = -INFINITY;
        double b_min = INFINITY;

        for (int j = 0; j < shape_a->point_count; j++){
            double value = project_along_vectors_normal(shape_b->transformed_points[point_a], shape_b->transformed_points[point_b], shape_a->transformed_points[j]);

            if (value < a_min){
                a_min = value;
            }
            
            if (value > a_max){
                a_max = value;
            }
        }
                        
        for (int j = 0; j < shape_b->point_count; j++){
            double value = project_along_vectors_normal(shape_b->transformed_points[point_a], shape_b->transformed_points[point_b], shape_b->transformed_points[j]);
            
            if (value < b_min)
                b_min = value;
            
            if (value > b_max)
                b_max = value;
        }
        
        if (!(b_min <= a_max && b_max >= a_min)){
            data.penetration = 0;
            return data;
        }

        double overlap = distance_2(shape_b->transformed_points[point_a], shape_b->transformed_points[point_b]) * (fmin(a_max, b_max) - fmax(a_min, b_min));

        FZ_vector_2 direction = v2_ew_sub(shape_a->position, shape_b->position);

        // found new smallest valid penetration
        if (v2_dot(direction, line_normal) > 0 && overlap < data.penetration){
            data.penetration = overlap;
            data.reference_line_index = i;
            data.reference_shape = shape_b;
            data.incident_shape = shape_a;
            smallest_penetrations_normal = line_normal;
        }
    }

    uint16_t incident_line_index;
    double lowest_dot = INFINITY;

    draw_line(vector_2_lerp(data.reference_shape->transformed_points[data.reference_line_index], data.reference_shape->transformed_points[(data.reference_line_index + 1) % data.reference_shape->point_count], 0.5), v2_ew_add(vector_2_lerp(data.reference_shape->transformed_points[data.reference_line_index], data.reference_shape->transformed_points[(data.reference_line_index + 1) % data.reference_shape->point_count], 0.5), v2_mul(smallest_penetrations_normal, 40)), 0x22FFFFFF);

    for (int i = 0; i < data.incident_shape->point_count; i++){
        FZ_vector_2 line_start = data.incident_shape->transformed_points[i];
        FZ_vector_2 line_end = data.incident_shape->transformed_points[(i + 1) % data.incident_shape->point_count];
    
        // normal of this line
        FZ_vector_2 normal = v2_normalize(v2_rotate_90(v2_ew_sub(line_start, line_end)));
        
        draw_line(vector_2_lerp(line_start, line_end, 0.5), v2_ew_add(vector_2_lerp(line_start, line_end, 0.5), v2_mul(normal, 40)), 0xFF22FFFF);

        // check if this normal and the reference normal are opposite
        double dot = v2_dot(normal, smallest_penetrations_normal);
        if (dot < lowest_dot){
            lowest_dot = dot;
            incident_line_index = i;
        }
    }

    data.incident_line_index = incident_line_index;

    FZ_vector_2 line_ab_vector = v2_ew_sub((data.incident_line_index - 1) < 0 ? data.incident_shape->transformed_points[data.incident_shape->point_count - 1] : data.incident_shape->transformed_points[data.incident_line_index - 1], data.incident_shape->transformed_points[data.incident_line_index]);
    FZ_vector_2 line_bc_vector = v2_ew_sub(data.incident_shape->transformed_points[data.incident_line_index], data.incident_shape->transformed_points[(data.incident_line_index + 1) % data.incident_shape->point_count]);

    double offset = v2_dot(data.reference_shape->transformed_points[data.reference_line_index], smallest_penetrations_normal);

    double dot_ab = v2_dot(line_ab_vector, smallest_penetrations_normal);
    double dot_bc = v2_dot(line_bc_vector, smallest_penetrations_normal);



    return data;
}

int FZ_tick(FZ_scene* scene, double deltatime){
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];

        if (shape->flags & FZ_SHAPE_IS_COLLIDING) 
            shape->flags ^= FZ_SHAPE_IS_COLLIDING;

        if (!(shape->flags & FZ_SHAPE_IS_STATIC)){
            shape->velocity = v2_ew_add(shape->velocity, v2_mul(scene->gravity, deltatime));

            shape->position = v2_ew_add(shape->position, v2_mul(shape->velocity, deltatime));
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
    
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape_1 = scene->shapes[i];
        
        for (int j = 0; j < scene->shape_count; j++){
            if (i <= j) continue;
            
            FZ_shape* shape_2 = scene->shapes[j];

            projected_points_data data = project_points(shape_1, shape_2);

            if (data.penetration != 0){
                shape_1->flags |= FZ_SHAPE_IS_COLLIDING;
                shape_2->flags |= FZ_SHAPE_IS_COLLIDING;

                int i0 = data.reference_line_index;
                int i1 = (i0 + 1) % data.reference_shape->point_count;

                FZ_vector_2 p0 = data.reference_shape->transformed_points[i0];
                FZ_vector_2 p1 = data.reference_shape->transformed_points[i1];

                FZ_vector_2 start = v2_mul(v2_ew_add(p0, p1), 0.5);

                FZ_vector_2 edge = v2_ew_sub(p1, p0);

                FZ_vector_2 normal = (FZ_vector_2){ edge.y, -edge.x };

                double len2 = v2_dot(normal, normal);

                if (len2 > 0.0) {
                    normal = v2_mul(normal, 1.0 / sqrt(len2));

                    FZ_vector_2 end =
                        v2_ew_add(start,
                            v2_mul(normal, data.penetration));

                    draw_line(start, end, 0xFF5555FF);
                
                    draw_line(data.incident_shape->transformed_points[data.incident_line_index], data.incident_shape->transformed_points[(data.incident_line_index + 1) % data.incident_shape->point_count], 0xFFFFFFFF);
                }
            } else {
            }
        }
    }
    

    return 0;
}

int FZ_render_debug(FZ_scene* scene){
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];
        
        for (int j = 0; j < shape->point_count; j++){
            // draw_line(shape->transformed_points[j], j < shape->point_count - 1 ? shape->transformed_points[j + 1] : shape->transformed_points[0], shape->flags & FZ_SHAPE_IS_COLLIDING ? 0x5555FFFF : 0x55FF55FF);
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

    for (int i = 0; i < context.sdl.width * context.sdl.height; i++){
        context.sdl.frame_buffer[i] = 0x333333FF;
    }

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
