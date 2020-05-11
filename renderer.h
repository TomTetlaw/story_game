#ifndef __RENDERER_H__
#define __RENDERER_H__

struct Vertex {
	Vec3 position;
	Vec4 colour;
	Vec2 uv;
	Vec3 normal;
};

struct Program {
	const char *vertex_filename = nullptr;
	const char *frag_filename = nullptr;
	unsigned int vertex_object = 0;
	unsigned int frag_object = 0;
	unsigned int program_object = 0;
};


void renderer_init();
void renderer_shutdown();
void renderer_begin_frame();
void renderer_end_frame();

void load_program(Program *program);
void set_program(Program *program);

int add_verts(Vertex *verts, int num_verts, uint *indices, int num_indices);

struct Render_Texture {
    Texture *texture = nullptr;
    Vec2 position;
    Vec2 size;
};

void render_texture(Render_Texture *rt);

#endif