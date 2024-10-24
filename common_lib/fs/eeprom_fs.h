#ifndef _EEPROM_FS_H
#define _EEPROM_FS_H

#define READ8     0
#define READ16    1
#define READ32    2
#define WRITE8    3
#define WRITE16   4
#define WRITE32   5 

void EEPROM_WritePage(unsigned int pageWrite, void* pBuf, unsigned int pSize, unsigned int Command);
void EEPROM_ReadPage(unsigned int pageRead, void* pBuf, unsigned int pSize, unsigned int Command);
void EEPROM_Erase(void);

typedef struct {
  unsigned int file_size;
  char file_name[28];
} eeprom_header;

typedef struct {
  eeprom_header hdr;
  char file_data[EEPROM_PAGE_SIZE - 32];
} eeprom_header_page;

typedef struct {
  unsigned int page_no, size;
} eeprom_section;

unsigned int EEPROM_get_psize(unsigned int size);
const char *EEPROM_CreateSection(const char *section_name, int section_size, unsigned int *ppage_no);
int EEPROM_FindSection(const char *name, eeprom_section *sect);
int EEPROM_DeleteSection(const char *name);
void EEPROM_Format(void);

#endif
