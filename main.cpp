#define IMGUI_IMPLIMENTATION
#include "include.h"

struct Star {
    Vec2 position;
    Vec4 colour;
    Vec2 cross_size;
    float point_size = 0;
    bool is_point = false;
};

enum Resource_Type {
    RES_PEOPLE = 1,
    RES_ORGANICS,
    RES_FOOD,
    RES_ROBOTS,
    RES_ORE,
    RES_TECH,
};

enum Planet_Type {
    PT_EARTH_LIKE = 0,
    PT_ROCKY = 1,
    PT_FORGE_WORLD = 2,
    
    PT_NUM_TYPES,
};

struct Resource {
    const char *name = nullptr;
    Resource_Type type;
    float amount = 0;
};

// these instances are copied into the processes
Resource res_people   = { "People",   RES_PEOPLE,   0.0f };
Resource res_organics = { "Organics", RES_ORGANICS, 0.0f };
Resource res_food     = { "Food",     RES_FOOD,     0.0f };
Resource res_robots   = { "Robots",   RES_ROBOTS,   0.0f };
Resource res_ore      = { "Ore",      RES_ORE,      0.0f };
Resource res_tech     = { "Tech",     RES_TECH,     0.0f };

struct Process {
    Array<Resource> needs;
    Resource produces;
};

struct Planet {
    Vec2 position;
    float radius = 0;
    Vec4 colour;
    
    Planet_Type type;
    
    Array<Process> processes;
    
    Texture *texture = nullptr;
    Process *active_process = nullptr;
    
    Array<Planet *> connections;
};

void setup_planet_for_type(Planet *planet) {
    switch(planet->type) {
        case PT_EARTH_LIKE: {       
            Process p0, p1, p2;
            
            p0.needs.append(res_food);
            p0.produces = res_people;
            
            p1.needs.append(res_people);
            p1.needs.append(res_organics);
            p1.produces = res_food;
            
            p2.needs.append(res_people);
            p2.produces = res_organics;
            
            planet->processes.append(p0);
            planet->processes.append(p1);
            planet->processes.append(p2);
            
            planet->texture = load_texture("data/textures/earth_like.png");
        } break;
        case PT_ROCKY: {
            Process p0, p1, p2, p3;
            
            p0.needs.append(res_people);
            p0.produces = res_ore;
            
            p1.needs.append(res_robots);
            p1.produces = res_ore;
            
            p2.needs.append(res_people);
            p2.needs.append(res_ore);
            p2.produces = res_tech;
            
            p3.needs.append(res_robots);
            p3.needs.append(res_ore);
            p3.produces = res_tech;
            
            planet->processes.append(p0);
            planet->processes.append(p1);
            planet->processes.append(p2);
            planet->processes.append(p3);
            
            planet->texture = load_texture("data/textures/rocky.png");
        } break; 
        case PT_FORGE_WORLD: {
            Process p0, p1, p2;
            
            p0.needs.append(res_people);
            p0.produces = res_tech;
            
            p1.needs.append(res_robots);
            p1.produces = res_tech;
            
            p2.needs.append(res_people);
            p2.needs.append(res_tech);
            p2.produces = res_robots;
            
            planet->processes.append(p0);
            planet->processes.append(p1);
            planet->processes.append(p2);
            
            planet->texture = load_texture("data/textures/forge_world.png");
        } break;
    }
}

Array<Star> stars;
Array<Planet> planets;
int selected_planet = -1;

void generate_stars() {
    for(int y = -25; y < 25; y++) {
        for(int x = -25; x < 25; x++) {
            Vec2 offset = Vec2(random_int(-window_width, window_width), random_int(-window_height, window_height));
            Star star;
            star.position = Vec2(x, y) + offset;
            star.colour = Vec4(1, 1, 1, random_float(0.1f, 0.4f));
            star.is_point = random_int(0, 10) > 7;
            if(star.is_point) star.point_size = random_float(1,3);
            else star.cross_size = Vec2(random_float(4, 8), random_float(1, 2));
            stars.append(star);
        }
    }    
}

