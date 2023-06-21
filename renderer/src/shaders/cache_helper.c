#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "core/api.h"
#include "shaders/cache_helper.h"
#include "shaders/pbr_shader.h"

static char *duplicate_string(const char *source) {
    char *target = (char*)malloc(strlen(source) + 1);
    strcpy(target, source);
    return target;
}

/* mesh related functions */

typedef struct {
    char *filename;
    mesh_t *mesh;
    int references;
} cached_mesh_t;

static std::vector<cached_mesh_t> g_meshes;
static std::string     asset_path = "";

void cache_set_path(const char *temp_asset_path)
{
    asset_path = temp_asset_path;
    printf("assetpath: %s\n", asset_path.c_str());
}

const char *cache_get_path()
{
    return asset_path.c_str();
}

const char *get_pathfilename(const char *filename)
{
    static std::string fullpath = cache_get_path();
    fullpath += filename;
    //printf("filepath: %s\n", fullpath.c_str());
    return fullpath.c_str();
}

mesh_t *cache_acquire_mesh(const char *filename) {
    if (filename != NULL) {
        cached_mesh_t cached_mesh;
        int num_meshes = g_meshes.size();
        int i;

        for (i = 0; i < num_meshes; i++) {
            if (strcmp(g_meshes[i].filename, filename) == 0) {
                if (g_meshes[i].references > 0) {
                    g_meshes[i].references += 1;
                } else {
                    assert(g_meshes[i].references == 0);
                    assert(g_meshes[i].mesh == NULL);
                    g_meshes[i].mesh = mesh_load(filename);
                    g_meshes[i].references = 1;
                }
                return g_meshes[i].mesh;
            }
        }

        cached_mesh.filename = duplicate_string(filename);
        cached_mesh.mesh = mesh_load(filename);
        cached_mesh.references = 1;
        g_meshes.push_back(cached_mesh);
        return cached_mesh.mesh;
    } else {
        return NULL;
    }
}

void cache_release_mesh(mesh_t *mesh) {
    if (mesh != NULL) {
        int num_meshes = g_meshes.size();
        int i;
        for (i = 0; i < num_meshes; i++) {
            if (g_meshes[i].mesh == mesh) {
                assert(g_meshes[i].references > 0);
                g_meshes[i].references -= 1;
                if (g_meshes[i].references == 0) {
                    mesh_release(g_meshes[i].mesh);
                    g_meshes[i].mesh = NULL;
                }
                return;
            }
        }
        assert(0);
    }
}

/* skeleton related functions */

typedef struct {
    char *filename;
    skeleton_t *skeleton;
    int references;
} cached_skeleton_t;

static std::vector<cached_skeleton_t> g_skeletons;

skeleton_t *cache_acquire_skeleton(const char *filename) {
    if (filename != NULL) {
        cached_skeleton_t cached_skeleton;
        int num_skeletons = g_skeletons.size();
        int i;

        for (i = 0; i < num_skeletons; i++) {
            if (strcmp(g_skeletons[i].filename, filename) == 0) {
                if (g_skeletons[i].references > 0) {
                    g_skeletons[i].references += 1;
                } else {
                    assert(g_skeletons[i].references == 0);
                    assert(g_skeletons[i].skeleton == NULL);
                    g_skeletons[i].skeleton = skeleton_load(filename);
                    g_skeletons[i].references = 1;
                }
                return g_skeletons[i].skeleton;
            }
        }

        cached_skeleton.filename = duplicate_string(filename);
        cached_skeleton.skeleton = skeleton_load(filename);
        cached_skeleton.references = 1;
        g_skeletons.push_back(cached_skeleton);
        return cached_skeleton.skeleton;
    } else {
        return NULL;
    }
}

void cache_release_skeleton(skeleton_t *skeleton) {
    if (skeleton != NULL) {
        int num_skeletons = g_skeletons.size();
        int i;
        for (i = 0; i < num_skeletons; i++) {
            if (g_skeletons[i].skeleton == skeleton) {
                assert(g_skeletons[i].references > 0);
                g_skeletons[i].references -= 1;
                if (g_skeletons[i].references == 0) {
                    skeleton_release(g_skeletons[i].skeleton);
                    g_skeletons[i].skeleton = NULL;
                }
                return;
            }
        }
        assert(0);
    }
}

/* texture related functions */

typedef struct {
    char *filename;
    usage_t usage;
    texture_t *texture;
    int references;
} cached_texture_t;

static std::vector<cached_texture_t> g_textures;

