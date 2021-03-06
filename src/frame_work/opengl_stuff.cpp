void   _check_gl_error(const char* file, int line);
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

namespace texture
{
	void init(Texture2D& t, const void* data, int w, int h, GLint internalFormat, GLenum format, GLenum type, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT, bool generateMipmaps, bool attachToFrameBuffer, GLenum fboAttachment, GLuint fboAttachmentLevel)
	{
		t.width = w;
		t.height = h;

		glGenTextures(1, &t.id);
		glBindTexture(GL_TEXTURE_2D, t.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS); // https://open.gl/textures
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w,h, 0, format, type, data);

		if (generateMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		if (attachToFrameBuffer)
			glFramebufferTexture2D(GL_FRAMEBUFFER, fboAttachment, GL_TEXTURE_2D, t.id, fboAttachmentLevel);

		check_gl_error();
		t.is_loaded = true;

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void uninit(Texture2D& t)
	{
		if (t.is_loaded) 
			glDeleteTextures(1, &t.id);
	}
	void activate(Texture2D& t, GLuint shader_id, const char* sampler_name, GLuint offset)
	{
		glUniform1i(glGetUniformLocation(shader_id, sampler_name), offset);
		glActiveTexture(GL_TEXTURE0 + offset);
		glBindTexture(GL_TEXTURE_2D, t.id);
	}
}
namespace texture3D
{
	void init(Texture3D& t, GLfloat* data, int dimensions)
	{
		t.dimensions = dimensions;

		glGenTextures(1, &t.id);
		glBindTexture(GL_TEXTURE_3D, t.id);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexStorage3D(GL_TEXTURE_3D, log2(dimensions), GL_RGBA8, dimensions,dimensions,dimensions);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0,0,0, dimensions,dimensions,dimensions, GL_RGBA, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);

		check_gl_error();

		t.is_loaded = true;
	}
	void uninit(Texture3D& t)
	{
		if (t.is_loaded) 
			glDeleteTextures(1, &t.id);
		t.is_loaded = false;
	}
	void activate(Texture3D& t, GLuint shader_id, const char* samplerName, int textureLocation)
	{
		glUniform1i(glGetUniformLocation(shader_id, samplerName), textureLocation);
		glActiveTexture(GL_TEXTURE0 + textureLocation);
		glBindTexture(GL_TEXTURE_3D, t.id);
	}
	void deactivate()
	{
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	void clear(Texture3D& t, const vec4& clearColor)
	{
		glBindTexture(GL_TEXTURE_3D, t.id);
		glClearTexImage(t.id, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(clearColor));
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	void generate_mipmaps(Texture3D& t)
	{
		glBindTexture(GL_TEXTURE_3D, t.id);
		glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	void fill_pixel(GLfloat* data, int x,int y,int z, int w,int h,int d)
	{
		int floats = 4; // r+g+b+a
		int i = floats * (x + w * (y + h * z));

		assert(i < (w*h*d*floats));

		data[i+0] = 1.0;
		data[i+1] = 1.0;
		data[i+2] = 1.0;
		data[i+3] = 1.0;
	}
	void fill_corners(GLfloat* data, int w, int h, int d)
	{
		assert(w == h && h == d);

		for (int x=1; x < w; x++) {
			fill_pixel(data, x, 1, 1, w,h,d);
			fill_pixel(data, x, h-1, 1, w,h,d);
			fill_pixel(data, x, 1, d-1, w,h,d);
			fill_pixel(data, x, h-1, d-1, w,h,d);

			fill_pixel(data, 1, x, 1, w,h,d);
			fill_pixel(data, w-1, x, 1, w,h,d);
			fill_pixel(data, w-1, x, d-1, w,h,d);
			fill_pixel(data, 1, x, d-1, w,h,d);

			fill_pixel(data, 1, 1, x, w,h,d);
			fill_pixel(data, w-1, 1, x, w,h,d);
			fill_pixel(data, w-1, h-1, x, w,h,d);
			fill_pixel(data, 1, h-1, x, w,h,d);
		}
	}
}

namespace framebuffer
{
	void init(Frame_Buffer& fbo, int w, int h)
	{
		fbo.width = w;
		fbo.height = h;

		GLenum magFilter = GL_NEAREST;
		GLenum minFilter = GL_NEAREST;
		GLint internalFormat = GL_RGB16F;
		GLint format = GL_FLOAT;
		GLint wrap = GL_REPEAT;

		glGenFramebuffers(1, &fbo.fbo_id);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo_id);

		glGenTextures(1, &fbo.color_texture_id);
		glBindTexture(GL_TEXTURE_2D, fbo.color_texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, GL_RGBA, format, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo.color_texture_id, 0);

		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "fbo", "failed to initialize");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		check_gl_error();

		fbo.is_inited = true;
	}

	void uninit(Frame_Buffer& fbo)
	{
		if (fbo.is_inited) {
			glDeleteTextures(1, &fbo.color_texture_id);
			if (fbo.has_depth)
				glDeleteRenderbuffers(1, &fbo.depth_rbo_id);
			glDeleteFramebuffers(1, &fbo.fbo_id);
		}
		fbo.is_inited = false;
	}

	void init_with_depth(Frame_Buffer& fbo, int w, int h)
	{
		init(fbo, w, h);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo_id);

		fbo.has_depth = true;
		glGenRenderbuffers(1, &fbo.depth_rbo_id);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo.depth_rbo_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo.depth_rbo_id);

