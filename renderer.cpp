#include "include.h"

#define MAX_VERTICIES 1000000
#define MAX_INDICES 1000000

struct RC_Texture {
	Texture *texture = nullptr;
	int first_vertex = 0;
};

struct RC_Some_Verts {
	int first_vertex = 0;
	int num_indices = 0;
	uint mode = 0;
	bool fill = false;
};

enum Render_Command_Type {
	RC_SOME_VERTS,
	RC_TEXTURE,
};

struct Render_Command {
	Render_Command() {}

	Render_Command_Type type;

	union {
		RC_Texture texture;
		RC_Some_Verts verts;
	};
};

internal unsigned int vertex_buffer = 0;
internal unsigned int index_buffer = 0;
internal unsigned int vertex_array = 0;

internal Program basic_textured;
internal Program basic;
internal Program *current_program = nullptr;

internal Mat4 projection_matrix;
internal Mat4 transformation_matrix;
internal Mat4 worldview_matrix;

internal Array<Vertex> vertex_list;
internal Array<uint> index_list;
internal Array<Render_Command> commands;

internal Scratch_Allocator r_allocator;

void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

void check_required_extensions() {
	struct {
		GLboolean available = false;
		const char *name = nullptr;
	} required_extensions[] = {
		{GLEW_ARB_debug_output, "GLEW_ARB_debug_output"},
		{GLEW_ARB_direct_state_access, "GLEW_ARB_direct_state_access"},
		{GLEW_ARB_draw_elements_base_vertex, "GLEW_ARB_draw_elements_base_vertex"},
	};

	for (int i = 0; i < 3; i++) {
		if (!required_extensions[i].available) {
			sys_error("Required extension %s not available.", required_extensions[i].name);
		}
	}
}

unsigned int load_shader(const char *filename, unsigned int type) {
	unsigned int object;
	GLint compiled = 0;

	Load_File_Result file = load_file(filename);
	object = glCreateShader(type);
	glShaderSource(object, 1, &file.data, &file.length);
	glCompileShader(object);
	delete[] file.data;

	glGetShaderiv(object, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		printf("Failed to compile shader %s", filename);

		GLint blen = 0;
		GLsizei slen = 0;
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &blen);
		if (blen > 1) {
			char *compiler_log = new char[blen];
			glGetShaderInfoLog(object, blen, &slen, compiler_log);
			printf("Shader info log: %s", compiler_log);
			delete[] compiler_log;
		}

		glDeleteShader(object);
		return 0;
	}

	return object;
}

void load_program(Program *program) {
	program->vertex_object = load_shader(program->vertex_filename, GL_VERTEX_SHADER);
	program->frag_object = load_shader(program->frag_filename, GL_FRAGMENT_SHADER);
	program->program_object = glCreateProgram();
	glAttachShader(program->program_object, program->vertex_object);
	glAttachShader(program->program_object, program->frag_object);
	glLinkProgram(program->program_object);

	GLint linked;
	glGetProgramiv(program->program_object, GL_LINK_STATUS, &linked);
	if (!linked) {
		printf("Failed to link program");

		GLint blen = 0;
		GLsizei slen = 0;
		glGetProgramiv(program->program_object, GL_INFO_LOG_LENGTH, &blen);
		if (blen > 1) {
			char *compiler_log = new char[blen];
			glGetProgramInfoLog(program->program_object, blen, &slen, compiler_log);
			printf("Program info log: %s", compiler_log);
			delete[] compiler_log;
		}

		glDeleteProgram(program->program_object);
	}
}

void hotload_vertex_callback(const char *filename, void *data) {
	Program *program = (Program *)data;

	glDetachShader(program->program_object, program->vertex_object);
	glDeleteShader(program->vertex_object);
	program->vertex_object = load_shader(program->vertex_filename, GL_VERTEX_SHADER);
	glAttachShader(program->program_object, program->vertex_object);
	glLinkProgram(program->program_object);
}

void hotload_frag_callback(const char *filename, void *data) {
	Program *program = (Program *)data;

	glDetachShader(program->program_object, program->frag_object);
	glDeleteShader(program->frag_object);
	program->frag_object = load_shader(program->frag_filename, GL_FRAGMENT_SHADER);
	glAttachShader(program->program_object, program->frag_object);
	glLinkProgram(program->program_object);
}

