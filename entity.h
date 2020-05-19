#ifndef __ENTITY_H__
#define __ENTITY_H__

struct Physics_Component {
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

    int _index = -1;
    bool _deleted = true;
};

struct Texture_Component {
    Texture *texture = nullptr;
    Vec2 position;
    Vec2 size;

    int _index = -1;
    bool _deleted = true;
};

struct Entity {
    Physics_Component *physics = nullptr;
    Texture_Component *texture = nullptr;

    int _index = -1;
    bool _deleted = true;
};

constexpr int max_entities = 10000;

struct Entity_Manager {
    Contiguous_Array<Physics_Component, max_entities>   physics_components;
    Contiguous_Array<Texture_Component, max_entities>   texture_components;
    Contiguous_Array<Entity, max_entities>              entities;
};

extern Entity_Manager entity_manager;

Entity *create_entity();
void remove_entity(Entity *entity);

void add_physics_component(Entity *entity);
void add_texture_component(Entity *entity);

void entity_init();
void entity_shutdown();
void entity_update();
void entity_render();

#endif