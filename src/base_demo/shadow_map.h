//
// Created by gangan on 2021/11/13.
//

#ifndef VXGI_SHADOW_MAP_H
#define VXGI_SHADOW_MAP_H


struct Shadow_Map
{
    struct Config {
        int resolution;
        float ortho;
        float near_plane;
        float far_plane;
    };
    Config config;
    mat4 view = mat4(1.0);
    mat4 projection = mat4(1.0);
    mat4 VP = mat4(1.0);
    mat4 VP_biased = mat4(1.0);
    bool   is_fbo_created = false;
    GLuint depth_fbo_id = 0;
    GLuint depth_texture_id = 0;
};

namespace shadowmap
{
    void init(Shadow_Map& s, const Shadow_Map::Config& config);
    void uninit(Shadow_Map& s);
    void update(Shadow_Map& s, const vec3& light_direction);
    void fbo_activate(Shadow_Map& s);
    void fbo_deactivate(Shadow_Map& s);
    void texture_activate(Shadow_Map& s, GLuint shader_id, const char* sampler_name, int texture_location_offset = 0);
    void texture_deactivate(Shadow_Map& s);
}

#endif //VXGI_SHADOW_MAP_H
