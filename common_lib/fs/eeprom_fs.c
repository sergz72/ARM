#include <board.h>
#include <eeprom_fs.h>

unsigned int EEPROM_get_psize(unsigned int size)
{
  size += sizeof(eeprom_header);

  if (size % EEPROM_PAGE_SIZE)
    return size / EEPROM_PAGE_SIZE + 1;

  return size / EEPROM_PAGE_SIZE;
}

const char *EEPROM_CreateSection(const char *section_name, int section_size, unsigned int *ppage_no)
{
  unsigned int page_no, free_space_max, section_psize;
  eeprom_header hdr, hdr2;

  if (section_size <= 0)
    return "Wrong section size";

  if (!*section_name)
    return "Empty section name";

  // checking if section already exist
  page_no = free_space_max = 0;
  section_psize = EEPROM_get_psize(section_size);

  while (page_no < EEPROM_PAGE_NUM)
  {
    EEPROM_ReadPage(page_no, &hdr, sizeof(eeprom_header) / 4, READ32);
    hdr.file_size = EEPROM_get_psize(hdr.file_size);
    if (!hdr.file_name[0]) // free space
    {
      if (hdr.file_size > free_space_max)
        free_space_max = hdr.file_size;
    }
    else if (!strcmp(hdr.file_name, section_name))
      return "Section already exist";
    page_no += hdr.file_size;
  }

  if (section_psize > free_space_max)
    return "No enough free space in the eeprom";

  // creating section
  page_no = 0;

  while (page_no < EEPROM_PAGE_NUM)
  {
    EEPROM_ReadPage(page_no, &hdr, sizeof(eeprom_header) / 4, READ32);
    hdr.file_size = EEPROM_get_psize(hdr.file_size);
    if (!hdr.file_name[0] && hdr.file_size >= section_psize)
    {
      strcpy_s(hdr2.file_name, sizeof(hdr2.file_name), section_name);
      hdr2.file_size = section_size;
      EEPROM_WritePage(page_no, &hdr2, strlen(section_name) + 5, WRITE8);
      if (hdr.file_size > section_psize)
      {
        hdr.file_size -= section_psize;
        hdr.file_size *= EEPROM_PAGE_SIZE;
        hdr.file_size -= sizeof(eeprom_header);
        EEPROM_WritePage(page_no + section_psize, &hdr, 5, WRITE8);
      }
      break;
    }
    page_no += hdr.file_size;
  }

  *ppage_no = page_no;

  return NULL;
}

int EEPROM_FindSection(const char *name, eeprom_section *sect)
{
  unsigned int page_no;
  eeprom_header hdr;

  if (!*name)
    return 0;

  page_no = 0;

  while (page_no < EEPROM_PAGE_NUM)
  {
    EEPROM_ReadPage(page_no, &hdr, sizeof(eeprom_header) / 4, READ32);
    if (!strcmp(hdr.file_name, name))
    {
      sect->page_no = page_no;
      sect->size = hdr.file_size;
      return 1;
    }
    page_no += EEPROM_get_psize(hdr.file_size);
  }

  return 0;
}

int EEPROM_DeleteSection(const char *name)
{
  int page_no, prev_page;
  eeprom_header hdr, hdr2;

  if (!*name)
    return 0;

  prev_page = -1;
  page_no = 0;

  while (page_no < EEPROM_PAGE_NUM)
  {
    EEPROM_ReadPage(page_no, &hdr, sizeof(eeprom_header) / 4, READ32);
    if (!strcmp(hdr.file_name, name))
    {
      if (prev_page >= 0)
      {
        EEPROM_ReadPage(prev_page, &hdr2, 5, READ8);
        if (!hdr2.file_name[0])
        {
          hdr.file_size += hdr2.file_size + sizeof(eeprom_header);
          page_no = prev_page;
        }
      }
      prev_page = page_no + EEPROM_get_psize(hdr.file_size);
      if (prev_page < EEPROM_PAGE_NUM)
      {
        EEPROM_ReadPage(prev_page, &hdr2, 5, READ8);
        if (!hdr2.file_name[0])
          hdr.file_size += hdr2.file_size + sizeof(eeprom_header);
      }
      hdr.file_name[0] = 0;
      hdr.file_size = EEPROM_get_psize(hdr.file_size) * EEPROM_PAGE_SIZE - sizeof(eeprom_header);
      EEPROM_WritePage(page_no, &hdr, 5, WRITE8);
      return 1;
    }
    prev_page = page_no;
    page_no += EEPROM_get_psize(hdr.file_size);
  }

  return 0;
}

void EEPROM_Format(void)
{
  eeprom_header hdr;

	EEPROM_Erase();

  hdr.file_size = EEPROM_SIZE - sizeof(eeprom_header);
  hdr.file_name[0] = 0;
  EEPROM_WritePage(0, &hdr, 5, WRITE8);
}