		check_gl_error();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void activate_color_as_texture(Frame_Buffer& fbo, const int shaderProgram, const char* glSamplerName, const int textureLocation)
	{
		glUniform1i(glGetUniformLocation(shaderProgram, glSamplerName), textureLocation);
		glActiveTexture(GL_TEXTURE0 + textureLocation);
		glBindTexture(GL_TEXTURE_2D, fbo.color_texture_id);
	}
}

namespace shader
{
	bool init(Shader_Program& prog, const char* name, const std::string& path_to_vert, const std::string& path_to_frag, const std::string& path_to_geom)
	{
		prog.name = name;

		bool hasGeom = (path_to_geom.size() > 0); // @TODO @Cleanup lol

		std::string vert_src = application::read_file(path_to_vert);
		std::string frag_src = application::read_file(path_to_frag);
		std::string geom_src;
		if (hasGeom) geom_src = application::read_file(path_to_geom);
		Shader_Source vert, frag, geom;
		source_init(vert, name, Shader_Type::VERTEX, vert_src.c_str());
		source_init(frag, name, Shader_Type::FRAGMENT, frag_src.c_str());
		if (hasGeom) source_init(geom, name, Shader_Type::GEOMETRY, geom_src.c_str());

		if (!source_compile(vert)) return false;
		if (!source_compile(frag)) return false;
		if (hasGeom) if (!source_compile(geom)) return false;
		prog.id = glCreateProgram();
		glAttachShader(prog.id, vert.id);
		glAttachShader(prog.id, frag.id);
		if (hasGeom) glAttachShader(prog.id, geom.id);
		glLinkProgram(prog.id);

		GLint is_linked = 0;
		glGetProgramiv(prog.id, GL_LINK_STATUS, &is_linked);
		assert(is_linked == GL_TRUE);

		source_uninit(vert);
		source_uninit(frag);
		if (hasGeom) source_uninit(geom);

		LOG("shader", "created shader program (%s) (%u)", prog.name, prog.id);
		return true;
	}
	void deinit(Shader_Program& prog) {
		glDeleteProgram(prog.id);
		prog.id = 0;
	}
	GLuint activate(Shader_Program& prog) {
		assert(prog.id != u32(-1));
		glUseProgram(prog.id);
		check_gl_error();
		return prog.id;
	}
	void deactivate() {
		glUseProgram(0);
	}

	void source_init(Shader_Source& shader, const char* name, Shader_Type type, const char* src) {
		shader.name = name;
		shader.type = type;
		shader.src = src;
	}
	void source_uninit(Shader_Source& shader) {
		glDeleteShader(shader.id);
		shader.id = 0;
		shader.isCompiled = false;
	}

	bool source_compile(Shader_Source& shader)
	{
		ASSERT(shader.isCompiled == false, "shader", "shader (%s) already compiled", shader.name);
		ASSERT(shader.src != 0, "shader", "shader (%s) source is null", shader.name);

		shader.id = glCreateShader((GLenum) shader.type);
		glShaderSource(shader.id, 1, &shader.src, NULL);
		glCompileShader(shader.id);

		shader.isCompiled = was_compilation_successful(shader);
		if (shader.isCompiled)
			LOG("shader", "compiled shader (%s)", shader.name);
		else
			LOG("shader", "couldn't compile shader (%s)", shader.name);

		return shader.isCompiled;
	}

	bool was_compilation_successful(Shader_Source& shader)
	{
		GLint compileStatus = 0;
		glGetShaderiv(shader.id, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE) {
			log_compile_error(shader);
			return false;
		}

		return true;
	}

	void log_compile_error(Shader_Source& shader)
	{
		// http://www.opengl.org/sdk/docs/man/xhtml/glGetShader.xml
		// http://www.opengl.org/sdk/docs/man/xhtml/glGetShaderInfoLog.xml
		char* log = 0;
		int logLength = 0;
		int charsWritten = 0;

		glGetShaderiv(shader.id, GL_INFO_LOG_LENGTH, &logLength);
		ASSERT(logLength > 0, "shader", "log length 0 woot?");

		log = (char*) malloc(logLength);
		glGetShaderInfoLog(shader.id, logLength, &charsWritten, log);
		LOG("shader", "error loading shader (%s)", shader.name);
		LOG("shader", "%s\n", log);
		free(log);

		glDeleteShader(shader.id);
		shader.id = 0;

		ASSERT(false, "shader", "fix yo shader!");
	}
}

void _print_gl_error(const char* error, const char* file, int line) {
	LOG("gl", "GL error %s at %s : %d", error, file, line);
}
void _check_gl_error(const char* file, int line)
{
	GLenum err (glGetError());

	while (err != GL_NO_ERROR)
	{
		switch (err)
		{
			case GL_INVALID_OPERATION: _print_gl_error("INVALID_OPERATION",file,line); break;
			case GL_INVALID_ENUM: _print_gl_error("INVALID_ENUM",file,line); break;
			case GL_INVALID_VALUE: _print_gl_error("INVALID_VALUE",file,line); break;
			case GL_OUT_OF_MEMORY: _print_gl_error("OUT_OF_MEMORY",file,line); break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: _print_gl_error("INVALID_FRAMEBUFFER_OPERATION",file,line); break;
		}

		err = glGetError();
	}
}

