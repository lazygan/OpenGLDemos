struct Renderer
{
	Camera fps_camera;
	Frame_Buffer main_fbo;
};


namespace renderer
{
    Renderer& get_renderer();
    void init(GLFWwindow* window);
    void uninit();
    Camera& get_camera(); // @Cleanup should be in app or scene
	void upload_camera(GLuint shader_id, Camera& camera);
	void upload_material(GLuint shader_id, Material& material, int texture_location_offset = 0);
	void draw_models_with_materials(GLuint shader_id, Scene& scene, int texture_location_offset = 0);
	void draw_models_with_albedo(GLuint shader_id, Scene& scene, int texture_location_offset);
    void draw_simple_mesh(GLuint shader_id, Mesh& mesh);
    void draw_models_without_materials(GLuint shader_id, Scene& scene);
	void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height);
}