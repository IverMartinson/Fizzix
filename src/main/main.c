#include "../headers/fizzix.h"
#include <stdio.h>

FZ_context context;

FZ_shape* FZ_new_shape(){
    FZ_shape* new_shape = malloc(sizeof(FZ_shape));

    // default transforms
    new_shape->position = (FZ_vector_2){0, 0};
    new_shape->velocity = (FZ_vector_2){0, 0};
    new_shape->angle = 0;
    new_shape->angular_velocity = 0;

    // default otha stuff
    new_shape->flags = 0;
    new_shape->mass = 1;
    new_shape->moment_of_inertia = 1;
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
    new_scene->restitution = 0.7;

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

        FZ_vector_2 line_normal = v2_normalize(v2_rotate_90(v2_ew_sub(shape_b->transformed_points[point_a], shape_b->transformed_points[point_b])));

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

    double highest_dot = -INFINITY;

    // draw_line(vector_2_lerp(data.reference_shape->transformed_points[data.reference_line_index], data.reference_shape->transformed_points[(data.reference_line_index + 1) % data.reference_shape->point_count], 0.5), v2_ew_add(vector_2_lerp(data.reference_shape->transformed_points[data.reference_line_index], data.reference_shape->transformed_points[(data.reference_line_index + 1) % data.reference_shape->point_count], 0.5), v2_mul(smallest_penetrations_normal, 40)), 0x22FFFFFF);

    // find the incident line that is most opposite of the reference line normal
    // also finding the most penetrating point
    for (int i = 0; i < data.incident_shape->point_count; i++){
        FZ_vector_2 line_start = data.incident_shape->transformed_points[i];
        FZ_vector_2 line_end = data.incident_shape->transformed_points[(i + 1) % data.incident_shape->point_count];
    
        // normal of this line
        FZ_vector_2 normal = v2_normalize(v2_rotate_90(v2_ew_sub(line_start, line_end)));
        
        // draw_line(vector_2_lerp(line_start, line_end, 0.5), v2_ew_add(vector_2_lerp(line_start, line_end, 0.5), v2_mul(normal, 40)), 0xFF22FFFF);

        // check if this normal and the reference normal are opposite
        double dot = v2_dot(normal, smallest_penetrations_normal);
        if (dot < lowest_dot){
            lowest_dot = dot;
            incident_line_index = i;
        }

        // find most penetrating point
        dot = v2_dot(v2_ew_sub(line_start, data.reference_shape->position), v2_mul(smallest_penetrations_normal, -1));
        if (dot > highest_dot){
            highest_dot = dot;

            data.point_b = line_start;
        }
    }

    data.incident_line_index = incident_line_index;

    // find point a
    
    FZ_vector_2 reference_a = data.reference_shape->transformed_points[(data.reference_line_index) % data.reference_shape->point_count];
    FZ_vector_2 reference_b = data.reference_shape->transformed_points[(data.reference_line_index + 1) % data.reference_shape->point_count];

    FZ_vector_2 reference_vector = v2_ew_sub(reference_b, reference_a);
    FZ_vector_2 point_to_project = v2_ew_sub(data.point_b, reference_a);

    double c = v2_dot(reference_vector, point_to_project) / v2_dot(reference_vector, reference_vector);

    data.point_a = v2_ew_add(
            v2_mul(reference_vector, c), 
            reference_a
        );

    // debug points
    // for (int i = 0; i < 8; i++){
        // draw_line(data.point_a, v2_ew_add(data.point_a, v2_rot((FZ_vector_2){0, 40}, (FZ_vector_2){0, 0}, 6.28318530718 * ((double)i / (double)8.0))), 0xFFFFFFFF);
        // draw_line(data.point_b, v2_ew_add(data.point_b, v2_rot((FZ_vector_2){0, 40}, (FZ_vector_2){0, 0}, 6.28318530718 * ((double)i / (double)8.0))), 0xFFFFFFFF);
    // }

    return data;
}