texture_t *cache_acquire_texture(const char *filename, usage_t usage) {
    if (filename != NULL) {
        cached_texture_t cached_texture;
        int num_textures = g_textures.size();
        int i;

        for (i = 0; i < num_textures; i++) {
            if (strcmp(g_textures[i].filename, filename) == 0) {
                if (g_textures[i].usage == usage) {
                    if (g_textures[i].references > 0) {
                        g_textures[i].references += 1;
                    } else {
                        assert(g_textures[i].references == 0);
                        assert(g_textures[i].texture == NULL);
                        g_textures[i].texture = texture_from_file(filename,
                                                                  usage);
                        g_textures[i].references = 1;
                    }
                    return g_textures[i].texture;
                }
            }
        }

        cached_texture.filename = duplicate_string(filename);
        cached_texture.usage = usage;
        cached_texture.texture = texture_from_file(filename, usage);
        cached_texture.references = 1;
        g_textures.push_back(cached_texture);
        return cached_texture.texture;
    } else {
        return NULL;
    }
}

void cache_release_texture(texture_t *texture) {
    if (texture != NULL) {
        int num_textures = g_textures.size();
        int i;
        for (i = 0; i < num_textures; i++) {
            if (g_textures[i].texture == texture) {
                assert(g_textures[i].references > 0);
                g_textures[i].references -= 1;
                if (g_textures[i].references == 0) {
                    texture_release(g_textures[i].texture);
                    g_textures[i].texture = NULL;
                }
                return;
            }
        }
        assert(0);
    }
}

/* skybox related functions */

typedef struct {
    char *skybox_name;
    int blur_level;
    cubemap_t *skybox;
    int references;
} cached_skybox_t;

static std::vector<cached_skybox_t> g_skyboxes;

static cubemap_t *load_skybox(const char *skybox_name, int blur_level) {
    const char *faces[6] = {"px", "nx", "py", "ny", "pz", "nz"};
    char paths[6][PATH_SIZE];
    cubemap_t *skybox;
    int i;

    for (i = 0; i < 6; i++) {
        const char *format;
        if (blur_level == -1) {
            format = "%s/i_%s.hdr";
        } else if (blur_level == 1) {
            format = "%s/m1_%s.hdr";
        } else {
            assert(blur_level == 0);
            format = "%s/m0_%s.hdr";
        }
        sprintf(paths[i], format, skybox_name, faces[i]);
    }
    skybox = cubemap_from_files(paths[0], paths[1], paths[2],
                                paths[3], paths[4], paths[5],
                                USAGE_LDR_COLOR);

    return skybox;
}

static void free_skybox(cubemap_t *skybox) {
    cubemap_release(skybox);
}

cubemap_t *cache_acquire_skybox(const char *skybox_name, int blur_level) {
    if (skybox_name != NULL) {
        cached_skybox_t cached_skybox;
        int num_skyboxes = g_skyboxes.size();
        int i;

        for (i = 0; i < num_skyboxes; i++) {
            if (strcmp(g_skyboxes[i].skybox_name, skybox_name) == 0) {
                if (g_skyboxes[i].blur_level == blur_level) {
                    if (g_skyboxes[i].references > 0) {
                        g_skyboxes[i].references += 1;
                    } else {
                        assert(g_skyboxes[i].skybox == NULL);
                        assert(g_skyboxes[i].references == 0);
                        g_skyboxes[i].skybox = load_skybox(skybox_name,
                                                           blur_level);
                        g_skyboxes[i].references = 1;
                    }
                    return g_skyboxes[i].skybox;
                }
            }
        }

        cached_skybox.skybox_name = duplicate_string(skybox_name);
        cached_skybox.blur_level = blur_level;
        cached_skybox.skybox = load_skybox(skybox_name, blur_level);
        cached_skybox.references = 1;
        g_skyboxes.push_back(cached_skybox);
        return cached_skybox.skybox;
    } else {
        return NULL;
    }
}

void cache_release_skybox(cubemap_t *skybox) {
    if (skybox != NULL) {
        int num_skyboxes = g_skyboxes.size();
        int i;
        for (i = 0; i < num_skyboxes; i++) {
            if (g_skyboxes[i].skybox == skybox) {
                assert(g_skyboxes[i].references > 0);
                g_skyboxes[i].references -= 1;
                if (g_skyboxes[i].references == 0) {
                    free_skybox(g_skyboxes[i].skybox);
                    g_skyboxes[i].skybox = NULL;
                }
                return;
            }
        }
        assert(0);
    }
}

/* ibldata related functions */

