#include "../headers/fizzix.h"
#include <time.h>

int main(){
    FZ_init();

    FZ_context* context = FZ_get_context();

    FZ_scene* scene = FZ_new_scene();

    FZ_shape* shape_1 = FZ_new_shape();
    FZ_shape* shape_2 = FZ_new_shape();
    FZ_shape* shape_3 = FZ_new_shape();

    scene->shapes = malloc(sizeof(FZ_shape*) * 3);

    scene->shapes[0] = shape_1;
    scene->shapes[1] = shape_2;
    scene->shapes[2] = shape_3;

    shape_2->flags = FZ_SHAPE_IS_STATIC;

    shape_1->scale = (FZ_vector_2){100, 20};
    shape_3->scale = (FZ_vector_2){100, 20};
    shape_2->scale = (FZ_vector_2){2000, 100};

    shape_1->position = (FZ_vector_2){-150, 50};
    shape_3->position = (FZ_vector_2){150, 50};
    shape_2->position = (FZ_vector_2){0, -150};

    scene->shape_count = 3;

    shape_1->velocity = (FZ_vector_2){60, 300};
    shape_3->velocity = (FZ_vector_2){-60, 300};
    
    shape_1->angular_velocity = 4;
    shape_3->angular_velocity = -5;
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
