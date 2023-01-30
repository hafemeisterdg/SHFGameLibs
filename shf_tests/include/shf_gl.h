#if !defined(SHF_GL_H)
#define SHF_GL_H

// API Decls
// ================================================
#if defined(_WIN32)
#if defined(BUILD_LIBYTPE_SHARED)
#define SHF_GL_API __declspec(dllexport)
#elif defined(USE_LIBTYPE_SHARED)
#define SHF_GL_API __declspec(dllimport)
#else
#define SHF_GL_API 
#endif // dll check
#else
#define SHF_GL_API 
#endif // API decls

#define SHF_GL_PLATFORM_API SHF_GL_API
// ================================================

// Platform Detection
// ================================================
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SHF_GL_WINDOWS
#ifdef _WIN64
#define SHF_GL_WINDOWS64
#else
#define SHF_GL_WINDOWS32
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define SHF_GL_MAC
#else
// Unsupported apple
#endif
#elif defined(__linux__)
#define SHF_GL_LINUX
#endif // platform detection
// ================================================

#include <stdint.h>
#include <stdlib.h>

// GL Types
typedef unsigned int   GLenum;
typedef unsigned char  GLboolean;
typedef unsigned int   GLbitfield;
typedef void		   GLvoid;
typedef signed char	   GLbyte;	 /* 1-byte signed */
typedef short		   GLshort;	 /* 2-byte signed */
typedef int		       GLint;	 /* 4-byte signed */
typedef unsigned char  GLubyte;	 /* 1-byte unsigned */
typedef unsigned short GLushort; /* 2-byte unsigned */
typedef unsigned int   GLuint;	 /* 4-byte unsigned */
typedef int		       GLsizei;	 /* 4-byte signed */
typedef float		   GLfloat;	 /* single precision float */
typedef float		   GLclampf; /* single precision float in [0,1] */
typedef double		   GLdouble; /* double precision float */
typedef double		   GLclampd; /* double precision float in [0,1] */
typedef size_t         GLsizeiptr;
typedef char           GLchar;


// GL 
#define GL_FALSE 0
#define GL_TRUE  1

#define GL_BYTE		         0x1400
#define GL_UNSIGNED_BYTE     0x1401
#define GL_SHORT		     0x1402
#define GL_UNSIGNED_SHORT    0x1403
#define GL_INT			     0x1404
#define GL_UNSIGNED_INT      0x1405
#define GL_FLOAT		     0x1406
#define GL_2_BYTES		     0x1407
#define GL_3_BYTES		     0x1408
#define GL_4_BYTES		     0x1409
#define GL_DOUBLE		     0x140A
#define GL_TRIANGLES	     0x0004
#define GL_UNSIGNED_INT_24_8 0x84FA

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100

// GL VBO
#define GL_ARRAY_BUFFER         0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW          0x88E4

// GL Shaders
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER   0x8B31
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_COMPUTE_SHADER  0x91B9
#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82

// GL Textures
#define GL_TEXTURE_2D               0x0DE1
#define GL_RGB                      0x1907
#define GL_RGBA                     0x1908
#define GL_DEPTH24_STENCIL8         0x88F0
#define GL_DEPTH32F_STENCIL8        0x8CAD
#define GL_DEPTH_ATTACHMENT         0x8D00
#define GL_DEPTH_STENCIL            0x84F9
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_TEXTURE_MIN_FILTER       0x2801
#define GL_TEXTURE_MAG_FILTER       0x2800
#define GL_LINEAR                   0x2601


namespace shf {
	namespace gl {
		typedef void* Gl_Api_Object;

		struct GL_Context {
			Gl_Api_Object platform_context;
			uint32_t      version_major;
			uint32_t      version_minor;

			void SHF_GL_API swap_buffers();
		};

		struct Vertex {
			struct {
				float x = 0;
				float y = 0;
				float z = 0;
			} position;

			struct {
				float x = 0;
				float y = 0;
				float z = 0;
			} normal;

			struct {
				float r = 1;
				float g = 1;
				float b = 1;
			} color;

			struct {
				float u = 0;
				float v = 0;
			} texcoords;
		};

		struct Index_Buffer {
			GLuint gl_id;

			uint32_t count;
			uint32_t buffer_size;

			void bind();
			void buffer_data(uint32_t* indices, uint32_t count, GLenum usage);
			void unbind();
		};

		enum Pixel_Format {
			Pixel_Format_Undefined = 0,

			Pixel_Format_Depth_Stencil,
			Pixel_Format_RGB24,
			Pixel_Format_RGBA32,

			Pixel_Format_Count
		};

		enum Shader_Type {
			Shader_Type_Undefined = 0,

			Shader_Type_Compute,
			Shader_Type_Fragment,
			Shader_Type_Geometry,
			Shader_Type_Vertex,

			Shader_Type_Count
		};

		struct Shader {
			GLuint gl_id;

			Shader_Type type;

			bool compile_from_source(const char* shader_source);
		};

		struct Shader_Program {
			GLuint gl_id;

			void attach_shader(Shader* shader);
			void bind();
			bool link();
			void unbind();
		};

		struct Texture {
			GLuint gl_id;

			uint32_t     width;
			uint32_t     height;
			Pixel_Format format;

			void bind();
			void set_pixel_data(uint8_t* pixel_data, uint32_t width, uint32_t height, Pixel_Format format);
			void unbind();
		};

		struct Vertex_Array_Object {
			GLuint gl_id;

			void bind();
			void unbind();
		};

		struct Vertex_Buffer {
			GLuint gl_id;

			uint32_t count;
			uint32_t buffer_size;