typedef struct {
    const char *env_name;
    int mip_levels;
    ibldata_t *ibldata;
    int references;
} cached_ibldata_t;

static cached_ibldata_t g_ibldata[] = {
    {"spruit", 10, NULL, 0},
    {"venice", 10, NULL, 0},
    {"workshop", 10, NULL, 0},
};

static ibldata_t *load_ibldata(const char *env_name, int mip_levels) {
    const char *faces[6] = {"px", "nx", "py", "ny", "pz", "nz"};
    char paths[6][PATH_SIZE];
    ibldata_t *ibldata;
    int i, j;

    ibldata = (ibldata_t*)malloc(sizeof(ibldata_t));
    memset(ibldata, 0, sizeof(ibldata_t));
    ibldata->mip_levels = mip_levels;

    /* diffuse environment map */
    for (j = 0; j < 6; j++) {
        sprintf(paths[j], "%s/i_%s.hdr", env_name, faces[j]);
    }
    ibldata->diffuse_map = cubemap_from_files(paths[0], paths[1], paths[2],
                                              paths[3], paths[4], paths[5],
                                              USAGE_HDR_COLOR);

    /* specular environment maps */
    for (i = 0; i < mip_levels; i++) {
        for (j = 0; j < 6; j++) {
            sprintf(paths[j], "%s/m%d_%s.hdr", env_name, i, faces[j]);
        }
        ibldata->specular_maps[i] = cubemap_from_files(paths[0], paths[1],
                                                       paths[2], paths[3],
                                                       paths[4], paths[5],
                                                       USAGE_HDR_COLOR);
    }

    /* brdf lookup texture */
    ibldata->brdf_lut = cache_acquire_texture("common/brdf_lut.hdr",
                                              USAGE_HDR_DATA);

    return ibldata;
}

static void free_ibldata(ibldata_t *ibldata) {
    int i;
    cubemap_release(ibldata->diffuse_map);
    for (i = 0; i < ibldata->mip_levels; i++) {
        cubemap_release(ibldata->specular_maps[i]);
    }
    cache_release_texture(ibldata->brdf_lut);
    free(ibldata);
}

ibldata_t *cache_acquire_ibldata(const char *env_name) {
    if (env_name != NULL) {
        int num_ibldata = ARRAY_SIZE(g_ibldata);
        int i;
        for (i = 0; i < num_ibldata; i++) {
            if (strcmp(g_ibldata[i].env_name, env_name) == 0) {
                if (g_ibldata[i].references > 0) {
                    g_ibldata[i].references += 1;
                } else {
                    int mip_levels = g_ibldata[i].mip_levels;
                    assert(g_ibldata[i].ibldata == NULL);
                    assert(g_ibldata[i].references == 0);
                    g_ibldata[i].ibldata = load_ibldata(env_name, mip_levels);
                    g_ibldata[i].references = 1;
                }
                return g_ibldata[i].ibldata;
            }
        }
        assert(0);
        return NULL;
    } else {
        return NULL;
    }
}

void cache_release_ibldata(ibldata_t *ibldata) {
    if (ibldata != NULL) {
        int num_ibldata = ARRAY_SIZE(g_ibldata);
        int i;
        for (i = 0; i < num_ibldata; i++) {
            if (g_ibldata[i].ibldata == ibldata) {
                assert(g_ibldata[i].references > 0);
                g_ibldata[i].references -= 1;
                if (g_ibldata[i].references == 0) {
                    free_ibldata(g_ibldata[i].ibldata);
                    g_ibldata[i].ibldata = NULL;
                }
                return;
            }
        }
        assert(0);
    }
}

/* misc cache functions */

void cache_cleanup(void) {
    int num_meshes = g_meshes.size();
    int num_skeletons = g_skeletons.size();
    int num_textures = g_textures.size();
    int num_skyboxes = g_skyboxes.size();
    int num_ibldata = ARRAY_SIZE(g_ibldata);
    int i;
    for (i = 0; i < num_meshes; i++) {
        free(g_meshes[i].filename);
    }
    for (i = 0; i < num_skeletons; i++) {
        free(g_skeletons[i].filename);
    }
    for (i = 0; i < num_textures; i++) {
        free(g_textures[i].filename);
    }
    for (i = 0; i < num_skyboxes; i++) {
        free(g_skyboxes[i].skybox_name);
    }
    for (i = 0; i < num_ibldata; i++) {
    }
    g_meshes.clear();
    g_skeletons.clear();
    g_textures.clear();
    g_skyboxes.clear();
}
