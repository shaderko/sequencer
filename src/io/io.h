/**
 * @file io.h
 * @author shaderko
 * @brief Module for working with files
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef IO_H
#define IO_H

#include <stdlib.h>

typedef struct Serialized Serialized;
struct Serialized
{
    unsigned char *buffer;
    size_t size;
};

void clear_file(const char *path);
int save(Serialized data, const char *path);
int save_all(Serialized *data_array, size_t count, const char *path);
int get_serialized_sizes(size_t **sizes, size_t *count, const char *path);
int load(Serialized *data, const char *path, size_t start);
int load_all(Serialized ***data, size_t *count, const char *path);

#endif