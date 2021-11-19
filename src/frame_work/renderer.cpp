#include <functional>

namespace renderer
{
    Renderer& get_renderer() {
        static Renderer renderer;
        return renderer;
    }

    void init(GLFWwindow* window)
    {
        LOG("renderer", "initializing");
        Renderer& renderer = get_renderer();

        int window_w, window_h;
        glfwGetWindowSize(window, &window_w, &window_h);
		// fbos
		Resolution& resolution = resolution::get();
		framebuffer::init_with_depth(renderer.main_fbo, resolution.internal.x, resolution.internal.y);
		check_gl_error();
		camera::set_to_perspective(renderer.fps_camera, resolution.internalAspectRatio);
		glfwSetFramebufferSizeCallback(window, glfw_framebuffer_size_callback);
		check_gl_error();
	}

	void uninit()
	{
		LOG("renderer", "destroying");
		Renderer& renderer = get_renderer();
		framebuffer::uninit(renderer.main_fbo);
	}


    void render_texture_to_main_fbo(Shader_Program& shaderProgram,std::string uniform_tex_name,GLuint tex_id ){
        Renderer& renderer = get_renderer();
        glBindFramebuffer(GL_FRAMEBUFFER, renderer.main_fbo.fbo_id);
        GLuint shader_id = shader::activate(shaderProgram);
        glViewport(0, 0, resolution::get().internal.x, resolution::get().internal.y);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUniform1i(glGetUniformLocation(shader_id, uniform_tex_name.c_str()), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex_id);
        draw_simple_mesh(shader_id, assets::get_unit_quad());
        shader::deactivate();
    }
    void render_to_main_fbo(Scene& scene,std::function<void(void)> proc){
        Renderer& renderer = get_renderer();
        Resolution& resolution = resolution::get();
        GLuint fboID = renderer.main_fbo.fbo_id;
        static const GLfloat bg_color[] = {1., 1., 1., 1.0};
        glBindFramebuffer(GL_FRAMEBUFFER, fboID);
        glViewport(0, 0, resolution.internal.x, resolution.internal.y);
        glClearBufferfv(GL_COLOR, 0, bg_color);
        glClear(GL_DEPTH_BUFFER_BIT);
        check_gl_error();
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        proc();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        check_gl_error();
    }
    void render_fbo_to_window(){
        // blit main fbo to actual window
        {
            Renderer& renderer = get_renderer();
            Resolution& resolution = resolution::get();
            static const GLfloat bg_color[] = { 1., 1., 1., 1.0 };
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, resolution.window.x, resolution.window.y);
            glClearBufferfv(GL_COLOR, 0, bg_color);
            glClear(GL_DEPTH_BUFFER_BIT);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.main_fbo.fbo_id);
            glReadBuffer(GL_COLOR_ATTACHMENT0);

            glBlitFramebuffer(
                    0, 0, resolution.internal.x, resolution.internal.y,
                    resolution.position0.x, resolution.position0.y,
                    resolution.position1.x, resolution.position1.y,
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glReadBuffer(0);
            check_gl_error();
        }
    }
	Camera& get_camera() {
		return get_renderer().fps_camera;
	}
}

namespace renderer
{


	void upload_camera(GLuint shader_id, Camera& camera)
	{
		glUniformMatrix4fv(glGetUniformLocation(shader_id, "VP"), 1, GL_FALSE, glm::value_ptr(camera.VP));
	}

    void upload_material(GLuint shader_id, Material& material, int texture_location_offset)
	{
		using glm::value_ptr;
		
		glUniform3fv(glGetUniformLocation(shader_id, "u_material.Ka"), 1, value_ptr(material.Ka));
		glUniform3fv(glGetUniformLocation(shader_id, "u_material.Kd"), 1, value_ptr(material.Kd));
		glUniform3fv(glGetUniformLocation(shader_id, "u_material.Ks"), 1, value_ptr(material.Ks));
		glUniform1f(glGetUniformLocation(shader_id, "u_material.Ns"), material.Ns);
		glUniform3fv(glGetUniformLocation(shader_id, "u_material.Ke"), 1, value_ptr(material.Ke));
		glUniform1f(glGetUniformLocation(shader_id, "u_material.d"), material.d);
		glUniform1f(glGetUniformLocation(shader_id, "u_material.Ni"), material.Ni);
		glUniform3fv(glGetUniformLocation(shader_id, "u_material.Tf"), 1, value_ptr(material.Tf));

		texture::activate(*material.map_Ka,   shader_id, "u_tex_ambient", texture_location_offset + 0);
		texture::activate(*material.map_Kd,   shader_id, "u_tex_diffuse", texture_location_offset + 1);
		texture::activate(*material.map_Ks,   shader_id, "u_tex_specular", texture_location_offset + 2);
		texture::activate(*material.map_Ke,   shader_id, "u_tex_emission", texture_location_offset + 3);
		texture::activate(*material.map_bump, shader_id, "u_tex_bumpmap", texture_location_offset + 4);
	}

