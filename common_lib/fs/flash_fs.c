#include "board.h"
#include <flash_fs.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

#define FILE_ENTRY_COUNT (FLASH_FS_SIZE / FLASH_FS_MINIMUM_ERASE_SIZE - 1)
#define MAX_FREE_SIZE (FLASH_FS_SIZE - FLASH_FS_MINIMUM_ERASE_SIZE)

#define FAT_END_CLUSTER 0xFF

typedef struct {
  unsigned short size;
  unsigned char first_cluster;
  char name[FLASH_FS_MAX_FILE_NAME_LENGTH];
} FileEntry;

typedef struct {
  FileEntry file_entries[FILE_ENTRY_COUNT];
  unsigned int free_size;
  unsigned char cluster_table[FILE_ENTRY_COUNT];
} FileTable;

#define BUFFER_SIZE ((sizeof(FileTable) / FLASH_FS_MINIMUM_PROGRAM_SIZE + 1) * FLASH_FS_MINIMUM_PROGRAM_SIZE)

typedef union {
  FileTable file_table;
  char buffer[BUFFER_SIZE];
} FileTableBuffer;

typedef struct {
  FileTableBuffer fat;
  char file_buffer[FLASH_FS_MINIMUM_PROGRAM_SIZE];
} FlashFSData;

static FlashFSData *data = NULL;

int FlashFSValidateFAT(void)
{
  int i, free_size, used_size, file_size, sz;
  unsigned char c;
  FileEntry* fe;

  if (data->fat.file_table.free_size > MAX_FREE_SIZE)
    return ENODEV;
  fe = data->fat.file_table.file_entries;
  free_size = used_size = 0;
  for (i = 0; i < FILE_ENTRY_COUNT; i++)
  {
    c = data->fat.file_table.cluster_table[i];
    if (!c)
      free_size += FLASH_FS_MINIMUM_ERASE_SIZE;
    else if (c != FAT_END_CLUSTER && c > FILE_ENTRY_COUNT)
      return ENODEV;
    c = fe->first_cluster;
    if (c)
    {
      if (c > FILE_ENTRY_COUNT)
        return ENODEV;
      file_size = 0;
      while (c != FAT_END_CLUSTER)
      {
        file_size++;
        c = data->fat.file_table.cluster_table[c - 1];
        if (!c || (c != FAT_END_CLUSTER && c > FILE_ENTRY_COUNT))
          return ENODEV;
      }
      sz = fe->size / FLASH_FS_MINIMUM_ERASE_SIZE + ((fe->size % FLASH_FS_MINIMUM_ERASE_SIZE) ? 1 : 0);
      if (file_size != sz)
        return ENODEV;
      used_size += file_size * FLASH_FS_MINIMUM_ERASE_SIZE;
    }
    fe++;
  }

  if (data->fat.file_table.free_size != free_size)
    return ENODEV;

  if (used_size + free_size != MAX_FREE_SIZE)
    return ENODEV;

  return 0;
}

int FlashFSMount(void)
{
  int rc;

  if (data)
    return EBUSY;

  data = malloc(sizeof(FlashFSData));
  if (!data)
    return ENOMEM;

  if (FlashFSRead(0, data->fat.buffer, sizeof(data->fat)))
  {
    free(data);
    data = NULL;
    return EIO;
  }

  rc = FlashFSValidateFAT();
  if (rc)
  {
    free(data);
    data = NULL;
  }
  return rc;
}

int FlashFSUnmount(void)
{
  int rc;
  if (!data)
    return ENODEV;
  rc = FlashFSSync();
  free(data);
  data = NULL;
  return rc;
}

int FlashFSFormat(void)
{
  if (!data)
  {
    data = malloc(sizeof(FlashFSData));
    if (!data)
      return ENOMEM;
  }
  memset(data->fat.buffer, 0xFF, sizeof(data->fat));
  memset(&data->fat.file_table, 0, sizeof(data->fat.file_table));
  data->fat.file_table.free_size = MAX_FREE_SIZE;
  return FlashFSSync();
}

int FlashFSSync(void)
{
  if (!data)
    return ENODEV;
  if (FlashFSErase(0, FLASH_FS_MINIMUM_ERASE_SIZE))
    return EIO;
  if (FlashFSWrite(0, data->fat.buffer, sizeof(data->fat)))
    return EIO;
  return 0;
}

static int FlashFSInternalFileFind(const char* file_name, FileEntry** entry)
{
  int i;
  FileEntry* fe;

  if (!file_name || !file_name[0] || strlen(file_name) > FLASH_FS_MAX_FILE_NAME_LENGTH)
    return EINVAL;

  fe = data->fat.file_table.file_entries;

  for (i = 0; i < FILE_ENTRY_COUNT; i++)
  {
    if (fe->first_cluster && !strncmp(file_name, fe->name, FLASH_FS_MAX_FILE_NAME_LENGTH))
    {
      if (entry)
        *entry = fe;
      return 0;
    }
    fe++;
  }

  return ENOENT;
}

