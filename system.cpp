#include "include.h"

System sys;
int window_width = 0;
int window_height = 0;
float now = 0.0f;
float delta_time = 0.0f;
int frame_num = 0;
Vec2 mouse_position;

void sys_init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        sys_error("Failed to initialize SDL: %s.", SDL_GetError());
    }

    sys.window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1366, 768, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
    if(!sys.window) sys_error("Failed to create window: %s.", SDL_GetError());

    window_width = 1366;
    window_height = 768;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	sys.context = SDL_GL_CreateContext(sys.window);
    if(!sys.context) sys_error("Failed to create OpenGL context: %s.", SDL_GetError());

    const unsigned char *version = glGetString(GL_VERSION);
    printf("OpenGL Version: %s\n", version);

    GLenum result = glewInit();
    if(result != GLEW_OK) sys_error("Failed to initialize glew: %s.", glewGetErrorString(result));

    if(!IMG_Init(IMG_INIT_PNG)) sys_error("Failed to initialize SDL_image: %s.", IMG_GetError());

    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(sys.window, sys.context);
    ImGui_ImplOpenGL3_Init();

    hotload_init();
    renderer_init();
    editor_init();
}

internal float last_time = 0.0f;
void sys_update() {
    float current_time = SDL_GetTicks() / 1000.0f;
    delta_time = current_time - last_time;
    now = current_time;
    last_time = current_time;

    hotload_check_files_non_blocking();

    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    mouse_position = Vec2(x, y);
}

void sys_shutdown() {
    editor_shutdown();
    hotload_shutdown();

    unload_textures();
    renderer_shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    IMG_Quit();

    SDL_GL_MakeCurrent(sys.window, nullptr);
    SDL_GL_DeleteContext(sys.context);
    SDL_DestroyWindow(sys.window);
    SDL_Quit();
    
    exit(0);
}

void sys_error(const char *text, ...) {
	va_list argptr;
	char message[1024];

	va_start(argptr, text);
	vsnprintf_s(message, _TRUNCATE, text, argptr);
	va_end(argptr);

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", message, sys.window);
	sys_shutdown();
}