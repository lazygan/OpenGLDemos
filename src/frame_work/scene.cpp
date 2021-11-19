namespace
{
	static const Scene_Config cornell_config =
	{
		.name                      = "cornell",
		.obj_file_path             = "cornell.obj",
		.res_file_path             = "",
		.scale                     = 1.0f
	};

	static const Scene_Config sponza_config =
	{
		.name                      = "Crytek Sponza",
		.obj_file_path             = "sponza.obj",
		.res_file_path             = "sponzatextures/",
		.scale                     = 0.01f
	};
}

namespace scene
{
    Scene& get_scene() {
    	static Scene s;
    	return s;
    }
	void init(Scene& scene, const Scene_Config& config)
	{
		scene.name = config.name;
		assets::load(scene.models, scene.bounding_box, config.obj_file_path, config.res_file_path);

		//lights::add_directional_light(scene.light_manager, config.sun_direction, config.sun_color, config.sun_attenuation, config.sun_strength, config.shadow_map_config);

		scene.scale = config.scale;
		scene.bounding_box.min_point *= config.scale;
		scene.bounding_box.max_point *= config.scale;
		boundingbox::update(scene.bounding_box);

		// make sure scene center point is at 0,0,0 so that all of it fits into the voxel grid
		int i = 0 ;
		for (Model* model : scene.models) {
			model->transform.scale = vec3(config.scale);
			model->transform.position = -scene.bounding_box.center;
			transform::update(model->transform);
		}
		// 2.0f = NDC is [-1, 1] so abs(1 - -1) = 2.0f
		const float offset = 0.1f; // small offset so that a vertex at bounds (1,1,1) will be voxelized aswell
	}
	void uninit(Scene& scene)
	{
		// TODO
	}
}

namespace scenes
{
	void init()
	{
        scene::init(scene::get_scene(),sponza_config);
	}
	void uninit()
	{
		scene::uninit(scene::get_scene());
	}
}

