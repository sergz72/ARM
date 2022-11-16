#include "fat16_nocache.h"
#include "hal.h"
#include "board.h"
#include <string.h>

#define PARTITION_TABLE_OFFSET 0x1BE
#define FAT16_FILESYSTEM_ID 6
#define PARTITION_TABLE_SIGNATURE 0xAA55

#define FILE_ATTRIBUTE_FILE 0
#define FILE_ATTRIBUTE_VOLUME_LABEL 8
#define FILE_ATTRIBUTE_UNUSED 0x80
#define FILE_ATTRIBUTE_DEV 0x40
#define FILE_ATTRIBUTE_SUBDIRECTORY 0x10
#define FILE_ATTRIBUTE_LONG_FILE_NAME 0x0F

#define FILE_NAME_LEN 65

#pragma pack(1)
typedef struct {
  unsigned char active;
  unsigned char start_head_no;
  unsigned short start_sector_and_cylinder;
  unsigned char filesystem_id;
  unsigned char end_head_no;
  unsigned short end_sector_and_cylinder;
  unsigned int first_sector_no;
  unsigned int size_in_sectors;
} partition;

typedef struct {
  unsigned char reserved[PARTITION_TABLE_OFFSET];
  partition partitions[4];
  unsigned short signature;
} partition_table;

typedef struct {
  unsigned char reserved[3];
  char oem_name[8];
  unsigned short bytes_per_sector;
  unsigned char sectors_per_cluster;
  unsigned short reserved_sector_count;
  unsigned char number_of_fats;
  unsigned short number_of_root_dir_entries;
  unsigned short total_sectors;
  unsigned char media_descriptor;
  unsigned short sectors_per_fat;
  unsigned short sectors_per_track;
  unsigned short number_of_heads;
  unsigned int hidden_sector_count;
  unsigned int total_sectors_long;
  unsigned char physical_drive_no;
  unsigned char reserved2;
  unsigned char extended_signature;
  unsigned int serial;
  char volume_label[11];
  char file_system_type[8];
} boot_sector;

typedef struct {
  char file_name[8];
  char file_ext[3];
  unsigned char file_attributes;
  char reserved;
  unsigned char create_time1;
  unsigned short create_time;
  unsigned short create_date;
  unsigned short last_access_date;
  unsigned short ea_index;
  unsigned short last_modified_time;
  unsigned short last_modified_date;
  unsigned short start_cluster;
  unsigned int file_size;
} directory_entry;

typedef struct {
  unsigned char sequence_number;
  short characters[5];
  unsigned char file_attributes;
  unsigned char reserved;
  unsigned char checksum;
  short characters2[6];
  unsigned short first_cluster;
  short characters3[2];
} long_file_name_entry;

typedef struct _FileEntry {
  char FileName[FILE_NAME_LEN];
  unsigned char file_attributes;
  unsigned short start_cluster;
  unsigned int file_size;
} FileEntry;

FileEntry fe;
unsigned int file_cluster, file_sector, file_size_left;

char temp_buffer[BLOCK_SIZE];
char temp_buffer2[BLOCK_SIZE];
char file_n[128];
unsigned int first_fat_sector, first_data_sector, sectors_per_cluster, temp_sector, root_directory_sector_no, root_directory_length;
unsigned int scan_cluster_no, scan_sector_no, scan_sectors_left;
unsigned int prev_cluster;

const char *EFILENOTF = "File not found";

const char *ReadToTempBuffer(unsigned int sector_no)
{
  const char *error_message;

  if (temp_sector != sector_no)
  {
    error_message = BlockDeviceRead(sector_no, temp_buffer);
    if (error_message)
    {
      temp_sector = 0xFFFFFFFF;
      return error_message;
    }
    temp_sector = sector_no;
  }

  return NULL;
}

unsigned short GetFATEntry(unsigned int cluster_no, const char **error_message)
{
  unsigned short *sector;

  unsigned int sector_no = first_fat_sector + cluster_no * sizeof(unsigned short) / BLOCK_SIZE;

  *error_message = ReadToTempBuffer(sector_no);
  if (*error_message)
    return 0;

  sector = (unsigned short*)temp_buffer;
  return sector[cluster_no % (BLOCK_SIZE / sizeof(unsigned short))];
}

unsigned int GetNextCluster(unsigned int cluster_no, const char **error_message)
{
  unsigned int next;

  prev_cluster = cluster_no;

  next = GetFATEntry(cluster_no, error_message);
  if (*error_message)
    return 0;

  if (next >= 0xFFF8)
  {
    *error_message = NULL;
    return 0;
  }

  if (next > 0xFFEF || next < 2)
  {
    *error_message = "Bad FAT entry";
    return 0;
  }

  return next;
}

const char *ReadFromCluster(unsigned int cluster_no, unsigned int sector_no)
{
  sector_no += first_data_sector + (cluster_no - 2) * sectors_per_cluster;

  return ReadToTempBuffer(sector_no);
}

