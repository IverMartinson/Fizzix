#ifndef FZ_FUNCTIONS_H
#define FZ_FUNCTIONS_H

#include "FZ_structs.h"

FZ_scene* FZ_new_scene();
FZ_shape* FZ_new_shape();
int FZ_init();
int FZ_tick(FZ_scene* scene, double deltatime); // ticks a scene
int FZ_render_debug(FZ_scene* scene); // renders a debug window
FZ_context* FZ_get_context();

#endif