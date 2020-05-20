#ifndef __CONFIG_H__
#define __CONFIG_H__

struct Config_File {
    const char *file_name = nullptr;
};

bool load_config_file(Config_File *config, const char *file_name);

#endif