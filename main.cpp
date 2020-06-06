#define IMGUI_IMPLIMENTATION
#include "include.h"

struct Star {
    Vec2 position;
    Vec4 colour;
    Vec2 cross_size;
    float point_size = 0;
    bool is_point = false;
};

enum Resources {
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

struct Process {
    int needs[8] = {};
    int produces = 0;
};

struct Planet {
    Vec2 position;
    float radius = 0;
    Vec4 colour;
    
    Planet_Type type;
    
    Texture *texture = nullptr;
    Process processes[8];
    int num_processes = 0;
    Process *active_process = nullptr;
};

void setup_planet_for_type(Planet *planet) {
    switch(planet->type) {
        case PT_EARTH_LIKE:
        planet->num_processes = 3;
        planet->processes[0].needs[0] = RES_FOOD;
        planet->processes[0].produces = RES_PEOPLE;
        
        planet->processes[1].needs[0] = RES_PEOPLE;
        planet->processes[1].needs[1] = RES_ORGANICS;
        planet->processes[1].produces = RES_FOOD;
        
        planet->processes[2].needs[0] = RES_PEOPLE;
        planet->processes[2].produces = RES_ORGANICS;
        
        planet->active_process = &planet->processes[1];
        
        planet->texture = load_texture("data/textures/earth_like.png");
        break;
        case PT_ROCKY:
        planet->num_processes = 4;
        planet->processes[0].needs[0] = RES_PEOPLE;
        planet->processes[0].produces = RES_ORE;
        
        planet->processes[1].needs[0] = RES_ROBOTS;
        planet->processes[1].produces = RES_ORE;
        
        planet->processes[2].needs[0] = RES_PEOPLE;
        planet->processes[2].produces = RES_TECH;
        
        planet->processes[3].needs[0] = RES_ROBOTS;
        planet->processes[3].produces = RES_TECH;
        
        planet->active_process = &planet->processes[3];
        
        planet->texture = load_texture("data/textures/rocky.png");
        break;
        case PT_FORGE_WORLD:
        planet->num_processes = 3;
        planet->processes[0].needs[0] = RES_PEOPLE;
        planet->processes[0].produces = RES_TECH;
        
        planet->processes[1].needs[0] = RES_ROBOTS;
        planet->processes[1].produces = RES_TECH;
        
        planet->processes[2].needs[0] = RES_PEOPLE;
        planet->processes[2].needs[1] = RES_TECH;
        planet->processes[2].produces = RES_ROBOTS;
        
        planet->active_process = &planet->processes[2];
        
        planet->texture = load_texture("data/textures/forge_world.png");
        break;
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
    int distSq = (apos.x - bpos.x) * (apos.x - bpos.x) + 
        (apos.y - bpos.y) * (apos.y - bpos.y);
    int radSumSq = (aradius + bradius) * (aradius + bradius); 
    if(distSq > radSumSq) return false;
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
        render_texture(&rt);
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
            if(p) selected_planet = p;
            else  selected_planet = -1;
        }
    }
}

void input_key(int key, bool down, uint mods) {
}

void panel_for_planet(Planet *planet) {
    ui_new_panel(to_ui_pos(planet->position), "Planet");
    
    for(int i = 0; i < planet->num_processes; i++) {
        char buffer[1024] = {};
        
        Process *p = &planet->processes[i];
        
        int needs_num = 0;
        while(true) {
            if(!p->needs[needs_num]) break;
            
            if(needs_num > 0) strcat_s(buffer, " + ");
            
            switch(p->needs[needs_num]) {
                case RES_PEOPLE: strcat_s(buffer, "people"); break;
                case RES_ORGANICS: strcat_s(buffer, "organics"); break;
                case RES_FOOD: strcat_s(buffer, "food"); break;
                case RES_ROBOTS: strcat_s(buffer, "robots"); break;
                case RES_ORE: strcat_s(buffer, "ore"); break;
                case RES_TECH: strcat_s(buffer, "tech"); break;
            }
            
            needs_num++;
        }
        
        strcat_s(buffer, " = ");
        
        switch(p->produces) {
            case RES_PEOPLE: strcat_s(buffer, "people"); break;
            case RES_ORGANICS: strcat_s(buffer, "organics"); break;
            case RES_FOOD: strcat_s(buffer, "food"); break;
            case RES_ROBOTS: strcat_s(buffer, "robots"); break;
            case RES_ORE: strcat_s(buffer, "ore"); break;
            case RES_TECH: strcat_s(buffer, "tech"); break;
        }
        
        ui_label(buffer);
    }
    
    ui_end_panel();
}

int main() {
    sys_init();
    
    generate_stars();
    generate_planets();
    
    bool middle_mouse_down = false;
    
    bool running = true;
    SDL_Event event;
    while(running) {
        if(SDL_PollEvent(&event)) {
            ui_process_event(&event);
            
            if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
                running = false;
                break;
            }
            
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
        } else {
            sys_update();
            
            ui_new_frame();
            
            if(selected_planet != -1) {
                Planet *p = &planets[selected_planet];
                panel_for_planet(p);
            }
            
            render_begin_frame();
            
            render_setup_for_world();
            
            render_stars();
            render_grid();
            render_planets();
            
            ui_render();
            
            render_end_frame();
            
            frame_num++;
        }
    }
    
    sys_shutdown();
    
    return 0;
} 