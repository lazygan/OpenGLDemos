#version 450 core
#define MAX_DIRECTIONAL_LIGHTS 4

struct Directional_Light
{
    float strength;
    vec3 direction;
    vec3 color;
    vec3 attenuation;
};

struct Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Ns;
    vec3 Ke;
    float d;
    float Ni;
    vec3 Tf;
};

uniform	vec3 u_ambient_light;
uniform int u_total_directional_lights;
uniform Directional_Light u_directional_lights[MAX_DIRECTIONAL_LIGHTS];

uniform Material u_material;
uniform sampler2D u_tex_ambient;
uniform sampler2D u_tex_diffuse;
uniform sampler2D u_tex_specular;
uniform sampler2D u_tex_emission;
uniform sampler2D u_tex_bumpmap;

in vec3 f_world_pos;
in vec2 f_tex_coord;
in vec3 f_normal;
out vec4 o_color;

uniform mat4 u_shadowmap_mvp;
//uniform sampler2DShadow u_tex_shadowmap;
uniform sampler2D u_tex_shadowmap;


float calc_visibility(vec4 f_shadow_coord) {
    vec3 projCoords = f_shadow_coord.xyz/f_shadow_coord.w;
    float closestDepth = texture(u_tex_shadowmap,projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    return currentDepth - bias < closestDepth  ? 1.0 : 0.0;
}

void main()
{

    vec4 f_shadow_coord = u_shadowmap_mvp * vec4(f_world_pos, 1.0f);
    float f_visibility = calc_visibility(f_shadow_coord);

    for (int i=0; i < u_total_directional_lights; i++) {
        Directional_Light light = u_directional_lights[i];
        vec3 diff_color = u_material.Kd * (light.strength * light.color * max(0.0f,dot(light.direction , f_normal))  + u_ambient_light);
        o_color += f_visibility *vec4(diff_color, 1.0) * texture(u_tex_diffuse, f_tex_coord);
    }

}