static int FlashFSInternalWrite(FileEntry* fe, void* vcontents)
{
  unsigned char c;
  int offset;
  unsigned int length;
  char* contents = vcontents;

  c = fe->first_cluster;
  length = fe->size;

  while (length > 0)
  {
    offset = c * FLASH_FS_MINIMUM_ERASE_SIZE;
    if (FlashFSErase(offset, FLASH_FS_MINIMUM_ERASE_SIZE))
      return EIO;
    if (length >= FLASH_FS_MINIMUM_ERASE_SIZE)
    {
      if (FlashFSWrite(offset, contents, FLASH_FS_MINIMUM_ERASE_SIZE))
        return EIO;
      length -= FLASH_FS_MINIMUM_ERASE_SIZE;
      contents += FLASH_FS_MINIMUM_ERASE_SIZE;
      c = data->fat.file_table.cluster_table[c - 1];
    }
    else
    {
      while (length >= FLASH_FS_MINIMUM_PROGRAM_SIZE)
      {
        if (FlashFSWrite(offset, contents, FLASH_FS_MINIMUM_PROGRAM_SIZE))
          return EIO;
        length -= FLASH_FS_MINIMUM_PROGRAM_SIZE;
        contents += FLASH_FS_MINIMUM_PROGRAM_SIZE;
        offset += FLASH_FS_MINIMUM_PROGRAM_SIZE;
      }
      if (length)
      {
        memset(data->file_buffer, 0xFF, FLASH_FS_MINIMUM_PROGRAM_SIZE);
        memcpy(data->file_buffer, contents, length);
        if (FlashFSWrite(offset, data->file_buffer, FLASH_FS_MINIMUM_PROGRAM_SIZE))
          return EIO;
        return 0;
      }
    }
  }
  return 0;
}

static int FlashFSBuildClusterList(FileEntry* fe, unsigned int length)
{
  int i, last_cluster;
  unsigned char* c = data->fat.file_table.cluster_table;

  last_cluster = 0;
  for (i = 1; i <= FILE_ENTRY_COUNT; i++)
  {
    if (!*c)
    {
      data->fat.file_table.free_size -= FLASH_FS_MINIMUM_ERASE_SIZE;
      if (!fe->first_cluster)
        fe->first_cluster = i;
      else
        data->fat.file_table.cluster_table[last_cluster] = i;
      last_cluster = i - 1;

      if (length <= FLASH_FS_MINIMUM_ERASE_SIZE)
      {
        *c = FAT_END_CLUSTER;
        return 0;
      }
      length -= FLASH_FS_MINIMUM_ERASE_SIZE;
    }
    c++;
  }

  return ENOMEM;
}

static int FlashFSContinueClusterList(unsigned int cluster, unsigned int delta)
{
  unsigned int i;
  unsigned char* c = data->fat.file_table.cluster_table;

  for (i = 1; i <= FILE_ENTRY_COUNT; i++)
  {
    if (!*c)
    {
      data->fat.file_table.free_size -= FLASH_FS_MINIMUM_ERASE_SIZE;
      data->fat.file_table.cluster_table[cluster] = i;
      cluster = i - 1;

      if (!--delta)
      {
        *c = FAT_END_CLUSTER;
        return 0;
      }
    }
    c++;
  }

  return ENOMEM;
}

static int FlashFSUpdateClusterList(FileEntry* fe, unsigned int length)
{
  unsigned int delta, cluster, current_count, required_count;
  
  current_count = fe->size / FLASH_FS_MINIMUM_ERASE_SIZE;
  if (fe->size % FLASH_FS_MINIMUM_ERASE_SIZE)
    current_count++;
  required_count = length / FLASH_FS_MINIMUM_ERASE_SIZE;
  if (length % FLASH_FS_MINIMUM_ERASE_SIZE)
    required_count++;
  if (required_count > current_count)
  {
    delta = required_count - current_count;
    if (delta * FLASH_FS_MINIMUM_ERASE_SIZE > data->fat.file_table.free_size)
      return ENOSPC;

    cluster = fe->first_cluster - 1;

    while (required_count--)
    {
      if (current_count == 1)
        return FlashFSContinueClusterList(cluster, delta);
      else
        current_count--;
      cluster = data->fat.file_table.cluster_table[cluster] - 1;
    }
  }
  else if (required_count < current_count)
  {
    cluster = fe->first_cluster - 1;

    while (current_count--)
    {
      delta = data->fat.file_table.cluster_table[cluster] - 1;
      if (!required_count)
      {
        data->fat.file_table.cluster_table[cluster] = 0;
        data->fat.file_table.free_size += FLASH_FS_MINIMUM_ERASE_SIZE;
      }
      else
      {
        if (required_count == 1)
          data->fat.file_table.cluster_table[cluster] = FAT_END_CLUSTER;
        required_count--;
      }
      cluster = delta;
    }
  }

  return 0;
}