			void bind();
			void buffer_data(Vertex* vertices, uint32_t count, GLenum usage);
			void predraw();
			void unbind();
		};

		SHF_GL_PLATFORM_API bool create_context(void* platform_window, uint32_t version_major, uint32_t version_minor, GL_Context* out_ctx);
		SHF_GL_PLATFORM_API bool destroy_context(GL_Context* ctx);
		SHF_GL_PLATFORM_API bool make_context_current(GL_Context* ctx);

		SHF_GL_API bool create_index_buffer(Index_Buffer* out_buffer);
		SHF_GL_API bool create_shader(Shader_Type type, Shader* out_shader);
		SHF_GL_API bool create_shader_program(Shader_Program* out_program);
		SHF_GL_API bool create_texture(uint32_t width, uint32_t height, Pixel_Format format, Texture* out_texture);
		SHF_GL_API bool create_vertex_array_object(Vertex_Array_Object* out_vao);
		SHF_GL_API bool create_vertex_buffer(Vertex_Buffer* out_buffer);
		SHF_GL_API bool destroy_index_buffer(Index_Buffer* buffer);
		SHF_GL_API bool destroy_shader(Shader* shader);
		SHF_GL_API bool destroy_shader_program(Shader_Program* program);
		SHF_GL_API bool destroy_texture(Texture* texture);
		SHF_GL_API bool destroy_vertex_array_object(Vertex_Array_Object* vao);
		SHF_GL_API bool destroy_vertex_buffer(Vertex_Buffer* buffer);
		SHF_GL_API void draw(Vertex_Buffer* buffer);
		SHF_GL_API void draw_indexed(Vertex_Buffer* v_buff, Index_Buffer* i_buff);
		SHF_GL_API void draw_textured(Vertex_Buffer* buffer, Texture* texture);
		SHF_GL_API void draw_textured_indexed(Vertex_Buffer* v_buffer, Index_Buffer* i_buffer, Texture* texture);

		SHF_GL_API void   glAttachShader(GLuint program, GLuint shader);
		SHF_GL_API void   glBindBuffer(GLenum target, GLuint buffer);
		SHF_GL_API void   glBindTexture(GLenum target, GLuint texture);
		SHF_GL_API void   glBindVertexArray(GLuint array);
		SHF_GL_API void   glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		SHF_GL_API void   glClear(GLbitfield mask);
		SHF_GL_API void   glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
		SHF_GL_API void   glCompileShader(GLuint shader);
		SHF_GL_API GLuint glCreateProgram();
		SHF_GL_API GLuint glCreateShader(GLenum shader_type);
		SHF_GL_API void   glDeleteBuffers(GLsizei num, const GLuint* buffers);
		SHF_GL_API void   glDeleteProgram(GLuint program);
		SHF_GL_API void   glDeleteShader(GLuint shader);
		SHF_GL_API void   glDeleteTextures(GLsizei num, const GLuint* textures);
		SHF_GL_API void   glDeleteVertexArrays(GLsizei num, const GLuint* arrays);
		SHF_GL_API void   glDrawArrays(GLenum mode, GLint first, GLsizei count);
		SHF_GL_API void   glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices);
		SHF_GL_API void   glEnableVertexAttribArray(GLuint index);
		SHF_GL_API void   glGenBuffers(GLsizei num, GLuint* buffers);
		SHF_GL_API void   glGenerateMipmap(GLenum target);
		SHF_GL_API void   glGenTextures(GLsizei num, GLuint* textures);
		SHF_GL_API void   glGenVertexArrays(GLsizei num, GLuint* arrays);
		SHF_GL_API void   glGetProgramInfoLog(GLuint program, GLsizei max_length, GLsizei* length, GLchar* info_log);
		SHF_GL_API void   glGetProgramiv(GLuint program, GLenum pname, GLint* params);
		SHF_GL_API void   glGetShaderInfoLog(GLuint shader, GLsizei max_length, GLsizei* length, GLchar* info_log);
		SHF_GL_API void   glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
		SHF_GL_API void   glLinkProgram(GLuint program);
		SHF_GL_API void   glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
		SHF_GL_API void   glTexImage2D(GLenum target, GLint level, GLint internal_format, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data);
		SHF_GL_API void   glTexParameteri(GLenum target, GLenum pname, GLint param);
		SHF_GL_API void   glUseProgram(GLuint program);
		SHF_GL_API void   glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
		SHF_GL_API void   glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
	}
}

#endif // SHF_GL_H

#if defined(SHF_GL_IMPL)

#include <stdio.h>

namespace shf {
	namespace gl {
		static bool gl_functions_need_loaded = true;

		bool create_index_buffer(Index_Buffer* out_buffer) {
			if (!out_buffer) return false;

			glGenBuffers(1, &out_buffer->gl_id);
			out_buffer->count       = 0;
			out_buffer->buffer_size = 0;
		}

		bool create_shader(Shader_Type type, Shader* out_shader) {
			if (!out_shader) return false;

			switch (type) {
				case Shader_Type_Compute: {
					out_shader->gl_id = glCreateShader(GL_COMPUTE_SHADER);
				} break; 

				case Shader_Type_Fragment: {
					out_shader->gl_id = glCreateShader(GL_FRAGMENT_SHADER);
				} break; 

				case Shader_Type_Geometry: {
					out_shader->gl_id = glCreateShader(GL_GEOMETRY_SHADER);
				} break; 

				case Shader_Type_Vertex: {
					out_shader->gl_id = glCreateShader(GL_VERTEX_SHADER);
				} break; 
			}

			out_shader->type = type;

			return true;
		}

