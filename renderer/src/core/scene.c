#include <stdlib.h>
#include "core/graphics.h"
#include "core/maths.h"
#include "core/mesh.h"
#include "core/scene.h"
#include "core/skeleton.h"
#include "core/texture.h"
#include "shaders/cache_helper.h"
#include <vector>

scene_t *scene_create(vec3_t background, model_t *skybox, std::vector<model_t *> models,
                      float ambient_intensity, float punctual_intensity,
                      int shadow_width, int shadow_height) {

    scene_t *scene = new scene_t(); //(scene_t*)malloc(sizeof(scene_t));
    scene->background = vec4_from_vec3(background, 1);

    scene->skybox = skybox;
    scene->models = models;
                
    scene->ambient_intensity = ambient_intensity;
    scene->punctual_intensity = punctual_intensity;        
    scene->asset_path = cache_get_path();       
        
    if (shadow_width > 0 && shadow_height > 0) {
        scene->shadow_buffer = framebuffer_create(shadow_width, shadow_height);
        scene->shadow_map = texture_create(shadow_width, shadow_height);
    } else {
        scene->shadow_buffer = NULL;
        scene->shadow_map = NULL;
    }

    return scene;
}

void scene_release(scene_t *scene) {
    int num_models = scene->models.size();
    int i;
    if (scene->skybox) {
        model_t *skybox = scene->skybox;
        skybox->release(skybox);
    }
    for (i = 0; i < num_models; i++) {
        model_t *model = scene->models[i];
        model->release(model);
    }
    scene->models.clear();
    
    if (scene->shadow_buffer) {
        framebuffer_release(scene->shadow_buffer);
    }
    if (scene->shadow_map) {
        texture_release(scene->shadow_map);
    }
    free(scene);
}
