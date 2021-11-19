struct Scene
{
	const char*                    name = "";
	float                          scale = 1.0f;
	Bounding_Box                   bounding_box;
	Array<Model*>                  models; // note: model memory owned by Assets.
};

struct Scene_Config
{
    const char* name = "";
	const char* obj_file_path = "";
	const char* res_file_path = "";
	float scale = 1.0f;
};


namespace scene
{
    Scene& get_scene();
	void init(Scene& scene, const Scene_Config& config);
	void uninit(Scene& scene);
}

namespace scenes
{
	void init();
	void init(const char* selectedSceneName);
	void uninit();
}