		bool create_shader_program(Shader_Program* out_program) {
			if (!out_program) return false;

			out_program->gl_id = glCreateProgram();

			return true;
		}

		bool create_texture(uint32_t width, uint32_t height, Pixel_Format format, Texture* out_texture) {
			if (!out_texture) return false;

			glGenTextures(1, &out_texture->gl_id);
			out_texture->set_pixel_data(0, width, height, format);

			return true;
		}

		bool create_vertex_array_object(Vertex_Array_Object* out_vao) {
			if (!out_vao) return false;

			glGenVertexArrays(1, &out_vao->gl_id);

			return true;
		}

		bool create_vertex_buffer(Vertex_Buffer* out_buffer) {
			if (!out_buffer) return false;

			glGenBuffers(1, &out_buffer->gl_id);
			out_buffer->count       = 0;
			out_buffer->buffer_size = 0;
		}

		bool destroy_index_buffer(Index_Buffer* buffer) {
			if (!buffer || buffer->gl_id == 0) return false;

			glDeleteBuffers(1, &buffer->gl_id);
			memset(buffer, 0, sizeof(Index_Buffer));

			return true;
		}

		bool destroy_shader(Shader* shader) {
			if (!shader || shader->gl_id == 0) return false;

			glDeleteShader(shader->gl_id);
			memset(shader, 0, sizeof(Shader));

			return true;
		}

		bool destroy_shader_program(Shader_Program* program) {
			if (!program || program->gl_id == 0) return false;

			glDeleteProgram(program->gl_id);
			memset(program, 0, sizeof(Shader_Program));

			return true;
		}

		bool destroy_texture(Texture* texture) {
			if (!texture || texture->gl_id == 0) return false;

			glDeleteTextures(1, &texture->gl_id);
			memset(texture, 0, sizeof(Texture));

			return true;
		}

		bool destroy_vertex_array_object(Vertex_Array_Object* vao) {
			if (!vao || vao->gl_id == 0) return false;

			glDeleteVertexArrays(1, &vao->gl_id);
			memset(vao, 0, sizeof(Vertex_Array_Object));

			return true;
		}

		bool destroy_vertex_buffer(Vertex_Buffer* buffer) {
			if (!buffer || buffer->gl_id == 0) return false;

			glDeleteBuffers(1, &buffer->gl_id);
			memset(buffer, 0, sizeof(Vertex_Buffer));


			return true;
		}

		void draw(Vertex_Buffer* buffer) {
			if (!buffer || buffer->gl_id == 0) return;

			buffer->bind();
			buffer->predraw();

			glDrawArrays(GL_TRIANGLES, 0, buffer->count);
		}

		void draw_indexed(Vertex_Buffer* v_buff, Index_Buffer* i_buff) {
			if (!v_buff || v_buff->gl_id == 0) return;
			if (!i_buff || i_buff->gl_id == 0) return;

			v_buff->bind();
			v_buff->predraw();

			i_buff->bind();
			glDrawElements(GL_TRIANGLES, i_buff->count, GL_UNSIGNED_INT, 0);
		}

		void draw_textured(Vertex_Buffer* buffer, Texture* texture) {
			if (!texture || texture->gl_id == 0) return;

			texture->bind();
			draw(buffer);
		}

		void draw_textured_indexed(Vertex_Buffer* v_buffer, Index_Buffer* i_buffer, Texture* texture) {
			if (!texture || texture->gl_id == 0) return;

			texture->bind();
			draw_indexed(v_buffer, i_buffer);
		}