void render_stars() {
    render_setup_for_world();
    
    For(stars) {
        if(it.is_point) {
            render_point(it.position, it.point_size, it.colour);
        } else {
            render_box(it.position, Vec2(it.cross_size.x, it.cross_size.y), true, it.colour);
            render_box(it.position, Vec2(it.cross_size.y, it.cross_size.x), true, it.colour);
        }
    }}}
}

int circle_circle_intersection(Vec2 apos, Vec2 bpos, float aradius, float bradius) {  
    float dist_squared = apos.distance_to_sqr(bpos);
    int radSumSq = (aradius + bradius) * (aradius + bradius); 
    if(dist_squared > radSumSq) return false;
    return true;
} 

void generate_planets() {
    for(int y = -4; y < 4; y++) {
        for(int x = -4; x < 4; x++) {
            Vec2 offset = Vec2(random_int(-window_width, window_width), random_int(-window_height, window_height));
            Planet planet;
            planet.position = Vec2(x, y) + offset;
            planet.colour = Vec4(1, 1, 1, 1);
            planet.radius = random_float(20, 40);
            planet.type = (Planet_Type)random_int(0, 2);
            setup_planet_for_type(&planet);
            planets.append(planet);
        }
    }
    
    for(int j = 0; j < planets.num; j++) {
        for(int i = 0; i < planets.num; i++) {
            if(i == j) continue;
            
            Planet *a = &planets[i];
            Planet *b = &planets[j];
            
            if(circle_circle_intersection(a->position, b->position, a->radius + 10, b->radius + 10)) {
                planets.remove(i);
            }
        }
    }
}

bool circle_point_intersection(Vec2 position, float radius, Vec2 point) {
    if(position.distance_to(point) > radius) return false;
    return true;
}

#define mouse_to_world to_world_pos(mouse_position - (window_size * 0.5f))

void string_for_process(Process *p, char dest[1024]) {
    For(p->needs) {
        if(it_index > 0) strcat_s(dest, 1024, " + ");
        strcat_s(dest, 1024, it.name);
    }}}

    strcat_s(dest, 1024, " = ");    
    strcat_s(dest, 1024, p->produces.name);
}

void render_planets() {
    For(planets) {       
        Vec4 colour = it.colour;
        if(it_index == selected_planet) {
            colour = Vec4(1, 0, 0, 1);
        }
        
        Render_Texture rt;
        rt.position = it.position;
        rt.texture = it.texture;
        rt.size = Vec2(it.radius, it.radius);
        rt.colour = colour;
        render_texture(&rt);        
        
        if(it.active_process) {
            char buffer[1024] = {};
            string_for_process(it.active_process, buffer);
            render_string_format_lazy(it.position + Vec2(0, it.radius), "%s = %f", buffer, it.active_process->produces.amount);
            
            if(it_index == selected_planet) {
                render_line(it.position, mouse_to_world);
            }
        }
        
        for(int i = 0; i < it.connections.num; i++) {
            render_line(it.position, it.connections[i]->position);
        }
    }}}
}

void update_planets() {
    For(planets) {
        if(!it.active_process) continue;        
        
        int needs_met = 0;
        for(int i = 0; i < it.connections.num; i++) {
            Planet *p = it.connections[i];
            Resource_Type type = p->active_process->produces.type;
            
            for(int needs_num = 0; needs_num < it.active_process->needs.num; needs_num++) {
                Resource *r = &it.active_process->needs[needs_num];
                if(r->type == type) {
                    needs_met++;
                }
            }
        }
        
        it.active_process->produces.amount += (needs_met * delta_time);
    }}}
}

void render_grid() {
    float total_width = window_width;
    float total_height = window_height;
    float grid_width = 64;
    float grid_height = 64;
    int num_x = total_width / grid_width;
    int num_y = total_height / grid_height;
    
    Vec4 grid_colour = Vec4(0, 0, 1, 0.3f);
    
    for(int x = -num_x; x < num_x; x++) {            
        render_line(Vec2(x * grid_width, -total_height), Vec2(x * grid_width, total_height), grid_colour);
    }
    for(int y = -num_y; y < num_y; y++) {
        render_line(Vec2(-total_width, y * grid_height), Vec2(total_width, y * grid_height), grid_colour);
    }
}

