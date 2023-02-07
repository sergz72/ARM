#ifndef _FLASH_FS_H_
#define _FLASH_FS_H_

#define FLASH_FS_MAX_FILE_NAME_LENGTH 13

typedef struct {
  char name[FLASH_FS_MAX_FILE_NAME_LENGTH+1];
  unsigned int size, id;
} FlashFSFileFind;

int FlashFSRead(int offset, void* buffer, unsigned int length);
int FlashFSWrite(int offset, void* buffer, unsigned int length);
int FlashFSErase(int offset, unsigned int length);

int FlashFSMount(void);
int FlashFSUnmount(void);
int FlashFSFormat(void);
int FlashFSSync(void);
int FlashFSFileCreate(const char* file_name, void* contents, unsigned int length);
int FlashFSFileRead(unsigned int id, void* contents);
int FlashFSFileWrite(const char* file_name, void* contents, unsigned int length);
unsigned int FlashFSGetFreeSize(void);
int FlashFSValidateFAT(void);
int FlashFSFindFile(const char* file_name, FlashFSFileFind* ff);
int FlashFSFindFirst(FlashFSFileFind* ff);
int FlashFSFindNext(FlashFSFileFind* ff);
int FlashFSFileDelete(const char* file_name);
int FlashFSFileRename(const char* file_name, const char* new_name);

#endif

