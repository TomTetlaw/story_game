#include "include.h"

bool ui_wants_focus = false;

struct Label {
    char text[1024] = {};
    Vec2 position;
};

struct Panel {   
    Vec2 position;
    int width = 0;
    int height = 0;
    int num_rows = 0;
    int current_y = 0;
};

struct Click_Event {
    Vec2 position;
    int button = 0;
    bool down = false;
};

internal Array<Click_Event> clicks;
internal Array<Panel> panels;
internal Array<Label> labels;
internal int current_panel = -1;

bool box_point_intersection(Vec2 position, Vec2 size, Vec2 point) {
    if(point.x < position.x) return false;
    if(point.y < position.y) return false;
    if(point.x > position.x + size.x) return false;
    if(point.y > position.y + size.y) return false;
    return true;
}

void ui_init() {
}

void ui_shutdown() {
}

void ui_process_event(SDL_Event *event) {
    if(event->type == SDL_MOUSEBUTTONDOWN) {     
        Click_Event ev;
        ev.position = mouse_position;
        ev.button = event->button.button;
        ev.down = true;
        clicks.append(ev);
    } else if(event->type == SDL_MOUSEMOTION) {
        ui_wants_focus = false;
        For(panels) {
            if(box_point_intersection(it.position, Vec2(it.width, it.height), 
                                             mouse_position)) {
                ui_wants_focus = true;
            }
        }}}
    }
}

void ui_new_frame() {
    panels.num = 0;
    labels.num = 0;
}

void ui_render() {
    render_setup_for_ui();
    
    For(panels) {
        Panel *p = &panels[it_index];
        render_box(p->position, Vec2(p->width, p->height), true, Vec4(1, 1, 1, 0.5f));
        
        for(int i = 0; i < p->num_rows; i++) {
            Vec2 box_position = p->position + Vec2(0, i * renderer.default_font->line_skip);
            Vec2 box_size = Vec2(p->width, renderer.default_font->line_skip);
            if(box_point_intersection(box_position, box_size, mouse_position)) {
                render_box(box_position, box_size, true, Vec4(1, 1, 1, 0.8f));
            }
        }
    }}}

    For(labels) {
        render_string(it.position, it.text, Vec4(0, 0, 0, 1));
    }}}

    clicks.num = 0;
}

void update_width(int panel_index, int new_width) {
    assert(panel_index >= 0);
    assert(panel_index < panels.num);
    
    Panel *p = &panels[panel_index];
    if(p->width < new_width) p->width = new_width;
}

void new_row(int panel_index) {
    assert(panel_index >= 0);
    assert(panel_index < panels.num);
    
    Panel *p = &panels[panel_index];
    p->height += renderer.default_font->line_skip;
    p->current_y += renderer.default_font->line_skip;
    p->num_rows++;
}

void ui_new_panel(Vec2 position, const char *title) {
    assert(current_panel == -1);
    
    Panel new_panel;
    new_panel.position = position;
    current_panel = panels.num;
    panels.append(new_panel);
    
    ui_label(title);
}

void ui_end_panel() {
    assert(current_panel != -1);
    current_panel = -1;
}

void ui_label(const char *text) {
    assert(current_panel != -1);
    Panel *p = &panels[current_panel];
    
    Label label;
    label.position = p->position + Vec2(0, p->current_y);
    strcpy_s(label.text, text);
    labels.append(label);
    
    update_width(current_panel, font_get_string_length_in_pixels(renderer.default_font, text));
    new_row(current_panel);
}

bool ui_button(const char *text) {
    assert(current_panel != -1);
    
    ui_label(text);
    
    Panel *p = &panels[current_panel];
    For(clicks) {
        Vec2 box_position = labels[labels.num - 1].position;
        Vec2 box_size = Vec2(p->width, renderer.default_font->line_skip);
        if(box_point_intersection(box_position, box_size, it.position)) {
            return true;
        }
    }}}

    return false;
}