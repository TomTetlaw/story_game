#include "include.h"

struct Label {
    int owner = -1;
    char text[1024] = {};
    Vec2 position;
};

struct Panel {   
    Vec2 position;
    int width = 0;
    int height = 0;
    
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

void ui_init() {
}

void ui_shutdown() {
}

void ui_process_event(SDL_Event *event) {
    if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP) {
        bool down = (event->type == SDL_MOUSEBUTTONDOWN);
        
        Click_Event ev;
        ev.position = Vec2(event->button.x, event->button.y);
        ev.button = event->button.button;
        ev.down = down;
        clicks.append(ev);
    }
}

void ui_new_frame() {
}

void ui_render() {
    render_setup_for_ui();
    
    For(panels) {
        render_box(it.position, Vec2(it.width, it.height), true, Vec4(1, 1, 1, 0.5f));
    }}}

    For(labels) {
        render_string(it.position, it.text);
    }}}

    clicks.num = 0;
    panels.num = 0;
    labels.num = 0;
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
}

void ui_new_panel(Vec2 position, const char *title) {
    assert(current_panel == -1);
    
    Panel new_panel;
    new_panel.position = position;;
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
    label.owner = current_panel;
    label.position = p->position + Vec2(0, p->current_y);
    strcpy_s(label.text, text);
    labels.append(label);
    
    update_width(current_panel, font_get_string_length_in_pixels(renderer.default_font, text));
    new_row(current_panel);
}