int FZ_tick(FZ_scene* scene, double deltatime){
    // update objects based on velocities
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];

        if (shape->flags & FZ_SHAPE_IS_COLLIDING) 
            shape->flags ^= FZ_SHAPE_IS_COLLIDING;

        if (!(shape->flags & FZ_SHAPE_IS_STATIC)){
            shape->velocity = v2_ew_add(shape->velocity, v2_mul(scene->gravity, deltatime));

            shape->position = v2_ew_add(shape->position, v2_mul(shape->velocity, deltatime));
            shape->angle += shape->angular_velocity * deltatime;
        }

        for (int j = 0; j < shape->point_count; j++){
            shape->transformed_points[j] = v2_ew_add(
                v2_rot(v2_ew_mul(shape->points[j], shape->scale), (FZ_vector_2){0, 0}, shape->angle),
                shape->position
            );
        }

        // printf("%f %f\n", shape->transformed_points[0].x, shape->transformed_points[0].y);
    }
    
    // check for colisions
    double iterations = 1;

    for (int k = 0; k < (int)iterations; k++){ 
        for (int i = 0; i < scene->shape_count; i++){
            FZ_shape* shape_1 = scene->shapes[i];
            
            for (int j = 0; j < scene->shape_count; j++){
                if (i <= j) continue;
                
                FZ_shape* shape_2 = scene->shapes[j];

                projected_points_data data = project_points(shape_1, shape_2);

                if (shape_1->flags & FZ_SHAPE_IS_STATIC && shape_2->flags & FZ_SHAPE_IS_STATIC) continue;

                // shapes are colliding
                if (data.penetration != 0){
                    shape_1->flags |= FZ_SHAPE_IS_COLLIDING;
                    shape_2->flags |= FZ_SHAPE_IS_COLLIDING;

                    int i0 = data.reference_line_index;
                    int i1 = (i0 + 1) % data.reference_shape->point_count;

                    FZ_vector_2 p0 = data.reference_shape->transformed_points[i0];
                    FZ_vector_2 p1 = data.reference_shape->transformed_points[i1];

                    FZ_vector_2 start = v2_mul(v2_ew_add(p0, p1), 0.5);

                    FZ_vector_2 edge = v2_ew_sub(p1, p0);

                    FZ_vector_2 reference_normal = v2_normalize((FZ_vector_2){ edge.y, -edge.x });

                    FZ_vector_2 pc_a = v2_ew_sub(
                                    data.point_a,
                                    data.reference_shape->position
                                );

                    FZ_vector_2 pc_b = v2_ew_sub(
                                    data.point_b,
                                    data.incident_shape->position
                                );
                                

                    FZ_vector_2 point_a_velocity = v2_ew_add(
                            data.reference_shape->velocity,
                            v2_scalar_cross(
                                pc_a,
                                data.reference_shape->angular_velocity
                            )
                        );

                    
                    FZ_vector_2 point_b_velocity = v2_ew_add(
                            data.incident_shape->velocity,
                            v2_scalar_cross(
                                pc_b,
                                data.incident_shape->angular_velocity
                            )
                        );
                    
                    double normal_velocity_scalar = v2_dot(v2_ew_sub(
                                point_a_velocity,
                                point_b_velocity
                            ),
                            reference_normal
                        );

                    double J_numerator = -(1 + scene->restitution) * normal_velocity_scalar;
                    
                    data.reference_shape->moment_of_inertia = data.reference_shape->mass * (pow(data.reference_shape->scale.x, 2) + pow(data.reference_shape->scale.y, 2)) / 12.0;
                    data.incident_shape->moment_of_inertia = data.incident_shape->mass * (pow(data.incident_shape->scale.x, 2) + pow(data.incident_shape->scale.y, 2)) / 12.0;

                    double m̃_a = data.reference_shape->flags & FZ_SHAPE_IS_STATIC ? 0.0 : 1.0 / data.reference_shape->mass + squared_distance_2(data.reference_shape->position, data.point_a) / data.reference_shape->moment_of_inertia;
                    double m̃_b = data.incident_shape->flags & FZ_SHAPE_IS_STATIC ? 0.0 : 1.0 / data.incident_shape->mass + squared_distance_2(data.incident_shape->position, data.point_b) / data.incident_shape->moment_of_inertia;
                    double J_denominator = m̃_a + m̃_b;
                
                    double J = J_numerator / J_denominator;

                    FZ_vector_2 delta_a_v = v2_mul(reference_normal, J / data.reference_shape->mass);
                    FZ_vector_2 delta_b_v = v2_mul(reference_normal, -J / data.incident_shape->mass);
                
                    double delta_a_w = (J * v2_v2_cross(pc_a, reference_normal)) / data.reference_shape->moment_of_inertia;
                    double delta_b_w = (-J * v2_v2_cross(pc_b, reference_normal)) / data.incident_shape->moment_of_inertia;

                    double separation = data.penetration / (double)(!(data.reference_shape->flags & FZ_SHAPE_IS_STATIC) + !(data.incident_shape->flags & FZ_SHAPE_IS_STATIC));

                    if(!(data.reference_shape->flags & FZ_SHAPE_IS_STATIC)){
                        data.reference_shape->velocity = v2_ew_add(
                            data.reference_shape->velocity,
                            delta_a_v
                        );
                
                        data.reference_shape->angular_velocity += delta_a_w;

                        data.reference_shape->position = v2_ew_add(
                            data.reference_shape->position, 
                            v2_mul(
                                reference_normal, 
                                separation
                            )
                        );
                    }

                    if(!(data.incident_shape->flags & FZ_SHAPE_IS_STATIC)){
                        data.incident_shape->velocity = v2_ew_add(
                            data.incident_shape->velocity,
                            delta_b_v
                        );
                    
                        data.incident_shape->angular_velocity += delta_b_w;
                    
                        data.incident_shape->position = v2_ew_add(
                            data.incident_shape->position, 
                            v2_mul(
                                reference_normal, 
                                -separation
                            )
                        );
                    }
                } else {
                }
            }
        }
    }
    

    return 0;
}

int FZ_render_debug(FZ_scene* scene){
    for (int i = 0; i < scene->shape_count; i++){
        FZ_shape* shape = scene->shapes[i];
        
        for (int j = 0; j < shape->point_count; j++){
            draw_line(shape->transformed_points[j], j < shape->point_count - 1 ? shape->transformed_points[j + 1] : shape->transformed_points[0], shape->flags & FZ_SHAPE_IS_COLLIDING ? 0x5555FFFF : 0x55FF55FF);
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

    context.sdl.width = 2000;
    context.sdl.height = 1000;

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
