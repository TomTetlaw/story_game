#include "include.h"

Contiguous_Array<Entity, 10000> entities;

void update_entity(Entity *entity) {
    entity->position = entity->position + (entity->velocity * delta_time);
}

void render_entity(Entity *entity) {
    Render_Texture rt;
    rt.position = entity->position;
    rt.size = entity->size;
    rt.texture = entity->texture;
    render_texture(&rt);
}

void remove_entity(Entity *entity) {
    entities.remove(entity);
}

Entity *create_entity() {
    return entities.alloc();
}

void set_entity_texture(Entity *entity, const char *file_name) {
    entity->texture = load_texture(file_name);
    if(entity->texture) {
        entity->size = entity->texture->size;
    }
}

void update_entities() {
    for(int i = 0; i < entities.max_index; i++) {
        if(!entities[i]) continue;
        update_entity(entities[i]);
    }
}

void render_entities() {
    for(int i = 0; i < entities.max_index; i++) {
        if(!entities[i]) continue;
        render_entity(entities[i]);
    }
}