		void Index_Buffer::bind() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->gl_id);
		}

		void Index_Buffer::buffer_data(uint32_t* indices, uint32_t count, GLenum usage) {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, (const void*)indices, usage);
			this->count       = count;
			this->buffer_size = sizeof(uint32_t) * count;
		}

		void Index_Buffer::unbind() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		bool Shader::compile_from_source(const char* shader_source) {
			int compile_status;
            char compile_log[512];

            glShaderSource(this->gl_id, 1, &shader_source, 0);
            glCompileShader(this->gl_id);
            glGetShaderiv(this->gl_id, GL_COMPILE_STATUS, &compile_status);
            if (!compile_status) {
                glGetShaderInfoLog(this->gl_id, 512, 0, compile_log);

                printf("Shader Compilation Error: \n");
                printf("%s \n", compile_log);

                return false;
            }

			return true;
		}

		void Shader_Program::attach_shader(Shader* shader) {
			if (!shader) return;

			glAttachShader(this->gl_id, shader->gl_id);
		}

		void Shader_Program::bind() {
			glUseProgram(this->gl_id);
		}

		bool Shader_Program::link() {
			int link_status;
	        char link_log[512];

	        glLinkProgram(this->gl_id);
	        glGetProgramiv(this->gl_id, GL_LINK_STATUS, &link_status);

	        if (!link_status) {
		        glGetProgramInfoLog(this->gl_id, 512, 0, link_log);

		        printf("Shader Program Link Error: \n");
		        printf("%s \n", link_log);

		        return false;
            }

            return true;
		}

		void Shader_Program::unbind() {
			glUseProgram(0);
		}

		void Texture::bind() {
			glBindTexture(GL_TEXTURE_2D, this->gl_id);
		}

		void Texture::set_pixel_data(uint8_t* pixel_data, uint32_t width, uint32_t height, Pixel_Format format) {
			this->width  = width;
			this->height = height;
			this->format = format;

			GLenum gl_format = 0;

			switch (format) {
				case Pixel_Format_Depth_Stencil: {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

					return;
				} break;

				case Pixel_Format_RGB24: {
					gl_format = GL_RGB;
				} break;

				case Pixel_Format_RGBA32: {
					gl_format = GL_RGBA;
				} break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, pixel_data);

            glGenerateMipmap(GL_TEXTURE_2D);
		}

		void Texture::unbind() {
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Vertex_Array_Object::bind() {
			glBindVertexArray(this->gl_id);
		}

		void Vertex_Array_Object::unbind() {
			glBindVertexArray(0);
		}

		void Vertex_Buffer::bind() {
			glBindBuffer(GL_ARRAY_BUFFER, this->gl_id);
		}

		void Vertex_Buffer::buffer_data(Vertex* vertices, uint32_t count, GLenum usage) {
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * count, (const void*)vertices, usage);
			this->count       = count;
			this->buffer_size = sizeof(Vertex) * count;
		}

		void Vertex_Buffer::predraw() {
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, normal)));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, color)));

			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, texcoords)));
		}

		void Vertex_Buffer::unbind() {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		typedef void (*PFN_glAttachShader)(GLuint, GLuint);
		static PFN_glAttachShader shf_glAttachShader = 0;
		void glAttachShader(GLuint program, GLuint shader) {
			shf_glAttachShader(program, shader);
		}

		typedef void (*PFN_glBindBuffer)(GLenum, GLuint);
		static PFN_glBindBuffer shf_glBindBuffer = 0;
		void glBindBuffer(GLenum target, GLuint buffer) {
			shf_glBindBuffer(target, buffer);
		}

		typedef void (*PFN_glBindTexture)(GLenum, GLuint);
		static PFN_glBindTexture shf_glBindTexture = 0;
		void glBindTexture(GLenum target, GLuint texture) {
			shf_glBindTexture(target, texture);
		}

		typedef void (*PFN_glBindVertexArray)(GLuint);
		static PFN_glBindVertexArray shf_glBindVertexArray = 0;
		void glBindVertexArray(GLuint array) {
			shf_glBindVertexArray(array);
		}

		typedef void (*PFN_glBufferData)(GLenum, GLsizeiptr, const void*, GLenum);
		static PFN_glBufferData shf_glBufferData = 0;
		void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
			shf_glBufferData(target, size, data, usage);
		}

		typedef void (*PFN_glClear)(GLbitfield);
		static PFN_glClear shf_glClear = 0;
		void glClear(GLbitfield mask) {
			shf_glClear(mask);
		}

		typedef void (*PFN_glClearColor)(GLclampf, GLclampf, GLclampf, GLclampf);
		static PFN_glClearColor shf_glClearColor = 0;
		void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a) {
			shf_glClearColor(r, g, b, a);
		}

		typedef void (*PFN_glCompileShader)(GLuint);
		static PFN_glCompileShader shf_glCompileShader = 0;
		void glCompileShader(GLuint shader) {
			shf_glCompileShader(shader);
		}

		typedef GLuint(*PFN_glCreateProgram)();
		static PFN_glCreateProgram shf_glCreateProgram = 0;
		GLuint glCreateProgram() {
			return shf_glCreateProgram();
		}

		typedef GLuint (*PFN_glCreateShader)(GLenum);
		static PFN_glCreateShader shf_glCreateShader = 0;
		GLuint glCreateShader(GLenum shader_type) {
			return shf_glCreateShader(shader_type);
		}

		typedef void (*PFN_glDeleteBuffers)(GLsizei, const GLuint*);
		static PFN_glDeleteBuffers shf_glDeleteBuffers = 0;
		void glDeleteBuffers(GLsizei num, const GLuint* buffers) {
			shf_glDeleteBuffers(num, buffers);
		}

		typedef void (*PFN_glDeleteProgram)(GLuint);
		static PFN_glDeleteProgram shf_glDeleteProgram = 0;
		void glDeleteProgram(GLuint program) {
			shf_glDeleteProgram(program);
		}

		typedef void (*PFN_glDeleteShader)(GLuint);
		static PFN_glDeleteShader shf_glDeleteShader = 0;
		void glDeleteShader(GLuint shader) {
			shf_glDeleteShader(shader);
		}

		typedef void (*PFN_glDeleteTextures)(GLsizei, const GLuint*);
		static PFN_glDeleteTextures shf_glDeleteTextures = 0;
		void glDeleteTextures(GLsizei num, const GLuint* textures) {
			shf_glDeleteTextures(num, textures);
		}

		typedef void (*PFN_glDeleteVertexArrays)(GLsizei, const GLuint*);
		static PFN_glDeleteVertexArrays shf_glDeleteVertexArrays = 0;
		void glDeleteVertexArrays(GLsizei num, const GLuint* arrays) {
			shf_glDeleteVertexArrays(num, arrays);
		}

		typedef void (*PFN_glDrawArrays)(GLenum, GLint, GLsizei);
		static PFN_glDrawArrays shf_glDrawArrays = 0;
		void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
			shf_glDrawArrays(mode, first, count);
		}

		typedef void (*PFN_glDrawElements)(GLenum, GLsizei, GLenum, const void*);
		static PFN_glDrawElements shf_glDrawElements = 0;
		void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
			shf_glDrawElements(mode, count, type, indices);
		}

		typedef void (*PFN_glEnableVertexAttribArray)(GLuint);
		static PFN_glEnableVertexAttribArray shf_glEnableVertexAttribArray = 0;
		void glEnableVertexAttribArray(GLuint index) {
			shf_glEnableVertexAttribArray(index);
		}

		typedef void (*PFN_glGenBuffers)(GLsizei, GLuint*);
		static PFN_glGenBuffers shf_glGenBuffers = 0;
		void glGenBuffers(GLsizei num, GLuint* buffers) {
			shf_glGenBuffers(num, buffers);
		}

		typedef void (*PFN_glGenerateMipmap)(GLenum);
		static PFN_glGenerateMipmap shf_glGenerateMipmap = 0;
		void glGenerateMipmap(GLenum target) {
			shf_glGenerateMipmap(target);
		}

		typedef void (*PFN_glGenTextures)(GLsizei, GLuint*);
		static PFN_glGenTextures shf_glGenTextures = 0;
		void glGenTextures(GLsizei num, GLuint* textures) {
			shf_glGenTextures(num, textures);
		}

		typedef void (*PFN_glGenVertexArrays)(GLsizei, GLuint*);
		static PFN_glGenVertexArrays shf_glGenVertexArrays = 0;
		void glGenVertexArrays(GLsizei num, GLuint* arrays) {
			shf_glGenVertexArrays(num, arrays);
		}

		typedef void (*PFN_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
		static PFN_glGetProgramInfoLog shf_glGetProgramInfoLog = 0;
		void glGetProgramInfoLog(GLuint program, GLsizei max_length, GLsizei* length, GLchar* info_log) {
			shf_glGetProgramInfoLog(program, max_length, length, info_log);
		}

		typedef void (*PFN_glGetProgramiv)(GLuint, GLenum, GLint*);
		static PFN_glGetProgramiv shf_glGetProgramiv = 0;
		void glGetProgramiv(GLuint program, GLenum pname, GLint* params) {
			shf_glGetProgramiv(program, pname, params);
		}

		typedef void (*PFN_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*);
		static PFN_glGetShaderInfoLog shf_glGetShaderInfoLog = 0;
		void glGetShaderInfoLog(GLuint shader, GLsizei max_length, GLsizei* length, GLchar* info_log) {
			shf_glGetShaderInfoLog(shader, max_length, length, info_log);
		}

		typedef void (*PFN_glGetShaderiv)(GLuint, GLenum, GLint*);
		static PFN_glGetShaderiv shf_glGetShaderiv = 0;
		void glGetShaderiv(GLuint shader, GLenum pname, GLint* params) {
			shf_glGetShaderiv(shader, pname, params);
		}

		typedef void (*PFN_glLinkProgram)(GLuint);
		static PFN_glLinkProgram shf_glLinkProgram = 0;
		void glLinkProgram(GLuint program) {
			shf_glLinkProgram(program);
		}

		typedef void (*PFN_glShaderSource)(GLuint, GLsizei, const GLchar**, const GLint*);
		static PFN_glShaderSource shf_glShaderSource = 0;
		void glShaderSource(GLuint shader, GLsizei count, const GLchar** string, const GLint* length) {
			shf_glShaderSource(shader, count, string, length);
		}

		typedef void (*PFN_glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
		static PFN_glTexImage2D shf_glTexImage2D = 0;
		void glTexImage2D(GLenum target, GLint level, GLint internal_format, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data) {
			shf_glTexImage2D(target, level, internal_format, width, height, border, format, type, data);
		}

		typedef void (*PFN_glTexParameteri)(GLenum, GLenum, GLint);
		static PFN_glTexParameteri shf_glTexParameteri = 0;
		void glTexParameteri(GLenum target, GLenum pname, GLint param) {
			shf_glTexParameteri(target, pname, param);
		}

		typedef void (*PFN_glUseProgram)(GLuint);
		static PFN_glUseProgram shf_glUseProgram = 0;
		void glUseProgram(GLuint program) {
			shf_glUseProgram(program);
		}

		typedef void (*PFN_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
		static PFN_glVertexAttribPointer shf_glVertexAttribPointer = 0;
		void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
			shf_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		}

		typedef void (*PFN_glViewport)(GLint, GLint, GLsizei, GLsizei);
		static PFN_glViewport shf_glViewport = 0;
		void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
			shf_glViewport(x, y, width, height);
		}
	}
}

