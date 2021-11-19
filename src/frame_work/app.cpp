namespace
{
	bool create_window(const Application_Config& config);
	void destroy_window();
	void GLFW_error_callback(int error, const char* description);
}

namespace application
{
	bool init(Demo& demo ,const vec2& internal_render_resolution) {
		LOG("app", "initializing");
		Application& app = get_app();
		Application_Config default_config;
		if (!create_window(default_config))
			return false;
        glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		resolution::set(default_config.window_resolution, internal_render_resolution);
		resolution::scale_with_black_bars();
		renderer::init(app.window);
		demo.init();
		flycamera::attach(app.camera_controls, &renderer::get_camera());
        assets::init();
        scenes::init();
		return true;
	}

	void deinit(Demo& demo) {
		LOG("app", "uninitializing");
		scenes::uninit();
		assets::uninit();
		renderer::uninit();
        demo.uninit();
		destroy_window();
	}

	void run(Demo& demo)
	{
		LOG("app", "starting loop");
		Application& app = get_app();
		bool is_exit_queued = false;
		while (!glfwWindowShouldClose(app.window) && !is_exit_queued)
		{
			float dt = 1.0f / 60.0f; // TODO

			if (glfwGetKey(app.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				is_exit_queued = true;

            flycamera::update(app.camera_controls, app.window, dt);
            demo.update();
			glfwSwapBuffers(app.window);
			glfwPollEvents();
		}

		LOG("app", "ending loop");
	}

	std::string read_file(std::string filepath)
	{
		std::ifstream file(filepath);
		std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if(result==""){
		    std::cout<<"read file "+filepath+ " error"<<std::endl;
            exit(-1);
        }
		file.close();
		return result;
	}
}

namespace resolution
{
	Resolution& get()
	{
		static Resolution resolution;
		return resolution;
	}
	void set(vec2 windowResolution, vec2 internalRenderResolution)
	{
		Resolution& resolution = resolution::get();

		resolution.window = windowResolution;
		resolution.internal = internalRenderResolution;
		resolution.windowAspectRatio = resolution.window.x / resolution.window.y;
		resolution.internalAspectRatio = resolution.internal.x / resolution.internal.y;
		resolution.position0 = { 0.0f, 0.0f };
		resolution.position1 = { 0.0f, 0.0f };

		LOG("resolution", "updated!");
		LOG("resolution", "window  : %.0fx%.0f (%f)", resolution.window.x,resolution.window.y,resolution.windowAspectRatio);
		LOG("resolution", "internal: %.0fx%.0f (%f)", resolution.internal.x,resolution.internal.y,resolution.internalAspectRatio);
	}
	void scale_with_black_bars()
	{
		// TODO this is buggy :D

		Resolution& resolution = resolution::get();

		bool doResolutionsMatch = (resolution.internal.x == resolution.window.x && resolution.internal.y == resolution.window.y);
		if (doResolutionsMatch) {
			resolution.position0 = { 0.0f, 0.0f };
			resolution.position1 = resolution.internal;
			return;
		}

		// set the viewport depending on the screen resolution
		float currentWidth = resolution.window.x;
		float currentHeight = resolution.window.y;

		float targetAspectRatio = resolution.internal.x / resolution.internal.y;

		// figure out the largest area that fits in this resolution at the desired aspect ratio
		float width = currentWidth;
		float height = width / targetAspectRatio + 0.5f;

		if (height > currentHeight) {
			// it doesn't fit our height, we must switch to pillarbox then
			height = currentHeight;
			width = height * targetAspectRatio + 0.5f;
		}

		// set up the new viewport centered in the backbuffer
		resolution.position0.x = 0.5f * (currentWidth - width);
		resolution.position0.y = 0.5f * (currentHeight - height);
		resolution.position1.x = width;
		resolution.position1.y = height;

		LOG("resolution", "render from %.0fx%.0f %.0fx%.0f", resolution.position0.x,resolution.position0.y, resolution.position1.x,resolution.position1.y);
	}
	void window_size_changed(int width, int height)
	{
		Resolution& resolution = resolution::get();
		resolution.window.x = width;
		resolution.window.y = height;
		resolution.windowAspectRatio = resolution.window.x / resolution.window.y;
		resolution::scale_with_black_bars();
	}
}

namespace
{
	bool create_window(const Application_Config& config)
	{
		LOG("app", "initializing opengl context");
		Application& app = application::get_app();

		// init GLFW
		{
			if (!glfwInit()) {
				LOG("app", "error in GLFW initialization");
				return false;
			} else {
				LOG("app", "glfw inited (version %s)", glfwGetVersionString());
			}
		}

		// init OpenGL profile
		{
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // GLFW_OPENGL_COMPAT_PROFILE); 
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.gl_major_version);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.gl_minor_version);
			glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE); // note: disabled for windowed mode windows
			glfwWindowHint(GLFW_SAMPLES, config.msaa_samples);
		}

		// init window
		{
			switch (config.window_mode)
			{
				case Application_Config::Window_Mode::WINDOWED_FULLSCREEN:
				{
					LOG("app", "creating windowed fullscreen");

					GLFWmonitor *monitor = glfwGetPrimaryMonitor();
					const GLFWvidmode *mode = glfwGetVideoMode(monitor);

					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

					app.window = glfwCreateWindow(mode->width, mode->height, config.window_title, monitor, NULL);
				}
				break;

				case Application_Config::Window_Mode::WINDOWED:
				{
					app.window = glfwCreateWindow(config.window_resolution.x, config.window_resolution.y, config.window_title, NULL, NULL);
				}
				break;
			}

			if (!app.window) {
				LOG("app", "error in window creation");
				glfwTerminate();
				return false;
			} else {
				LOG("app", "window created");
			}

			glfwMakeContextCurrent(app.window);
			glfwSwapInterval(config.vsync_mode);
		}

		// init GLEW
		{
			glewExperimental = GL_TRUE; 
			GLenum err = glewInit();

			if (err != GLEW_OK) {
				LOG("app", "error in GLEW initialization: %s", glewGetErrorString(err));
				glfwTerminate();
			} else {
				LOG("app", "glew inited (version %s)", glewGetString(GLEW_VERSION));
			}
		}

		// init OpenGL settings
		{
			//glEnable(GL_TEXTURE_3D);
			glEnable(GL_MULTISAMPLE); 
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			check_gl_error();

			LOG("app", "OpenGL version %s, GLSL version %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
		}


		{
			glfwSetErrorCallback(GLFW_error_callback);
		}

		LOG("app", "opengl initialization ready");
		return true;
	}

	void destroy_window()
	{
		Application& app = application::get_app();
		LOG("app", "terminating glfw");
		glfwDestroyWindow(app.window);
		glfwTerminate();
	}

	void GLFW_error_callback(int error, const char* description)
	{
		LOG("app", "glfw error %d: %s", error, description);
	}
}