const char *ReadFromCluster2(unsigned int cluster_no, unsigned int sector_no)
{
  sector_no += first_data_sector + (cluster_no - 2) * sectors_per_cluster;

  return BlockDeviceRead(sector_no, temp_buffer2);
}

const char *FolderScanBack(void);

const char *ReadLongFileName(directory_entry *de)
{
  const char *error_message;
  long_file_name_entry *le = (long_file_name_entry *)de;
  int i;
  char *np = fe.FileName, c;

  for (;;)
  {
    for (i = 0; i < 5; i++)
    {
      c = le->characters[i] & 0xFF;
      *np++ = c;
      if (c == 0)
        return NULL;
    }
    for (i = 0; i < 6; i++)
    {
      c = le->characters2[i] & 0xFF;
      *np++ = c;
      if (c == 0)
        return NULL;
    }
    for (i = 0; i < 2; i++)
    {
      c = le->characters3[i] & 0xFF;
      *np++ = c;
      if (c == 0)
        return NULL;
    }

    if (le->sequence_number & 0x40)
      break;

    le--;
    if (le < (long_file_name_entry *)temp_buffer)
    {
      error_message = FolderScanBack();
      if (error_message)
        return error_message;
      le = (long_file_name_entry *)(&temp_buffer2[BLOCK_SIZE - sizeof(directory_entry)]);
    }
  }

  *np = 0;
  return NULL;
}

void ReadFileName(directory_entry *de)
{
  char *np = fe.FileName;
  int i;

  for (i = 0; i < 8; i++)
  {
    if (de->file_name[i] == ' ')
      break;
    *np++ = de->file_name[i];
  }

  *np++ = '.';

  for (i = 0; i < 3; i++)
  {
    if (de->file_ext[i] == ' ')
      break;
    *np++ = de->file_ext[i];
  }

  np--;
  if (*np != '.')
    np++;

  *np = 0;
}

const char *FileOperationsStart(void)
{
  const char *error_message;
  unsigned int i;
  partition_table *pt;
  boot_sector *bs;

  error_message = BlockDeviceStart();
  if (error_message)
    return error_message;

  error_message = BlockDeviceRead(0, temp_buffer);
  if (error_message)
    return error_message;

  pt = (partition_table*)temp_buffer;
  if (pt->signature != PARTITION_TABLE_SIGNATURE)
    return "Bad partition table signature";

  for (i = 0; i < 4; i++)
  {
    if (pt->partitions[i].filesystem_id == FAT16_FILESYSTEM_ID)
    {
      
      first_fat_sector = pt->partitions[i].first_sector_no;

      error_message = BlockDeviceRead(first_fat_sector, temp_buffer);
      if (error_message)
        return error_message;

      bs = (boot_sector*)temp_buffer;

      if (bs->bytes_per_sector != BLOCK_SIZE)
        return "Wrong bytes per sector value";

      if (strncmp("FAT16", bs->file_system_type, 5))
        return "Wrong file system type";

      first_fat_sector += bs->reserved_sector_count;
      root_directory_sector_no = first_fat_sector + bs->number_of_fats * bs->sectors_per_fat;
      root_directory_length    = bs->number_of_root_dir_entries * 32 / BLOCK_SIZE;
      first_data_sector   = root_directory_sector_no + root_directory_length;
      sectors_per_cluster = bs->sectors_per_cluster;

      temp_sector = 0xFFFFFFFF;

      return NULL;
    }
  }

  return "Could not find FAT16 filesystem on disk";
}

const char *StartFolderScan(unsigned int first_sector, unsigned int size)
{
  const char *error_message;

  if (size == 0) // first_sector contains cluster no
  {
    error_message = ReadFromCluster(first_sector, 0);
    if (error_message)
      return error_message;
    scan_cluster_no = first_sector;
    scan_sector_no = 1;
  }
  else
  {
    error_message = ReadToTempBuffer(first_sector);
    if (error_message)
      return error_message;
    scan_cluster_no = 0;
    scan_sector_no = first_sector + 1;
    scan_sectors_left = size - 1;
  }

  return NULL;
}

int FolderScan(const char **error_message)
{
  if (scan_cluster_no)
  {
    if (scan_sector_no == sectors_per_cluster)
    {
      scan_cluster_no = GetNextCluster(scan_cluster_no, error_message);
      if (*error_message)
        return 0;
      if (!scan_cluster_no)
      {
        *error_message = NULL;
        return 0;
      }
      scan_sector_no = 0;
    }

    *error_message = ReadFromCluster(scan_cluster_no, scan_sector_no);
    if (*error_message)
      return 0;
    scan_sector_no++;
  }
  else
  {
    if (!scan_sectors_left)
      return 0;

    *error_message = ReadToTempBuffer(scan_sector_no);
    if (*error_message)
      return 0;
    scan_sector_no++;
    scan_sectors_left--;
  }

  *error_message = NULL;

  return 1;
}