void setup_program(Program *program, const char *vertex_filename, const char *frag_filename) {
	program->vertex_filename = vertex_filename;
	program->frag_filename = frag_filename;
	load_program(program);
	hotload_add_file(program->vertex_filename, program, hotload_vertex_callback);
	hotload_add_file(program->frag_filename, program, hotload_frag_callback);
}

void renderer_init() {
	fclose(fopen("data/opengl.log", "w"));

	check_required_extensions();

	glEnable(GL_TEXTURE_2D);
	
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)opengl_debug_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glEnableVertexArrayAttrib(vertex_array, 0);
	glEnableVertexArrayAttrib(vertex_array, 1);
	glEnableVertexArrayAttrib(vertex_array, 2);
	glEnableVertexArrayAttrib(vertex_array, 3);
	glVertexArrayAttribFormat(vertex_array, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribFormat(vertex_array, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, colour));
	glVertexArrayAttribFormat(vertex_array, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribFormat(vertex_array, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

	glCreateBuffers(1, &vertex_buffer);
	glNamedBufferData(vertex_buffer, MAX_VERTICIES * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glCreateBuffers(1, &index_buffer);
	glNamedBufferData(index_buffer, MAX_INDICES * sizeof(uint), nullptr, GL_DYNAMIC_DRAW);

	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	setup_program(&basic_textured, "data/shaders/basic_textured.vert", "data/shaders/basic_textured.frag");
	setup_program(&basic, "data/shaders/basic.vert", "data/shaders/basic.frag");

	r_allocator.init(1024 * 10);
	index_list.allocator = &r_allocator;
	vertex_list.allocator = &r_allocator;
	commands.allocator = &r_allocator;
}

void renderer_shutdown() {
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &index_buffer);
	glDeleteVertexArrays(1, &vertex_array);

	r_allocator.shutdown();
}

void renderer_begin_frame() {
	r_allocator.reset();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_end_frame() {
	assert(vertex_list.num < MAX_VERTICIES);
	assert(index_list.num < MAX_INDICES);
	glNamedBufferSubData(vertex_buffer, 0, vertex_list.num * sizeof(Vertex), vertex_list.data);
	glNamedBufferSubData(index_buffer, 0, index_list.num * sizeof(uint), index_list.data);

	For(commands) {
		switch (it.type) {
		case RC_TEXTURE:
			set_program(&basic_textured);
			glBindTexture(GL_TEXTURE_2D, it.texture.texture->api_object);
			glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, it.texture.first_vertex);
			glBindTexture(GL_TEXTURE_2D, 0);
			break;
		case RC_SOME_VERTS:
			set_program(&basic);
			if (it.verts.fill)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElementsBaseVertex(it.verts.mode, it.verts.num_indices, GL_UNSIGNED_INT, nullptr, it.verts.first_vertex);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		}
	}}}

	commands.num = 0;
	vertex_list.num = 0;
	index_list.num = 0;

	SDL_GL_SwapWindow(sys.window);
	glUseProgram(0);
}

void set_program(Program *program) {
	current_program = program;

	if (!current_program) {
		return;
	}

	glUseProgram(current_program->program_object);

	projection_matrix = create_ortho_matrix(0, window_width, window_height, 0);
	worldview_matrix = create_identity_matrix();

	int projection_matrix_loc = glGetUniformLocation(current_program->program_object, "projection_matrix");
	int worldview_matrix_loc = glGetUniformLocation(current_program->program_object, "worldview_matrix");

	glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, projection_matrix.e);
	glUniformMatrix4fv(worldview_matrix_loc, 1, GL_FALSE, worldview_matrix.e);

	int now_loc = glGetUniformLocation(current_program->program_object, "now");
	glUniform1f(now_loc, now);

	if (current_program == &basic_textured) {
		int sampler_loc = glGetUniformLocation(current_program->program_object, "diffuse_texture");
		glUniform1i(sampler_loc, 0);
	}
}

int add_verts(Vertex *_vertices, int num_vertices, uint *_indices, int num_indices) {
	vertex_list.ensure_size(vertex_list.num + num_vertices);
	index_list.ensure_size(index_list.num + num_indices);

	for (int i = 0; i < num_indices; i++) {
		index_list.append(_indices[i]);
	}

	int first_vertex = vertex_list.num;
	for (int i = 0; i < num_vertices; i++) {
		vertex_list.append(_vertices[i]);
	}
	return first_vertex;
}

