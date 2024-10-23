#ifndef OBJECT_FILE_LOADER_H
#define OBJECT_FILE_LOADER_H

typedef struct
{
    char *name;
    void *pointer;
} function_def;

int object_file_load(void *data, const function_def *function_map, void *bss, unsigned int bss_size);
int object_file_call(const char* function);

#endif