#if defined(SHF_GL_WINDOWS)
#include <windows.h>

namespace shf {
	namespace gl {
		#define WGL_NUMBER_PIXEL_FORMATS_ARB      0x2000
		#define WGL_DRAW_TO_WINDOW_ARB            0x2001
		#define WGL_DRAW_TO_BITMAP_ARB            0x2002
		#define WGL_ACCELERATION_ARB              0x2003
		#define WGL_NEED_PALETTE_ARB              0x2004
		#define WGL_NEED_SYSTEM_PALETTE_ARB       0x2005
		#define WGL_SWAP_LAYER_BUFFERS_ARB        0x2006
		#define WGL_SWAP_METHOD_ARB               0x2007
		#define WGL_NUMBER_OVERLAYS_ARB           0x2008
		#define WGL_NUMBER_UNDERLAYS_ARB          0x2009
		#define WGL_TRANSPARENT_ARB               0x200A
		#define WGL_TRANSPARENT_RED_VALUE_ARB     0x2037
		#define WGL_TRANSPARENT_GREEN_VALUE_ARB   0x2038
		#define WGL_TRANSPARENT_BLUE_VALUE_ARB    0x2039
		#define WGL_TRANSPARENT_ALPHA_VALUE_ARB   0x203A
		#define WGL_TRANSPARENT_INDEX_VALUE_ARB   0x203B
		#define WGL_SHARE_DEPTH_ARB               0x200C
		#define WGL_SHARE_STENCIL_ARB             0x200D
		#define WGL_SHARE_ACCUM_ARB               0x200E
		#define WGL_SUPPORT_GDI_ARB               0x200F
		#define WGL_SUPPORT_OPENGL_ARB            0x2010
		#define WGL_DOUBLE_BUFFER_ARB             0x2011
		#define WGL_STEREO_ARB                    0x2012
		#define WGL_PIXEL_TYPE_ARB                0x2013
		#define WGL_COLOR_BITS_ARB                0x2014
		#define WGL_RED_BITS_ARB                  0x2015
		#define WGL_RED_SHIFT_ARB                 0x2016
		#define WGL_GREEN_BITS_ARB                0x2017
		#define WGL_GREEN_SHIFT_ARB               0x2018
		#define WGL_BLUE_BITS_ARB                 0x2019
		#define WGL_BLUE_SHIFT_ARB                0x201A
		#define WGL_ALPHA_BITS_ARB                0x201B
		#define WGL_ALPHA_SHIFT_ARB               0x201C
		#define WGL_ACCUM_BITS_ARB                0x201D
		#define WGL_ACCUM_RED_BITS_ARB            0x201E
		#define WGL_ACCUM_GREEN_BITS_ARB          0x201F
		#define WGL_ACCUM_BLUE_BITS_ARB           0x2020
		#define WGL_ACCUM_ALPHA_BITS_ARB          0x2021
		#define WGL_DEPTH_BITS_ARB                0x2022
		#define WGL_STENCIL_BITS_ARB              0x2023
		#define WGL_AUX_BUFFERS_ARB               0x2024
		#define WGL_NO_ACCELERATION_ARB           0x2025
		#define WGL_GENERIC_ACCELERATION_ARB      0x2026
		#define WGL_FULL_ACCELERATION_ARB         0x2027
		#define WGL_SWAP_EXCHANGE_ARB             0x2028
		#define WGL_SWAP_COPY_ARB                 0x2029
		#define WGL_SWAP_UNDEFINED_ARB            0x202A
		#define WGL_TYPE_RGBA_ARB                 0x202B
		#define WGL_TYPE_COLORINDEX_ARB           0x202C
		#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
		#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
		#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
		#define WGL_SAMPLE_BUFFERS_ARB            0x2041
		#define WGL_SAMPLES_ARB                   0x2042
		#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
		#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

		typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC shared_context, const int* attrib_list);
		wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

		typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* attrib_int_List, const FLOAT* attrib_float_list, UINT max_formats, int* formats, UINT* num_formats);
		wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;

		typedef HGLRC (*PFN_wglCreateContext)(HDC);
		static PFN_wglCreateContext shf_wglCreateContext = 0;

		typedef BOOL (*PFN_wglDeleteContext)(HGLRC);
		static PFN_wglDeleteContext shf_wglDeleteContext = 0;

		typedef PROC (*PFN_wglGetProcAddress)(LPCSTR);
		static PFN_wglGetProcAddress shf_wglGetProcAddress = 0;

		typedef BOOL (*PFN_wglMakeCurrent)(HDC, HGLRC);
		static PFN_wglMakeCurrent shf_wglMakeCurrent = 0;

		static bool  win_gl_initialized = false;
		static bool  win_gl_initialize(HWND host_window);

		static HMODULE win_gl_dll_handle = 0;
		static void* win_gl_get_proc_address(const char* function_name);

		struct Windows_GL_Context {
			HDC   device_context;
			HGLRC gl_context;
			HWND  parent_window;
		};

		bool create_context(void* platform_window, uint32_t version_major, uint32_t version_minor, GL_Context* out_ctx) {
			if (!platform_window || !out_ctx)                                 return false;
			if (version_major <= 0 || version_minor <= 0) return false;

			HWND window = (HWND)platform_window;
			if (!win_gl_initialized) {
				if (!win_gl_initialize(window)) {
					return false;
				}

				win_gl_initialized = true;
			}

			const uint32_t gl_version_major = version_major;
			const uint32_t gl_version_minor = version_minor;
			HDC device_context = GetDC(window);

			const int32_t pixel_attributes[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
				WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
				WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,     32,
				WGL_ALPHA_BITS_ARB,     8,
				WGL_DEPTH_BITS_ARB,     24,
				WGL_STENCIL_BITS_ARB,   8,
				WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
				WGL_SAMPLES_ARB,        4,
				0
			};
    
			int32_t pixel_format_id;
			uint32_t num_formats;
    
			bool status = wglChoosePixelFormatARB(device_context, pixel_attributes, 0, 1, &pixel_format_id, &num_formats);
			if(!status || num_formats == 0) {
				ReleaseDC(window, device_context);
    		
    			return false;
			}
    
			PIXELFORMATDESCRIPTOR pixel_format;
			DescribePixelFormat(device_context, pixel_format_id, sizeof(pixel_format), &pixel_format);
			SetPixelFormat(device_context, pixel_format_id, &pixel_format);
    
			const int32_t context_attributes[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, gl_version_major,
				WGL_CONTEXT_MINOR_VERSION_ARB, gl_version_minor,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0
			};
    
			HGLRC gl_context = wglCreateContextAttribsARB(device_context, 0, context_attributes);
			if(!gl_context) {
    			ReleaseDC(window, device_context);
                
    			return false;
			}

			Windows_GL_Context* windows_gl_ctx = (Windows_GL_Context*)malloc(sizeof(Windows_GL_Context));
			if (!windows_gl_ctx) {
				shf_wglMakeCurrent(device_context, 0);
				shf_wglDeleteContext(gl_context);
				ReleaseDC(window, device_context);

				return false;
			}

			windows_gl_ctx->device_context = device_context;
			windows_gl_ctx->gl_context     = gl_context;
			windows_gl_ctx->parent_window  = window;

			out_ctx->platform_context = (Gl_Api_Object)windows_gl_ctx;
			out_ctx->version_major    = gl_version_major;
			out_ctx->version_minor    = gl_version_minor;
			make_context_current(out_ctx);
    
			if (gl_functions_need_loaded) { 
				// Basic Functions
				shf::gl::shf_glClear      = (PFN_glClear)win_gl_get_proc_address("glClear");
				shf::gl::shf_glClearColor = (PFN_glClearColor)win_gl_get_proc_address("glClearColor");
				shf::gl::shf_glViewport   = (PFN_glViewport)win_gl_get_proc_address("glViewport");
				if (!shf::gl::shf_glClear || !shf::gl::shf_glClearColor || !shf::gl::shf_glViewport) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glDrawArrays   = (PFN_glDrawArrays)win_gl_get_proc_address("glDrawArrays");
				shf::gl::shf_glDrawElements = (PFN_glDrawElements)win_gl_get_proc_address("glDrawElements");
				if (!shf::gl::shf_glDrawArrays || !shf::gl::shf_glDrawElements) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glVertexAttribPointer = (PFN_glVertexAttribPointer)win_gl_get_proc_address("glVertexAttribPointer");
				if (!shf::gl::shf_glVertexAttribPointer) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				// Array Functions
				shf::gl::shf_glBindVertexArray         = (PFN_glBindVertexArray)win_gl_get_proc_address("glBindVertexArray");
				shf::gl::shf_glDeleteVertexArrays      = (PFN_glDeleteVertexArrays)win_gl_get_proc_address("glDeleteVertexArrays");
				shf::gl::shf_glEnableVertexAttribArray = (PFN_glEnableVertexAttribArray)win_gl_get_proc_address("glEnableVertexAttribArray");
				shf::gl::shf_glGenVertexArrays         = (PFN_glGenVertexArrays)win_gl_get_proc_address("glGenVertexArrays");
				if (!shf::gl::shf_glBindVertexArray || !shf::gl::shf_glDeleteVertexArrays || !shf::gl::shf_glEnableVertexAttribArray || !shf::gl::shf_glGenVertexArrays) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				// Buffer Functions 
				shf::gl::shf_glBindBuffer    = (PFN_glBindBuffer)win_gl_get_proc_address("glBindBuffer");
				shf::gl::shf_glBufferData    = (PFN_glBufferData)win_gl_get_proc_address("glBufferData");
				shf::gl::shf_glDeleteBuffers = (PFN_glDeleteBuffers)win_gl_get_proc_address("glDeleteBuffers");
				shf::gl::shf_glGenBuffers    = (PFN_glGenBuffers)win_gl_get_proc_address("glGenBuffers");
				if (!shf::gl::shf_glBindBuffer || !shf::gl::shf_glBufferData || !shf::gl::shf_glDeleteBuffers || !shf::gl::shf_glGenBuffers) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				// Shader Functions
				shf::gl::shf_glAttachShader  = (PFN_glAttachShader)win_gl_get_proc_address("glAttachShader");
				shf::gl::shf_glCompileShader = (PFN_glCompileShader)win_gl_get_proc_address("glCompileShader");
				shf::gl::shf_glCreateShader  = (PFN_glCreateShader)win_gl_get_proc_address("glCreateShader");
				shf::gl::shf_glDeleteShader  = (PFN_glDeleteShader)win_gl_get_proc_address("glDeleteShader");
				if (!shf::gl::shf_glAttachShader || !shf::gl::shf_glCompileShader || !shf::gl::shf_glCreateShader || !shf::gl::shf_glDeleteShader) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glGetShaderInfoLog = (PFN_glGetShaderInfoLog)win_gl_get_proc_address("glGetShaderInfoLog");
				shf::gl::shf_glGetShaderiv      = (PFN_glGetShaderiv)win_gl_get_proc_address("glGetShaderiv");
				shf::gl::shf_glShaderSource     = (PFN_glShaderSource)win_gl_get_proc_address("glShaderSource");
				if (!shf::gl::shf_glGetShaderInfoLog || !shf::gl::shf_glGetShaderiv || !shf::gl::shf_glShaderSource) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glCreateProgram = (PFN_glCreateProgram)win_gl_get_proc_address("glCreateProgram");
				shf::gl::shf_glDeleteProgram = (PFN_glDeleteProgram)win_gl_get_proc_address("glDeleteProgram");
				shf::gl::shf_glLinkProgram   = (PFN_glLinkProgram)win_gl_get_proc_address("glLinkProgram");
				shf::gl::shf_glUseProgram    = (PFN_glUseProgram)win_gl_get_proc_address("glUseProgram");
				if (!shf::gl::shf_glCreateProgram || !shf::gl::shf_glDeleteProgram || !shf::gl::shf_glLinkProgram || !shf::gl::shf_glUseProgram) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glGetProgramInfoLog = (PFN_glGetProgramInfoLog)win_gl_get_proc_address("glGetProgramInfoLog");
				shf::gl::shf_glGetProgramiv      = (PFN_glGetProgramiv)win_gl_get_proc_address("glGetProgramiv");
				if (!shf::gl::shf_glGetProgramInfoLog || !shf::gl::shf_glGetProgramiv) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				// Texture Functions
				shf::gl::shf_glBindTexture    = (PFN_glBindTexture)win_gl_get_proc_address("glBindTexture");
				shf::gl::shf_glDeleteTextures = (PFN_glDeleteTextures)win_gl_get_proc_address("glDeleteTextures");
				shf::gl::shf_glGenerateMipmap = (PFN_glGenerateMipmap)win_gl_get_proc_address("glGenerateMipmap");
				if (!shf::gl::shf_glBindTexture || !shf::gl::shf_glDeleteTextures || !shf::gl::shf_glGenerateMipmap) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				shf::gl::shf_glGenTextures    = (PFN_glGenTextures)win_gl_get_proc_address("glGenTextures");
				shf::gl::shf_glTexImage2D     = (PFN_glTexImage2D)win_gl_get_proc_address("glTexImage2D");
				shf::gl::shf_glTexParameteri  = (PFN_glTexParameteri)win_gl_get_proc_address("glTexParameteri");
				if (!shf::gl::shf_glGenTextures || !shf::gl::shf_glTexImage2D || !shf::gl::shf_glTexParameteri) {
					shf_wglMakeCurrent(device_context, 0);
					shf_wglDeleteContext(gl_context);
					ReleaseDC(window, device_context);

					return false;
				}

				gl_functions_need_loaded = false;
			}

			return true;
		}

		bool destroy_context(GL_Context* ctx) {
			if (!ctx || !ctx->platform_context) return false;

			Windows_GL_Context* windows_gl_ctx = (Windows_GL_Context*)ctx->platform_context;
			shf_wglMakeCurrent(windows_gl_ctx->device_context, 0);
			if (!shf_wglDeleteContext(windows_gl_ctx->gl_context)) return false;

			ReleaseDC(windows_gl_ctx->parent_window, windows_gl_ctx->device_context);

			free(windows_gl_ctx);

			memset(ctx, 0, sizeof(GL_Context));

			return true;
		}

		bool make_context_current(GL_Context* ctx) {
			if (!ctx) return false;

			Windows_GL_Context* windows_gl_ctx = (Windows_GL_Context*)ctx->platform_context;
			if (!windows_gl_ctx->device_context || !windows_gl_ctx->gl_context) return false;

			return shf_wglMakeCurrent(windows_gl_ctx->device_context, windows_gl_ctx->gl_context);
		}

		void GL_Context::swap_buffers() {
			if (!this->platform_context) return;

			Windows_GL_Context* windows_gl_ctx = (Windows_GL_Context*)this->platform_context;
			SwapBuffers(windows_gl_ctx->device_context);
		}

		bool win_gl_initialize(HWND host_window) {
			win_gl_dll_handle = LoadLibraryA("opengl32.dll");
			if (!win_gl_dll_handle) return false;

			shf_wglCreateContext  = (PFN_wglCreateContext)GetProcAddress(win_gl_dll_handle, "wglCreateContext");
			shf_wglDeleteContext  = (PFN_wglDeleteContext)GetProcAddress(win_gl_dll_handle, "wglDeleteContext");
			shf_wglGetProcAddress = (PFN_wglGetProcAddress)GetProcAddress(win_gl_dll_handle, "wglGetProcAddress");
			shf_wglMakeCurrent    = (PFN_wglMakeCurrent)GetProcAddress(win_gl_dll_handle, "wglMakeCurrent");

			if (!shf_wglCreateContext || !shf_wglDeleteContext || !shf_wglGetProcAddress || !shf_wglMakeCurrent) {
				return false;
			}

			HDC device_context = GetDC(host_window);
			if (!device_context) return false;

			PIXELFORMATDESCRIPTOR fake_format_descriptor = {};
			ZeroMemory(&fake_format_descriptor, sizeof(fake_format_descriptor));

			fake_format_descriptor.nSize = sizeof(fake_format_descriptor);
			fake_format_descriptor.nVersion = 1;
			fake_format_descriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			fake_format_descriptor.iPixelType = PFD_TYPE_RGBA;
			fake_format_descriptor.cColorBits = 32;
			fake_format_descriptor.cAlphaBits = 8;
			fake_format_descriptor.cDepthBits = 24;

			int32_t fake_format_id = ChoosePixelFormat(device_context, &fake_format_descriptor);
			if (fake_format_id == 0 || !SetPixelFormat(device_context, fake_format_id, &fake_format_descriptor)) {
				ReleaseDC(host_window, device_context);

				return false;
			}
    
			HGLRC fake_rendering_context = shf_wglCreateContext(device_context);
			if (!fake_rendering_context || !shf_wglMakeCurrent(device_context, fake_rendering_context)) {
				ReleaseDC(host_window, device_context);

				return false;
			}
    
			wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*) shf_wglGetProcAddress("wglCreateContextAttribsARB");
			wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*) shf_wglGetProcAddress("wglChoosePixelFormatARB");
			if (!wglCreateContextAttribsARB || !wglChoosePixelFormatARB) {
				shf_wglMakeCurrent(device_context, 0);
				shf_wglDeleteContext(fake_rendering_context);
				ReleaseDC(host_window, device_context);

				return false;
			}
    
			shf_wglMakeCurrent(device_context, 0);
			shf_wglDeleteContext(fake_rendering_context);
			ReleaseDC(host_window, device_context);

			return true;
		}

		void* win_gl_get_proc_address(const char* function_name) {
			void* function_address = (void*)shf_wglGetProcAddress(function_name);

			if (!function_address) {
				function_address = (void*)GetProcAddress(win_gl_dll_handle, function_name);
			}

			return function_address;
		}
	}
}

#elif defined(SHF_GL_MAC)

namespace shf {
	namespace gl {
	
	}
}

#elif defined(SHF_GL_LINUX)

namespace shf {
	namespace gl {
	
	}
}

#endif // platform detection

#endif // SHF_GL_IMPL