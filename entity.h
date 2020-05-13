#ifndef __ENTITY_H__
#define __ENTITY_H__


struct Entity {
    Vec2 position;
    Vec2 velocity;
    Vec2 size;
    Texture *texture = nullptr;

    bool _deleted = true;
    int _index = -1;
};

void remove_entity(Entity *entity);
Entity *create_entity();
void set_entity_texture(Entity *entity, const char *file_name);
void update_entities();
void render_entities();

extern Contiguous_Array<Entity, 10000> entities;

#endif