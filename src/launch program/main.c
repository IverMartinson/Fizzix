#include "../headers/fizzix.h"
#include <time.h>

int main(){
    FZ_init();

    FZ_context* context = FZ_get_context();

    FZ_scene* scene = FZ_new_scene();

    FZ_shape* shape_1 = FZ_new_shape();
    FZ_shape* shape_2 = FZ_new_shape();
    FZ_shape* shape_3 = FZ_new_shape();
    FZ_shape* shape_4 = FZ_new_shape();
    FZ_shape* shape_5 = FZ_new_shape();
    FZ_shape* shape_6 = FZ_new_shape();
    FZ_shape* shape_7 = FZ_new_shape();
    FZ_shape* shape_8 = FZ_new_shape();
    FZ_shape* shape_9 = FZ_new_shape();
    FZ_shape* shape_10 = FZ_new_shape();
    FZ_shape* shape_11 = FZ_new_shape();

    scene->shapes = malloc(sizeof(FZ_shape*) * 3);

    scene->shapes[0] = shape_1;
    scene->shapes[1] = shape_2;
    scene->shapes[2] = shape_3;
    scene->shapes[3] = shape_4;
    scene->shapes[4] = shape_5;
    scene->shapes[5] = shape_6;
    scene->shapes[6] = shape_7;
    scene->shapes[7] = shape_8;
    scene->shapes[8] = shape_9;
    scene->shapes[9] = shape_10;
    scene->shapes[10] = shape_11;

    shape_2->flags = FZ_SHAPE_IS_STATIC;

    shape_1->scale = (FZ_vector_2){100, 20};
    shape_3->scale = (FZ_vector_2){100, 20};
    shape_4->scale = (FZ_vector_2){100, 20};
    shape_5->scale = (FZ_vector_2){100, 20};
    shape_2->scale = (FZ_vector_2){700, 100};
    shape_6->scale = (FZ_vector_2){50, 50};
    shape_7->scale = (FZ_vector_2){50, 50};
    shape_8->scale = (FZ_vector_2){50, 50};
    shape_9->scale = (FZ_vector_2){50, 50};
    shape_10->scale = (FZ_vector_2){50, 50};
    shape_11->scale = (FZ_vector_2){50, 50};

    shape_1->position = (FZ_vector_2){-150, 50};
    shape_3->position = (FZ_vector_2){150, 50};
    shape_4->position = (FZ_vector_2){150, 300};
    shape_5->position = (FZ_vector_2){150, 200};
    shape_6->position = (FZ_vector_2){500, 100};
    shape_7->position = (FZ_vector_2){500, 150};
    shape_8->position = (FZ_vector_2){500, 200};
    shape_9->position = (FZ_vector_2){500, 250};
    shape_10->position = (FZ_vector_2){500, 300};
    shape_11->position = (FZ_vector_2){500, 350};
    shape_2->position = (FZ_vector_2){0, -150};

    scene->shape_count = 11;

    shape_1->velocity = (FZ_vector_2){60, 300};
    shape_3->velocity = (FZ_vector_2){-60, 300};
    shape_4->velocity = (FZ_vector_2){-60, -300};
    shape_5->velocity = (FZ_vector_2){60, 300};
    
    shape_1->angular_velocity = 4;
    shape_3->angular_velocity = -5;
    shape_4->angular_velocity = -1;
    shape_5->angular_velocity = 3;
    // shape_2->angular_velocity = 2;

    shape_3->mass = 1;

    long int start, end;
    double fps = 0;

    float total_fps = 0;

    double deltatime = 0;
    double delta_min = 0.0001;
    double delta_max = 100000;

    
    while (context->is_running){
        start = clock();

        FZ_tick(scene, deltatime);
        FZ_render_debug(scene);

        end = clock();

        deltatime = fmin(fmax((double)(end - start) / (double)(CLOCKS_PER_SEC), delta_min), delta_max);

        // printf("%d fps      \r", (int)(1.0 / deltatime));
    }

    return 0;
}
