#include <string> // std::string
#include <assert.h> // assert
#include <fstream> // ifstream, read file
#include <iostream> // log
#include <cmath> // sin, cos
#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;





#define MAX_GRID_SIZE 32
#define ORTHO_PROJECTION
#define WIDTH 1920
#define HEIGHT 1080
#define SHADOWMAPSIZE 4096

#include "frame_work/CLightObject.h"
#include "frame_work/CFboManager.h"
#include "frame_work/CTextureManager.h"
#include "frame_work/GBuffer.h"

#include "frame_work/typedefs.h"
#include "frame_work/containers.h"
#include "frame_work/containers.cpp"
#include "frame_work/opengl_stuff.h"
#include "frame_work/geometry.h"
#include "frame_work/camera.h"
#include "frame_work/scene.h"
#include "frame_work/assets.h"
#include "frame_work/renderer.h"
#include "frame_work/Demo.h"
#include "frame_work/app.h"

#include "frame_work/BDemo.h"

#include "frame_work/scene.cpp"
#include "frame_work/camera.cpp"
#include "frame_work/opengl_stuff.cpp"
#include "frame_work/renderer.cpp"
#include "frame_work/assets.cpp"
#include "frame_work/app.cpp"
#include "frame_work/CLightObject.cpp"
#include "frame_work/CFboManager.cpp"
#include "frame_work/CTextureManager.cpp"
#include "frame_work/GBuffer.cpp"



#include "base_demo/BaseDemo.h"
#include "base_demo/BaseDemo.cpp"

//#include "basic_demo/BasicDemo.h"
//#include "basic_demo/BasicDemo.cpp"

//#include "lpv_demo/LpvDemo.h"
//#include "lpv_demo/LpvDemo.cpp"

int main(int argc, const char* argv[])
{
	LOG("main", "starting application");
	BaseDemo demo;
	if (application::init(demo)) {
		application::run(demo);
	}
	application::deinit(demo);
	LOG("main", "exiting application");
	return 0;
}