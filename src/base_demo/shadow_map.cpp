//
// Created by gangan on 2021/11/13.
//

namespace shadowmap
{
    void init(Shadow_Map& s, const Shadow_Map::Config& config)
    {
        LOG("shadowmap", "initializing");
        assert(!s.is_fbo_created);
        s.config = config;

        glGenFramebuffers(1, &s.depth_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, s.depth_fbo_id);

        glGenTextures(1, &s.depth_texture_id);
        glBindTexture(GL_TEXTURE_2D, s.depth_texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, s.config.resolution, s.config.resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, s.depth_texture_id, 0);
        glDrawBuffer(GL_NONE); // dont need color buffer

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            ASSERT(false, "shadowmap", "couldn't create FBO");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        s.is_fbo_created = true;

        check_gl_error();
    }

    void uninit(Shadow_Map& s)
    {
        if (s.is_fbo_created) {
            LOG("shadowmap", "destroying");
            glDeleteTextures(1, &s.depth_texture_id);
            glDeleteFramebuffers(1, &s.depth_fbo_id);
        }
    }

    void update(Shadow_Map& s, const vec3& light_direction){
        static mat4 bias = mat4({
                                        0.5, 0.0, 0.0, 0.0,
                                        0.0, 0.5, 0.0, 0.0,
                                        0.0, 0.0, 0.5, 0.0,
                                        0.5, 0.5, 0.5, 1.0 });
        s.view = glm::lookAt(light_direction, vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
        s.projection = glm::ortho(-s.config.ortho,s.config.ortho,-s.config.ortho,s.config.ortho, s.config.near_plane,s.config.far_plane);
        s.VP = s.projection * s.view;
        s.VP_biased = bias * s.VP;
    }

    // TODO!!! move these shaibas to fbo:: and texture::
    void fbo_activate(Shadow_Map& s)
    {
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, s.depth_fbo_id);
        glViewport(0,0, s.config.resolution, s.config.resolution);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void fbo_deactivate(Shadow_Map& s)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void texture_activate(Shadow_Map& s, GLuint shader_id, const char* sampler_name, int texture_location_offset)
    {
        glUniform1i(glGetUniformLocation(shader_id, sampler_name), texture_location_offset);
        glActiveTexture(GL_TEXTURE0 + texture_location_offset);
        glBindTexture(GL_TEXTURE_2D, s.depth_texture_id);
    }

    void texture_deactivate(Shadow_Map& s)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