int FlashFSFileCreate(const char* file_name, void *contents, unsigned int length)
{
  int i;
  FileEntry* fe;

  if (!data)
    return ENODEV;

  if (length == 0 || !contents)
    return EINVAL;

  if (length > data->fat.file_table.free_size)
    return ENOSPC;

  if (!FlashFSInternalFileFind(file_name, NULL))
    return EEXIST;

  fe = data->fat.file_table.file_entries;
  for (i = 0; i < FILE_ENTRY_COUNT; i++)
  {
    if (!fe->first_cluster)
    {
      strncpy(fe->name, file_name, FLASH_FS_MAX_FILE_NAME_LENGTH);
      i = FlashFSBuildClusterList(fe, length);
      if (i)
        return i;
      fe->size = length;
      return FlashFSInternalWrite(fe, contents);
    }
    fe++;
  }

  return ENFILE;
}

int FlashFSFileDelete(const char* file_name)
{
  FileEntry* fe;
  unsigned char c, next;

  if (!data)
    return ENODEV;

  int rc = FlashFSInternalFileFind(file_name, &fe);
  if (rc)
    return rc;

  c = fe->first_cluster;
  while (c != FAT_END_CLUSTER)
  {
    if (!c || c > FILE_ENTRY_COUNT)
      return ENOMEM;
    next = data->fat.file_table.cluster_table[c - 1];
    data->fat.file_table.cluster_table[c - 1] = 0;
    data->fat.file_table.free_size += FLASH_FS_MINIMUM_ERASE_SIZE;
    c = next;
  }

  fe->first_cluster = 0;

  return 0;
}

int FlashFSFileRead(unsigned int id, void* vcontents)
{
  FileEntry* fe;
  int l, cluster, count;
  char* contents;

  if (!data)
    return ENODEV;

  contents = vcontents;

  if (id >= FILE_ENTRY_COUNT)
    return EINVAL;
  fe = &data->fat.file_table.file_entries[id];
  cluster = fe->first_cluster;
  if (!cluster)
    return EINVAL;

  l = fe->size;
  while (l)
  {
    count = l <= FLASH_FS_MINIMUM_ERASE_SIZE ? l : FLASH_FS_MINIMUM_ERASE_SIZE;
    if (FlashFSRead(cluster * FLASH_FS_MINIMUM_ERASE_SIZE, contents, count))
      return EIO;
    l -= count;
    cluster = data->fat.file_table.cluster_table[cluster - 1];
    contents += count;
  }

  return 0;
}

int FlashFSFileWrite(const char* file_name, void* contents, unsigned int length)
{
  FileEntry* fe;

  if (!data)
    return ENODEV;

  if (!contents || !length)
    return EINVAL;

  int rc = FlashFSInternalFileFind(file_name, &fe);
  if (rc)
    return rc;

  rc = FlashFSUpdateClusterList(fe, length);
  if (rc)
    return rc;
  fe->size = length;

  return FlashFSInternalWrite(fe, contents);
}

unsigned int FlashFSGetFreeSize(void)
{
  if (!data)
    return 0;
  return data->fat.file_table.free_size;
}

int FlashFSFindFile(const char* file_name, FlashFSFileFind* ff)
{
  FileEntry* fe;

  if (!data)
    return ENODEV;

  int rc = FlashFSInternalFileFind(file_name, &fe);
  if (rc)
    return rc;

  ff->size = fe->size;
  ff->id = fe - data->fat.file_table.file_entries;

  return 0;
}

int FlashFSFindFirst(FlashFSFileFind *ff)
{
  int i;
  FileEntry* fe;

  if (!data)
    return ENODEV;

  fe = data->fat.file_table.file_entries;

  for (i = 0; i < FILE_ENTRY_COUNT; i++)
  {
    if (fe->first_cluster)
    {
      ff->id = i;
      ff->size = fe->size;
      strncpy(ff->name, fe->name, FLASH_FS_MAX_FILE_NAME_LENGTH);
      ff->name[FLASH_FS_MAX_FILE_NAME_LENGTH] = 0;
      return 0;
    }
    fe++;
  }

  return ENOENT;
}

int FlashFSFindNext(FlashFSFileFind *ff)
{
  unsigned int i;
  FileEntry* fe;

  if (!data)
    return ENODEV;

  i = ff->id + 1;
  fe = data->fat.file_table.file_entries;

  fe += i;
  for (; i < FILE_ENTRY_COUNT; i++)
  {
    if (fe->first_cluster)
    {
      ff->id = i;
      ff->size = fe->size;
      strncpy(ff->name, fe->name, FLASH_FS_MAX_FILE_NAME_LENGTH);
      ff->name[FLASH_FS_MAX_FILE_NAME_LENGTH] = 0;
      return 0;
    }
    fe++;
  }

  return ENOENT;
}

int FlashFSFileRename(const char* file_name, const char* new_name)
{
  int rc;
  FileEntry* fe;

  if (!data)
    return ENODEV;

  if (!new_name || !new_name[0] || strlen(new_name) > FLASH_FS_MAX_FILE_NAME_LENGTH)
    return EINVAL;

  rc = FlashFSInternalFileFind(file_name, &fe);
  if (rc)
    return rc;

  if (!FlashFSInternalFileFind(new_name, NULL))
    return EEXIST;

  strncpy(fe->name, new_name, FLASH_FS_MAX_FILE_NAME_LENGTH);

  return 0;
}
