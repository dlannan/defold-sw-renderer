// myextension.cpp
// Extension lib defines
#define LIB_NAME "Renderer"
#define MODULE_NAME "renderer"

// include the Defold SDK
#include <dmsdk/sdk.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <time.h>

#include "core/api.h"
#include "shaders/cache_helper.h"
#include "tests/test_blinn.h"
#include "tests/test_pbr.h"
#include "tests/test_helper.h"

static std::string      scenename = "";
static std::string      scenefilename = "";
static std::string      assetpath = "";
static scene_t *        mainscene = nullptr;
static userdata_t       userdata;
static scene_info_t     sceneinfo;
static tickfunc_t       * tickfunc = &tick_function;

extern texture_t *acquire_label_texture(const char *filename);

static void GetTableNumbersFloat( lua_State * L, int tblidx, float *data )
{
    // Iterate indices and set float buffer with correct lookups
    lua_pushnil(L);
    size_t idx = 0;
    // Build a number array matching the buffer. They are all assumed to be type float (for the time being)
    while( lua_next( L, tblidx ) != 0) {
        data[idx++] = lua_tonumber( L, -1 );
        lua_pop( L, 1 );
    }
}

static int SetTestScene(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    scenename = std::string(luaL_checkstring(L, 1));
    scenefilename = std::string(luaL_checkstring(L, 2));
    assetpath = std::string(luaL_checkstring(L, 3));
    mainscene = test_create_scene(scenename.c_str(), scenefilename.c_str(), assetpath.c_str());
    return 0;
}

static int InitRenderLoop(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    int width = luaL_checknumber(L, 1);
    int height = luaL_checknumber(L, 2);
       
    userdata.scene = mainscene;
    userdata.layer = -1;
    userdata.labels[0] = acquire_label_texture("common/diffuse.tga");
    userdata.labels[1] = acquire_label_texture("common/specular.tga");
    userdata.labels[2] = acquire_label_texture("common/roughness.tga");
    userdata.labels[3] = acquire_label_texture("common/occlusion.tga");
    userdata.labels[4] = acquire_label_texture("common/normal.tga");    
    sceneinfo = test_init_mainloop(width, height);

    return 0;
}

static int ReleaseRenderLoop(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    test_release_mainloop(sceneinfo);
    cache_release_texture(userdata.labels[0]);
    cache_release_texture(userdata.labels[1]);
    cache_release_texture(userdata.labels[2]);
    cache_release_texture(userdata.labels[3]);
    cache_release_texture(userdata.labels[4]);
    return 0;
}

static int UpdateRenderLoop(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    test_run_mainloop(sceneinfo, tickfunc, &userdata);
    return 0;
}

static int UpdateCamera(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);

    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);

    vec3_t pos;
    GetTableNumbersFloat(L, 1, (float *)&pos);

    vec3_t target;
    GetTableNumbersFloat(L, 2, (float *)&target);
        
    test_update_camera(sceneinfo, pos, target);
    return 0;
}

static int UpdateLight(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);
    float theta = luaL_checknumber(L, 1);
    float phi = luaL_checknumber(L, 2);
        
    test_update_light(sceneinfo, theta, phi); 
    return 0;
}

// Fetch frame buffer and set it to the texture in use
static int SetFrameBufferBytes(lua_State* L)
{
    DM_LUA_STACK_CHECK(L, 0);
    dmScript::LuaHBuffer *buffer = dmScript::CheckBuffer(L, 1);
    const char *streamname = luaL_checkstring(L, 2);

    uint8_t* bytes = 0x0;
    uint32_t size = 0;
    uint32_t count = 0;
    uint32_t components = 0;
    uint32_t stride = 0;
    dmBuffer::Result r = dmBuffer::GetStream(buffer->m_Buffer, dmHashString64(streamname), (void**)&bytes, &count, &components, &stride);

    size_t idx = 0;
    const unsigned char *fb = sceneinfo.framebuffer->color_buffer;
    if (r == dmBuffer::RESULT_OK) {
        for (int i = 0; i < count; ++i)
        {
            // for (int c = 0; c < components; ++c)
            // {
            //     bytes[c] = fb[idx++];
            // }
            bytes[0] = fb[idx]; 
            bytes[1] = fb[idx+1]; 
            bytes[2] = fb[idx+2]; 
            bytes[3] = 0xff;
            bytes += stride;
            idx += stride;
        }
    } else {
        // handle error
    }

    r = dmBuffer::ValidateBuffer(buffer->m_Buffer);
    return 0;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] =
{
    {"settestscene", SetTestScene},
    {"initrenderloop", InitRenderLoop},
    {"releaserenderloop", ReleaseRenderLoop},
    {"updatecamera", UpdateCamera},
    {"updatelight", UpdateLight},
    {"updaterenderloop", UpdateRenderLoop},
    {"setframebuffer", SetFrameBufferBytes},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);

    // Register lua names
    luaL_register(L, MODULE_NAME, Module_methods);

    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeRenderer(dmExtension::AppParams* params)
{
    dmLogInfo("AppInitializeRenderer\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeRenderer(dmExtension::Params* params)
{
    // Init Lua
    LuaInit(params->m_L);
    dmLogInfo("Registered %s Extension\n", MODULE_NAME);

    srand((unsigned int)time(NULL));

    return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeRenderer(dmExtension::AppParams* params)
{
    dmLogInfo("AppFinalizeRenderer\n");
    return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeRenderer(dmExtension::Params* params)
{
    dmLogInfo("FinalizeRenderer\n");

    cache_cleanup();

    return dmExtension::RESULT_OK;
}

dmExtension::Result OnUpdateRenderer(dmExtension::Params* params)
{
    // dmLogInfo("OnUpdateRenderer\n");
    return dmExtension::RESULT_OK;
}

void OnEventRenderer(dmExtension::Params* params, const dmExtension::Event* event)
{
    switch(event->m_Event)
    {
        case dmExtension::EVENT_ID_ACTIVATEAPP:
            dmLogInfo("OnEventRenderer - EVENT_ID_ACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_DEACTIVATEAPP:
            dmLogInfo("OnEventRenderer - EVENT_ID_DEACTIVATEAPP\n");
            break;
        case dmExtension::EVENT_ID_ICONIFYAPP:
            dmLogInfo("OnEventRenderer - EVENT_ID_ICONIFYAPP\n");
            break;
        case dmExtension::EVENT_ID_DEICONIFYAPP:
            dmLogInfo("OnEventRenderer - EVENT_ID_DEICONIFYAPP\n");
            break;
        default:
            dmLogWarning("OnEventRenderer - Unknown event id\n");
            break;
    }
}

// Defold SDK uses a macro for setting up extension entry points:
//
// DM_DECLARE_EXTENSION(symbol, name, app_init, app_final, init, update, on_event, final)

// Renderer is the C++ symbol that holds all relevant extension data.
// It must match the name field in the `ext.manifest`
DM_DECLARE_EXTENSION(Renderer, LIB_NAME, AppInitializeRenderer, AppFinalizeRenderer, InitializeRenderer, OnUpdateRenderer, OnEventRenderer, FinalizeRenderer)