	void draw_models_with_materials(GLuint shader_id, Scene& scene, int texture_location_offset)
	{
		for (Model* model : scene.models) {
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "M"), 1, GL_FALSE, glm::value_ptr(model->transform.mtx));
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "N"), 1, GL_FALSE, glm::value_ptr(model->transform.normal_mtx));

			for (Mesh* mesh : model->meshes) {
				glBindVertexArray(mesh->vao);
				for (Sub_Mesh& sub_mesh : mesh->sub_meshes) { // btw, usually n = 1 here, only a few models have more than 1 material per mesh.
					upload_material(shader_id, assets::get_material(sub_mesh.material_index), texture_location_offset);
					glDrawArrays(GL_TRIANGLES, sub_mesh.index, sub_mesh.length);
				}
			}
		}
	}

	void draw_models_with_albedo(GLuint shader_id, Scene& scene, int texture_location_offset)
	{
		for (Model* model : scene.models) {
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "M"), 1, GL_FALSE, glm::value_ptr(model->transform.mtx));
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "N"), 1, GL_FALSE, glm::value_ptr(model->transform.normal_mtx));

			for (Mesh* mesh : model->meshes) {
				glBindVertexArray(mesh->vao);

				for (Sub_Mesh& sub_mesh : mesh->sub_meshes) {
					Material& material = assets::get_material(sub_mesh.material_index);
					glUniform3fv(glGetUniformLocation(shader_id, "u_material.Ka"), 1, value_ptr(material.Ka));
					glUniform3fv(glGetUniformLocation(shader_id, "u_material.Kd"), 1, value_ptr(material.Kd));
					glUniform3fv(glGetUniformLocation(shader_id, "u_material.Ke"), 1, value_ptr(material.Ke));
					texture::activate(*material.map_Ka, shader_id, "u_tex_ambient", texture_location_offset + 0);
					texture::activate(*material.map_Kd, shader_id, "u_tex_diffuse", texture_location_offset + 1);
					texture::activate(*material.map_Kd, shader_id, "u_tex_emission", texture_location_offset + 2);
					glDrawArrays(GL_TRIANGLES, sub_mesh.index, sub_mesh.length);
				}
			}
		}
	}

	void draw_models_without_materials(GLuint shader_id, Scene& scene)
	{
		for (Model* model : scene.models) {
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "M"), 1, GL_FALSE, glm::value_ptr(model->transform.mtx));
			glUniformMatrix4fv(glGetUniformLocation(shader_id, "N"), 1, GL_FALSE, glm::value_ptr(model->transform.normal_mtx));

			for (Mesh* mesh : model->meshes) {
				glBindVertexArray(mesh->vao);

				for (Sub_Mesh& sub_mesh : mesh->sub_meshes) // @Speed: store the length of the whole vertex buffer so no need to iterate here
					glDrawArrays(GL_TRIANGLES, sub_mesh.index, sub_mesh.length);
			}
		}
	}

    void draw_simple_mesh(GLuint shader_id, Mesh& mesh)
    {
        glBindVertexArray(mesh.vao);

        static mat4 im = mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader_id, "M"), 1, GL_FALSE, glm::value_ptr(im));

        for (Sub_Mesh& sub_mesh : mesh.sub_meshes)
            glDrawArrays(GL_TRIANGLES, sub_mesh.index, sub_mesh.length);

        glBindVertexArray(0);
    }

	void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		resolution::window_size_changed(width, height);
	}
}