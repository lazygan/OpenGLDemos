//
// Created by gangan on 2021/11/13.
//
namespace{
    Light_Manager light_manager;

    Shader_Program basic_shader;
    Shader_Program mesh_shader;
    Shader_Program shadowmap_shader;
    Shader_Program shadowmap_visualizer_shader;

    void upload_shadowmap(GLuint shader_id, int texture_location_offset) // @Note: only 1 supported atm
    {
        if (::array::size(light_manager.directional_lights) > 0) {
            Directional_Light& light = light_manager.directional_lights[0];
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "u_shadowmap_mvp"), 1, GL_FALSE, glm::value_ptr(light.shadow_map.VP_biased));
            shadowmap::texture_activate(light.shadow_map, shader_id, "u_tex_shadowmap", texture_location_offset);
        } else {
            // no shadowmap -- throw in a white texture instead so everything will be visible.
            static mat4 identity_matrix = mat4(1.0f);
            glUniformMatrix4fv(glGetUniformLocation(shader_id, "u_shadowmap_mvp"), 1, GL_FALSE, glm::value_ptr(identity_matrix));
            texture::activate(assets::get_white_texture(), shader_id, "u_tex_shadowmap", texture_location_offset);
        }
    }


    void upload_lights(GLuint shader_id)
    {
        glUniform3fv(glGetUniformLocation(shader_id, "u_ambient_light"), 1, glm::value_ptr(light_manager.ambient_light));
        glUniform1i(glGetUniformLocation(shader_id, "u_total_directional_lights"), ::array::size(light_manager.directional_lights));
        int index = 0;
        for (Directional_Light& p : light_manager.directional_lights) {
            // @Todo @Speed @Cleanup string allocations
            glUniform1f(glGetUniformLocation(shader_id,  ("u_directional_lights[" + std::to_string(index) + "].strength").c_str()), p.strength);
            glUniform3fv(glGetUniformLocation(shader_id, ("u_directional_lights[" + std::to_string(index) + "].direction").c_str()), 1, glm::value_ptr(p.direction));
            glUniform3fv(glGetUniformLocation(shader_id, ("u_directional_lights[" + std::to_string(index) + "].attenuation").c_str()), 1, glm::value_ptr(p.attenuation));
            glUniform3fv(glGetUniformLocation(shader_id, ("u_directional_lights[" + std::to_string(index) + "].color").c_str()), 1, glm::value_ptr(p.color));
            index++;
        }
    }

    void render_shadowmaps(Scene& scene, GLuint mainFboId)
    {
        check_gl_error();
        glEnable(GL_DEPTH_TEST);
        GLuint shader_id = shader::activate(shadowmap_shader);
        for (Directional_Light& light : light_manager.directional_lights)
        {
            if (light.is_dirty)
            {
                light.is_dirty = false;
                glUniformMatrix4fv(glGetUniformLocation(shader_id, "VP_shadow"), 1, GL_FALSE, glm::value_ptr(light.shadow_map.VP));
                shadowmap::fbo_activate(light.shadow_map);
                renderer::draw_models_without_materials(shader_id, scene);
                shadowmap::fbo_deactivate(light.shadow_map);
            }
        }
        shader::deactivate();
        glBindFramebuffer(GL_FRAMEBUFFER, mainFboId);
    }
};

void BaseDemo::init(){
        std::string shader_root = "../../src/shaders";
        // load shaders first (early exit if some of them doesn't compile)
        shader::init(basic_shader, "basic_shader", shader_root+"/basic_shader_vert.glsl", shader_root+"/basic_shader_frag.glsl");
        shader::init(mesh_shader, "mesh_shader", shader_root+"/mesh_vert.glsl", shader_root+"/mesh_frag.glsl");
        shader::init(shadowmap_shader, "shadow_map_shader", shader_root+"/shadowmap.vert", shader_root+"/shadowmap.frag");
        shader::init(shadowmap_visualizer_shader, "shadowmap_visualizer_shader", shader_root+"/depth_tex_viewer.vert", shader_root+"/depth_tex_viewer.frag");

        glm::vec3 sun_direction= vec3(0.061f, 0.242f, 0.05f);
        glm::vec3 sun_color = vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 sun_attenuation=vec3(1.0f, 1.0f, 1.0f);
        float sun_strength = 3.0f;
        Shadow_Map::Config shadow_map_config = { 8192, 20.0f, -20.0f, 25.0f };
        lights::add_directional_light(light_manager,sun_direction,sun_color,sun_attenuation,sun_strength,shadow_map_config);
        check_gl_error();
    }
 void BaseDemo::uninit(){
        shader::deinit(basic_shader);
        shader::deinit(mesh_shader);
        shader::deinit(shadowmap_shader);
        shader::deinit(shadowmap_visualizer_shader);
 }
void BaseDemo::update(){
        Renderer& renderer = renderer::get_renderer();
        Scene& scene  = scene::get_scene();
        render_shadowmaps(scene, renderer.main_fbo.fbo_id);
        //render_texture_to_main_fbo(get_shaders().shadowmap_visualizer_shader,"u_tex_shadowmap",scene.light_manager.directional_lights[0].shadow_map.depth_texture_id);
        check_gl_error();

        renderer::render_to_main_fbo(
                scene,
                [&](){
                    //GLuint shader_id = shader::activate(get_shaders().mesh_shader);
                    GLuint shader_id = shader::activate(basic_shader);
                    check_gl_error();
                    renderer::upload_camera(shader_id, renderer.fps_camera);
                    upload_lights(shader_id);
                    upload_shadowmap(shader_id, 0);
                    renderer::draw_models_with_materials(shader_id, scene,1);
                    shader::deactivate();
                });
        renderer::render_fbo_to_window();
        check_gl_error();
}