const char *FolderScanBack(void)
{
  const char *error_message;
  unsigned int sector_no, cluster_no;

  if (scan_cluster_no)
  {
    if (scan_sector_no >= 2)
    {
      sector_no = scan_sector_no - 2;
      cluster_no = scan_cluster_no;
    }
    else
    {
      sector_no = sectors_per_cluster - 1;
      cluster_no = prev_cluster;
    }
    error_message = ReadFromCluster2(cluster_no, sector_no);
  }
  else
    error_message = BlockDeviceRead(scan_sector_no - 2, temp_buffer2);

  return error_message;
}

const char *FileFindInternal(const char *file_name)
{
  const char *error_message;
  directory_entry *de, *de2, *de3;
  int size;

  for (;;)
  {
    de = de2 = (directory_entry*)temp_buffer;
    size = BLOCK_SIZE / sizeof(directory_entry);

    for (;;)
    {
      while (de2->file_name[0] && (de2->file_attributes == FILE_ATTRIBUTE_LONG_FILE_NAME ||
             de2->file_attributes & (FILE_ATTRIBUTE_VOLUME_LABEL | FILE_ATTRIBUTE_DEV | FILE_ATTRIBUTE_UNUSED)))
      {
        size--;
        de2++;
        if (!size)
          break;
      }

      if (!size || !de2->file_name[0])
        break;

      if (de2->file_name[0] == 0xE5)
      {
        size--;
        de2++;
        if (!size)
          break;
        continue;
      }

      if (de2 > de)
      {
        de3 = de2;
        de3--;
        if (de3->file_attributes == FILE_ATTRIBUTE_LONG_FILE_NAME)
        {
          error_message = ReadLongFileName(de3);
          if (error_message)
            return error_message;
        }
        else
          ReadFileName(de2);
      }
      else
        ReadFileName(de2);

      if (!_stricmp(fe.FileName, file_name))
      {
        fe.file_attributes = de2->file_attributes;
        fe.file_size = de2->file_size;
        fe.start_cluster = de2->start_cluster;
        return NULL;
      }

      de2++;
      size--;
      if (!size || !de2->file_name[0])
        break;
    }

    if (!size)
    {
      size = FolderScan(&error_message);
      if (error_message)
        return error_message;
      if (!size)
        return NULL;
    }
    else
      break;
  }

  fe.FileName[0] = 0;
  return NULL;
}

FileEntry *FileFind(const char *file_name, const char **error_message)
{
  char *fn2;
  const char *fn;

  *error_message = StartFolderScan(root_directory_sector_no, root_directory_length);
  if (*error_message)
    return NULL;

  fn  = file_name;
  fn2 = file_n;

  while (*fn)
  {
    if (*fn == '\\' || *fn == '/')
    {
      *fn2 = 0;
      fn2 = file_n;
      *error_message = FileFindInternal(file_n);
      if (*error_message)
        return NULL;
      if (!fe.FileName[0] || !(fe.file_attributes & FILE_ATTRIBUTE_SUBDIRECTORY))
      {
        *error_message = NULL;
        return NULL;
      }
      *error_message = StartFolderScan(fe.start_cluster, 0);
      if (*error_message)
        return NULL;
    }
    else
      *fn2++ = *fn;
    fn++;
  }

  *fn2 = 0;
  *error_message = FileFindInternal(file_n);
  if (*error_message)
    return NULL;

  if (!fe.FileName[0])
    return NULL;

  return &fe;
}

unsigned int FileReadFirstBlock(const char *file_name, void **buffer, const char **error_message)
{
  FileEntry *f;
  unsigned int size;

  f = FileFind(file_name, error_message);
  if (*error_message)
    return 0;
  if (!f || (fe.file_attributes & FILE_ATTRIBUTE_SUBDIRECTORY))
  {
    *error_message = EFILENOTF;
    return 0;
  }

  *error_message = ReadFromCluster(fe.start_cluster, 0);
  if (*error_message)
    return 0;
  file_cluster = fe.start_cluster;
  file_sector = 1;
  size = fe.file_size >= BLOCK_SIZE ? BLOCK_SIZE : fe.file_size;
  file_size_left = fe.file_size - size;

  *buffer = temp_buffer;

  return size;
}

unsigned int FileReadNextBlock(const char **error_message)
{
  int size;

  if (!file_size_left)
  {
    *error_message = NULL;
    return 0;
  }
  
  size = file_size_left >= BLOCK_SIZE ?  BLOCK_SIZE : file_size_left;
  *error_message = ReadFromCluster(file_cluster, file_sector++);
  if (*error_message)
    return 0;
  file_size_left -= size;
  if (file_size_left && file_sector == sectors_per_cluster)
  {
    file_cluster = GetNextCluster(file_cluster, error_message);
    if (*error_message)
      return 0;
    file_sector = 0;
  }

  return size;
}

unsigned int FileGetSize(const char *file_name, const char **error_message)
{
  FileEntry *f;

  f = FileFind(file_name, error_message);
  if (*error_message)
    return 0;
  if (!f || (fe.file_attributes & FILE_ATTRIBUTE_SUBDIRECTORY))
  {
    *error_message = EFILENOTF;
    return 0;
  }

  return fe.file_size;
}

void FileOperationsEnd(void)
{
  BlockDeviceStop();
}

