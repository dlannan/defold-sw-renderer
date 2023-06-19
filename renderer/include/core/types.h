#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include "core/scene.h"
#include "core/camera.h"
#include "core/graphics.h"
#include "core/maths.h"

typedef struct {
    scene_t *scene;
    int layer;
    texture_t *labels[5];
} userdata_t;

typedef struct {
	const char *scene_name;
	scene_t *(*create_scene)(void);
} creator_t;

typedef struct {
    /* orbit */
    int is_orbiting;
    vec2_t orbit_pos;
    vec2_t orbit_delta;
    /* pan */
    int is_panning;
    vec2_t pan_pos;
    vec2_t pan_delta;
    /* zoom */
    float dolly_delta;
    /* light */
    float light_theta;
    float light_phi;
    /* click */
    float press_time;
    float release_time;
    vec2_t press_pos;
    vec2_t release_pos;
    int single_click;
    int double_click;
    vec2_t click_pos;
} record_t;

typedef struct {
    framebuffer_t *framebuffer;
    camera_t *camera;
    vec3_t light_dir;
    vec2_t click_pos;
    int single_click;
    int double_click;
    float frame_time;
    float delta_time;
} context_t;

typedef struct {
    framebuffer_t *framebuffer;
    camera_t *camera;
    record_t record;
    context_t context;
    float aspect;
    float prev_time;
    float print_time;
    int num_frames;
} scene_info_t;

#endif