void render_texture(Render_Texture *rt) {
	float width = rt->texture->size.x;
	float height = rt->texture->size.y;
	if (rt->size.x != -1)
		width = rt->size.x;
	if (rt->size.y != -1)
		height = rt->size.y;

	float x0 = rt->position.x - width / 2;
	float y0 = rt->position.y - height / 2;
	float x1 = rt->position.x + width / 2;
	float y1 = rt->position.y + height / 2;

	Vertex verts[4];
	verts[0].position = Vec3(x0, y0, 0);
	verts[1].position = Vec3(x0, y1, 0);
	verts[2].position = Vec3(x1, y1, 0);
	verts[3].position = Vec3(x1, y0, 0);
	verts[0].uv = Vec2(0, 0);
	verts[1].uv = Vec2(0, 1);
	verts[2].uv = Vec2(1, 1);
	verts[3].uv = Vec2(1, 0);
	verts[0].colour = Vec4(1, 1, 1, 1);
	verts[1].colour = Vec4(1, 1, 1, 1);
	verts[2].colour = Vec4(1, 1, 1, 1);
	verts[3].colour = Vec4(1, 1, 1, 1);

	unsigned int indices[6];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	int first_vertex = add_verts(verts, 4, indices, 6);

	Render_Command rc;
	rc.type = RC_TEXTURE;
	rc.texture.first_vertex = first_vertex;
	rc.texture.texture = rt->texture;
	commands.append(rc);
}

void render_box(Vec2 position, Vec2 size, Vec4 colour) {
	Vertex verts[4];
	verts[0].position = Vec3(position.x, position.y, 0);
	verts[1].position = Vec3(position.x, position.y + size.y, 0);
	verts[2].position = Vec3(position.x + size.x, position.y + size.y, 0);
	verts[3].position = Vec3(position.x + size.x, position.y, 0);
	verts[0].colour = colour;
	verts[1].colour = colour;
	verts[2].colour = colour;
	verts[3].colour = colour;

	unsigned int indices[4];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	int first_vertex = add_verts(verts, 4, indices, 4);

	Render_Command rc;
	rc.type = RC_SOME_VERTS;
	rc.verts.mode = GL_LINE_LOOP;
	rc.verts.fill = false;
	rc.verts.first_vertex = first_vertex;
	rc.verts.num_indices = 4;
	commands.append(rc);
}

void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	if (id == 131185)
		return;

	char message_source[64] = {};
	char message_type[64] = {};
	char message_severity[64] = {};

	switch (source) {
	case GL_DEBUG_SOURCE_API_ARB:
		strcpy_s(message_source, "OpenGL");
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
		strcpy_s(message_source, "Shader Compiler");
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
		strcpy_s(message_source, "Third Party");
		break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB:
		strcpy_s(message_source, "Application");
		break;
	case GL_DEBUG_SOURCE_OTHER_ARB:
		strcpy_s(message_source, "Other");
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
		strcpy_s(message_source, "Windows");
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR_ARB:
		strcpy_s(message_type, "Error");
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
		strcpy_s(message_type, "Deprecated behavior");
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
		strcpy_s(message_type, "Undefined behavior");
		break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB:
		strcpy_s(message_type, "Portability");
		break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:
		strcpy_s(message_type, "Performance");
		break;
	case GL_DEBUG_TYPE_OTHER_ARB:
		strcpy_s(message_type, "Other");
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH_ARB:
		strcpy_s(message_severity, "High");
		break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB:
		strcpy_s(message_severity, "Medium");
		break;
	case GL_DEBUG_SEVERITY_LOW_ARB:
		strcpy_s(message_severity, "Low");
		break;
	}

	if (severity != GL_DEBUG_SEVERITY_LOW_ARB) {
		printf("[%d, %s, %s, %s] %s\n", id, message_source, message_type, message_severity, message);
	}

	FILE *f = nullptr;
	fopen_s(&f, "data/opengl.log", "a");
	if (f) {
		fprintf(f, "[%d, %s, %s, %s] %s\n", id, message_source, message_type, message_severity, message);
		fclose(f);
	}
}