int find_planet_at_point(Vec2 point) {
    For(planets) {
        if(circle_point_intersection(it.position, it.radius, point)) {
            return it_index;
        }
    }}}

    return -1;
}

#define KEY_MOD_CTRL 1
#define KEY_MOD_ALT 2
#define KEY_MOD_SHIFT 4

void input_mouse(Vec2 position, int button, bool down) {
    if(down) {
        if(button == SDL_BUTTON_LEFT) {
            int p = find_planet_at_point(mouse_to_world);
            if(p != -1) {
                if(selected_planet == -1) {
                    selected_planet = p;
                } else {
                    if(planets[p].active_process) {
                        planets[selected_planet].connections.append(&planets[p]);
                        planets[p].connections.append(&planets[selected_planet]);
                        selected_planet = -1;
                    }
                }
            }
            else  selected_planet = -1;
        }
    }
}

int generation_seed = 0;

void generate() {
    generation_seed = time(0);
    srand(generation_seed);
    
    stars.num = 0;
    planets.num = 0;
    selected_planet = -1;
    
    generate_stars();
    generate_planets();
}

void input_key(int key, bool down, uint mods) {
    if(key == SDL_SCANCODE_SPACE && down) generate();
}

void panel_for_planet(Planet *planet) {
    ui_new_panel(to_ui_pos(planet->position) + Vec2(planet->radius, 0), "Planet");
    
    for(int i = 0; i < planet->processes.num; i++) {
        char buffer[1024] = {};
        Process *p = &planet->processes[i];
        string_for_process(p, buffer);
        if(ui_button(buffer)) {
            planet->active_process = p;
            selected_planet = -1;
        }
    }
    
    ui_end_panel();
}

int main() {
    sys_init();
    
    generate();
    
    bool middle_mouse_down = false;
    
    bool running = true;
    SDL_Event event;
    while(running) {
        if(SDL_PollEvent(&event)) {
            ui_process_event(&event);
            if(!ui_wants_focus) {
                if(event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
                    bool down = (event.type == SDL_MOUSEBUTTONDOWN);
                    input_mouse(Vec2(event.button.x, event.button.y), event.button.button, down);
                    if(event.button.button == SDL_BUTTON_MIDDLE) middle_mouse_down = down;
                } else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                    int mods = 0;
                    if(event.key.keysym.mod | KMOD_CTRL)    mods |= KEY_MOD_CTRL;
                    if(event.key.keysym.mod | KMOD_ALT)     mods |= KEY_MOD_ALT;
                    if(event.key.keysym.mod | KMOD_SHIFT)   mods |= KEY_MOD_SHIFT;
                    bool down = (event.type == SDL_KEYDOWN);
                    input_key(event.key.keysym.scancode, down, mods);
                } else if(event.type == SDL_MOUSEMOTION) {
                    Vec2 amount = Vec2(event.motion.xrel, event.motion.yrel);
                    if(middle_mouse_down) renderer.camera_position = renderer.camera_position - amount;
                }
            }
            
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = false;
                break;
            }
        } else {
            sys_update();
            
            ui_new_frame();
            
            if(selected_planet != -1) {
                Planet *p = &planets[selected_planet];
                if(!p->active_process) panel_for_planet(p);
            }
            
            update_planets();
            
            render_begin_frame();
            
            render_setup_for_world();
            
            render_stars();
            render_grid();
            render_planets();
            
            ui_render();
            
            render_setup_for_ui();
            debug_string("mouse_position  = (%.2f, %.2f)", v2parms(mouse_position));
            debug_string("mouse_to_world  = (%.2f, %.2f)", v2parms(mouse_to_world));
            debug_string("ui_wants_focus  = %s",           ui_wants_focus ? "true" : "false");
            debug_string("generation_seed = %d",           generation_seed);
            render_end_frame();
            
            frame_num++;
        }
    }
    
    sys_shutdown();
    
    return 0;
} 