#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "core/api.h"
#include "core/types.h"

typedef void tickfunc_t(context_t *context, userdata_t *userdata);

void test_enter_mainloop(tickfunc_t *tickfunc, userdata_t *userdata);
scene_t *test_create_scene(const char *scene_name, const char *scene_filename, const char *asset_path);
perframe_t test_build_perframe(scene_t *scene, context_t *context);
void test_draw_scene(scene_t *scene, framebuffer_t *framebuffer, perframe_t *perframe);

void set_matrix_root( vec3_t pos, vec3_t rot, vec3_t scale );
scene_info_t test_init_mainloop( int width, int height );

void test_update_camera(scene_info_t &info, vec3_t pos, vec3_t target);
void test_update_light(scene_info_t &info, float theta, float phi);

void test_run_mainloop(scene_info_t &info, tickfunc_t *tickfunc, userdata_t *userdata);

void test_release_mainloop(scene_info_t &info);

#endif
