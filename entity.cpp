#include "include.h"

Entity_Manager entity_manager;

Entity *create_entity() {
    return entity_manager.entities.alloc();
}

void remove_entity(Entity *entity) {
    entity_manager.physics_components.remove(entity->physics);
    entity_manager.texture_components.remove(entity->texture);
    entity_manager.entities.remove(entity);
}

void add_physics_component(Entity *entity) {
    entity->physics = entity_manager.physics_components.alloc();
}

void add_texture_component(Entity *entity) {
    entity->texture = entity_manager.texture_components.alloc();
}

void entity_init() {
    entity_manager.physics_components.init();
    entity_manager.texture_components.init();
    entity_manager.entities.init();
}

void entity_shutdown() {
}

void entity_update() {
    For(entity_manager.physics_components) {
        if(!it) continue;
        it->position = it->position + (it->velocity * delta_time);
    }}}

    For(entity_manager.entities) {
        if(!it) continue;
        
        it->texture->position = it->physics->position;
        it->texture->size = it->physics->size;
    }}}
}

void entity_render() {
    For(entity_manager.texture_components) {
        if(!it) continue;
        
        Render_Texture rt;
        rt.position = it->position;
        rt.size = it->size;
        rt.texture = it->texture;
        render_texture(&rt);
    }}}
}