#include "../headers/fizzix.h"

int main(){
    FZ_init();

    FZ_context* context = FZ_get_context();

    FZ_scene* scene = FZ_new_scene();

    FZ_shape* shape_1 = FZ_new_shape();
    FZ_shape* shape_2 = FZ_new_shape();

    scene->shapes = malloc(sizeof(FZ_shape*) * 2);

    scene->shapes[0] = shape_1;
    scene->shapes[1] = shape_2;

    shape_2->flags = FZ_SHAPE_IS_STATIC;

    shape_1->scale = (FZ_vector_2){50, 50};
    shape_2->scale = (FZ_vector_2){190, 40};

    shape_1->position = (FZ_vector_2){0, 150};
    shape_2->position = (FZ_vector_2){0, -150};


    scene->shape_count = 2;

    shape_1->velocity = (FZ_vector_2){0.01, .02};
    
    shape_1->angular_veclocity = 0.001;
    shape_2->angular_veclocity = 0.002;

    while (context->is_running){
        FZ_tick(scene);
        FZ_render_debug(scene);
    }

    return 0;
}
