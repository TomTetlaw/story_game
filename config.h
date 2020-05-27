#ifndef __CONFIG_H__
#define __CONFIG_H__

struct Config_Node {
    Config_Node *next = nullptr;
    Config_Node *children = nullptr;
};

struct Config_File {
    const char *file_name = nullptr;
    Array<Config_Node *> nodes;
};

bool load_config_file(Config_File *config, const char *file_name);

#endif