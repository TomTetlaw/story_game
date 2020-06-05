#include "include.h"

Render renderer;

internal Font *default_font = nullptr;
internal float debug_string_start_y = 10.0f;
internal Vec2 debug_string_position = Vec2(debug_string_start_y, 0.0f);
internal bool centered = false;
internal double z_near = 0.0;
internal double z_far = 1.0;

internal void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void render_init() {
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    glDebugMessageCallback((GLDEBUGPROC)opengl_debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	FILE *f = nullptr;
	fopen_s(&f, "data/opengl.log", "w");
	if(f) {
		fclose(f);
	}

	default_font = load_font("data/fonts/CascadiaCode.ttf", 16);
}

void render_shutdown() {
}

// scissor means that everything outside of the box will not be rendered
void render_start_scissor(float top, float left, float bottom, float right) {
	glEnable(GL_SCISSOR_TEST);
	// we have to adjust the y coord because glScissor uses bottom left as origin
	glScissor(left, bottom - window_height, right - left, bottom - top);
}

void render_end_scissor() {
	glDisable(GL_SCISSOR_TEST);
}

internal void setup_camera_and_zoom(bool use_camera, bool use_zoom) {
	if(use_camera) glTranslatef(-renderer.camera_position.x, -renderer.camera_position.y, 0);
	if(use_zoom) {
		float scale = render_scale_for_zoom_level();
		glScalef(scale, scale, 1);
	}
}

void render_setup_for_world() {
	centered = true;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float hw = window_width / 2;
	float hh = window_height / 2;
	glOrtho(-hw, hw, hh, -hh, z_near, z_far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	setup_camera_and_zoom(true, true);
}

void render_setup_for_ui() {
	centered = false;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window_width, window_height, 0, z_near, z_far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	

	setup_camera_and_zoom(false, false);
}

void render_begin_frame() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	debug_string_position = Vec2(debug_string_start_y, 10.0f);
}

void render_end_frame() {
	SDL_GL_SwapWindow(sys.window);
}

float render_scale_for_zoom_level() {
	float scale = 0.0f;
	if (renderer.zoom_level == 0) {
		scale = 1.0f;
	}
	else if (renderer.zoom_level < 0) {
		scale = 1.0f / (((renderer.zoom_level * 0.125f) * -1) + 1);
	}
	else if (renderer.zoom_level > 0) {
		scale = (renderer.zoom_level * 0.25f) + 1;
	}
	return scale;
}

float render_inverse_scale_for_zoom_level() {
	float scale = 0.0f;
	if (renderer.zoom_level == 0) {
		scale = 1.0f;
	}
	else if (renderer.zoom_level < 0) {
		scale = ((renderer.zoom_level * 0.125f) * -1) + 1;
	}
	else if (renderer.zoom_level > 0) {
		scale = 1.0f / ((renderer.zoom_level * 0.25f) + 1);
	}
	return scale;
}

Vec2 to_world_pos(Vec2 a) {
	return (a + renderer.camera_position) * render_inverse_scale_for_zoom_level();
}

void render_texture(Render_Texture *rt) {
	if (!rt->texture) {
		return;
	}

	float x = rt->position.x;
	float y = rt->position.y;
	float w = rt->size.x;
	float h = rt->size.y;

	if (rt->size.x <= 0) {
		w = (float)rt->texture->size.x;
	}
	if (rt->size.y <= 0) {
		h = (float)rt->texture->size.y;
	}

	float sl = rt->sl;
	float sh = rt->sh;
	float tl = rt->tl;
	float th = rt->th;

	if (sl == -1) { sl = 0; }
	if (sh == -1) { sh = 1; }
	if (tl == -1) { tl = 0; }
	if (th == -1) { th = 1; }

	float repeat_count_x = w / rt->texture->size.x;
	float repeat_count_y = h / rt->texture->size.y;
	if (!rt->repeat) {
		repeat_count_x = 1;
		repeat_count_y = 1;
	}

	float hw = w / 2;
	float hh = h / 2;

	glColor4f(v4parms(rt->colour));

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, rt->texture->api_object);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	float angle = rt->angle;
	if(centered) {
		glTranslatef(x, y, 0);
		glRotatef(angle, 0, 0, 1);
		glTranslatef(-x, -y, 0);
	}

	glBegin(GL_QUADS);
	if(centered) {
		glTexCoord2f(sl, tl);
		glVertex2f(x - hw, y - hh);
		glTexCoord2f(sl, th * repeat_count_y);
		glVertex2f(x - hw, y + hh);
		glTexCoord2f(sh * repeat_count_x, th * repeat_count_y);
		glVertex2f(x + hw, y + hh);
		glTexCoord2f(sh * repeat_count_x, tl);
		glVertex2f(x + hw, y - hh);
		glEnd();
	} else {
		glTexCoord2f(sl, tl);
		glVertex2f(x, y);
		glTexCoord2f(sl, th * repeat_count_y);
		glVertex2f(x, y + h);
		glTexCoord2f(sh * repeat_count_x, th * repeat_count_y);
		glVertex2f(x + w, y + h);
		glTexCoord2f(sh * repeat_count_x, tl);
		glVertex2f(x + w, y);
		glEnd();
	}

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void render_string(Vec2 position, const char *text, Vec4 colour, Font *font, float wrap) {
	if (!font) {
		font = default_font;
	}

	float origin_x = position.x;

	Render_Texture rt;

	int length = (int)strlen(text);
	for (int i = 0; i < length; i++) {
		if (text[i] == '\n' || ((position.x - origin_x + font->glyphs[(int)text[i]].advance) >= wrap && wrap > 0)) {
			position.x = origin_x;
			position.y += font->line_skip;
		}

		if (font->glyphs[(int)text[i]].available) {
			rt.texture = font->glyphs[(int)text[i]].texture;
			rt.position = position;
			rt.colour = colour;

			bool old_centered = centered;
			centered = false;

			render_texture(&rt);
			
			centered = old_centered;

			position.x += font->glyphs[(int)text[i]].advance;
		}
	}
}

void render_string_format(Vec2 position, Vec4 colour, Font *font, float wrap, const char *text, ...) {
	//@todo: make this better?
	constexpr int max_string_length = 1024;

	va_list argptr;
	char message[max_string_length];

	assert(strlen(text) < max_string_length);

	va_start(argptr, text);
	vsnprintf_s(message, 1024, text, argptr);
	va_end(argptr);

	render_string(position, message, colour, font, wrap);
}

// if you don't want to think about colour/font/wrap the default values will be same as those in render_string
void render_string_format_lazy(Vec2 position, const char *text, ...) {
	//@todo: make this better?
	constexpr int max_string_length = 1024;

	va_list argptr;
	char message[max_string_length];

	assert(strlen(text) < max_string_length);

	va_start(argptr, text);
	vsnprintf_s(message, 1024, text, argptr);
	va_end(argptr);

	render_string(position, message);
}

// renders string at the top of the screen underneath last debug string
// usually used to print out debug info that changes every frame.
// won't draw if var renderer_draw_debug is false
void debug_string(const char *text, ...) {
	if(!renderer.render_debug_strings) return;

	//@todo: make this better?
	constexpr int max_string_length = 1024;

	va_list argptr;
	char message[max_string_length];

	assert(strlen(text) < max_string_length);

	va_start(argptr, text);
	vsnprintf_s(message, 1024, text, argptr);
	va_end(argptr);

	render_string(debug_string_position, message);
	debug_string_position.y += (float)default_font->line_skip;
}

void render_line(Vec2 a, Vec2 b, Vec4 colour) {
	glColor4f(v4parms(colour));
	glBegin(GL_LINES);
	glVertex2f(a.x, a.y);
	glVertex2f(b.x, b.y);
	glEnd();
}

void render_line(float ax, float ay, float bx, float by, Vec4 colour) {
	render_line(Vec2(ax, ay), Vec2(bx, by), colour);
}

// render a line projected length units from a position 
void render_line2(Vec2 start, float length, float angle, Vec4 colour) {
	angle = deg2rad(angle);
	Vec2 end;
	end.x = start.x + (length * cos(angle));
	end.y = start.y + (length * sin(angle));
	render_line(start, end, colour);
}

void render_line2(float x, float y, float length, float angle, Vec4 colour) {
	render_line2(Vec2(x, y), length, angle, colour);
}

void render_box(Vec2 position, Vec2 size, bool fill, Vec4 colour) {
	if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	glColor4f(v4parms(colour));

	float x = position.x;
	float y = position.y;
	float w = size.x;
	float h = size.y;
	if(centered) {
		float hw = w / 2;
		float hh = h / 2;
		glBegin(GL_QUADS);
		glVertex2f(x + hw, y - hh);
		glVertex2f(x - hw, y - hh);
		glVertex2f(x - hw, y + hh);
		glVertex2f(x + hw, y + hh);
		glEnd();
	} else {
		glBegin(GL_QUADS);
		glVertex2f(x + w, y);
		glVertex2f(x, y);
		glVertex2f(x, y + h);
		glVertex2f(x + w, y + h);
		glEnd();
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void render_box(float x, float y, float w, float h, bool fill, Vec4 colour) {
	render_box(Vec2(x, y), Vec2(w, h), fill, colour);
}

void render_box2(float top, float left, float bottom, float right, bool fill, Vec4 colour) {
	if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}	

	glColor4f(v4parms(colour));
	glBegin(GL_QUADS);
	glVertex2f(right, top);
	glVertex2f(left, top);
	glVertex2f(left, bottom);
	glVertex2f(right, bottom);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void render_circle(Vec2 position, float radius, bool fill, Vec4 colour) {
    if (fill) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

    glColor4f(v4parms(colour));

    glBegin(GL_POLYGON);
	int num_segments = 30; //@todo: make this scale for larger radius
    for(int i = 0; i < num_segments; i++) {
        float theta = 2.0f * math_pi * (float)i / (float)num_segments;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex2f(position.x + x, position.y + y);
    }
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void render_circle(float x, float y, float radius, bool fill, Vec4 colour) {
	render_circle(Vec2(x, y), radius, fill, colour);
}

void render_point(Vec2 position, float size, Vec4 colour) {
	glColor4f(v4parms(colour));
	glPointSize(size);
	glBegin(GL_POINTS);
	glVertex2f(position.x, position.y);
	glEnd();
}

void render_point(float x, float y, float size, Vec4 colour) {
	render_point(Vec2(x, y), size, colour);
}

void opengl_debug_output_to_file(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, const char* message, bool should_print) {
       FILE* f = nullptr;
       fopen_s(&f, "data/opengl.log", "a");
       if(f) {
			char debSource[16] = {};
			char debType[20] = {};
			char debSev[5] = {};

			switch(source) {
			case GL_DEBUG_SOURCE_API_ARB:
				strcpy_s(debSource, "OpenGL");
				break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				strcpy_s(debSource, "Shader Compiler");
				break;
			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
			    strcpy_s(debSource, "Third Party");
				break;
			case GL_DEBUG_SOURCE_APPLICATION_ARB:
			    strcpy_s(debSource, "Application");
				break;
			case GL_DEBUG_SOURCE_OTHER_ARB:
			    strcpy_s(debSource, "Other");
				break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				strcpy_s(debSource, "Windows");
				break;
			}
			
			switch(type) {
			case GL_DEBUG_TYPE_ERROR_ARB:
			    strcpy_s(debType, "Error");
				break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
			    strcpy_s(debType, "Deprecated behavior");
				break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
			    strcpy_s(debType, "Undefined behavior");
				break;
			case GL_DEBUG_TYPE_PORTABILITY_ARB:
			    strcpy_s(debType, "Portability");
				break;
			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
			    strcpy_s(debType, "Performance");
				break;
			case GL_DEBUG_TYPE_OTHER_ARB:
			    strcpy_s(debType, "Other");
				break;
			}
			   
			switch(severity) {
			case GL_DEBUG_SEVERITY_HIGH_ARB:
			    strcpy_s(debSev, "High");
				break;
			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
			    strcpy_s(debSev, "Medium");
				break;
			case GL_DEBUG_SEVERITY_LOW_ARB:
			    strcpy_s(debSev, "Low");
				break;
			}
			
			if(should_print) {
				printf("Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n", debSource, debType, id, debSev, message);
			}
			fprintf(f, "Source:%s\tType:%s\tID:%d\tSeverity:%s\tMessage:%s\n", debSource, debType, id, debSev, message);
			fclose(f);
       }
}

void opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if(id == 131185) {
		/* Message:Buffer detailed info: Buffer object 1 (bound to GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (0), GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (1), GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (2), and GL_ARRAY_BUFFER_ARB, usage hint is GL_STREAM_DRAW) will use VIDEO memory as the source for buffer object operations.*/
		return;
	}

    opengl_debug_output_to_file(source, type, id, severity, message, severity == GL_DEBUG_SEVERITY_MEDIUM_ARB || severity == GL_DEBUG_SEVERITY_HIGH_ARB);
}