#ifndef _FAT16_NOCACHE_H
#define _FAT16_NOCACHE_H

const char *FileOperationsStart(void);
unsigned int FileReadFirstBlock(const char *file_name, void **buffer, const char **error_message);
unsigned int FileReadNextBlock(const char **error_message);
unsigned int FileGetSize(const char *file_name, const char **error_message);
void FileOperationsEnd(void);

#endif
