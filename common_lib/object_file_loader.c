#include "object_file_loader.h"
#include <stdint.h>
#include <string.h>

typedef struct
{
  unsigned char magic;
  char elf[3];
  unsigned char bits;
  unsigned char endianess;
  unsigned char header_version;
  unsigned char os_abi;
  unsigned char padding[8];
  unsigned short type;
  unsigned short instruction_set;
  uint32_t version;
#ifdef _M_X64
  uint64_t entry_offset;
  uint64_t program_header_table_offset;
  uint64_t section_header_table_offset;
#else
  uint32_t entry_offset;
  uint32_t program_header_table_offset;
  uint32_t section_header_table_offset;
#endif
  uint32_t flags;
  unsigned short header_size;
  unsigned short program_header_table_entry_size;
  unsigned short program_header_table_entries_count;
  unsigned short section_header_table_entry_size;
  unsigned short section_header_table_entries_count;
  unsigned short section_header_string_table_section;
} elf_header;

typedef struct
{
  uint32_t name_offset;
  uint32_t type;
#ifdef _M_X64
  uint64_t flags;
  uint64_t address;
  uint64_t offset;
  uint64_t size;
#else
  uint32_t flags;
  uint32_t address;
  uint32_t offset;
  uint32_t size;
#endif
  uint32_t link;
  uint32_t info;
} elf_section_header;

typedef struct
{
#ifdef _M_X64
  uint32_t name_offset;
  unsigned char info;
  unsigned char other;
  unsigned short section_index;
  uint64_t value;
  uint64_t size;
#else
  uint32_t name_offset;
  uint32_t value;
  uint32_t size;
  unsigned char info;
  unsigned char other;
  unsigned short section_index;
#endif
} symbol_table_entry;

typedef struct
{
#ifdef _M_X64
  uint64_t offset;
  uint64_t info;
  int64_t addend;
#else
  uint32_t offset;
  uint32_t info;
  int32_t addend;
#endif
} relocation_table_entry;

typedef struct
{
#ifdef _M_X64
  uint64_t offset;
  uint64_t info;
#else
  uint32_t offset;
  uint32_t info;
#endif
} relocation_table_entry_wo_addens;

#ifdef _M_X64
#define R_SYM(i)    ((i)>>32)
#define R_TYPE(i)   ((i)&0xffffffffL)
#else
#define R_SYM(i)	((i)>>8)
#define R_TYPE(i)   ((unsigned char)(i))
#endif

static const elf_header *h;
static const char *symbol_strings;
static const symbol_table_entry *symbol_table;
static int symbol_table_size;
static const function_def *function_map;

static void *get_address(unsigned int section_index, unsigned int value)
{
  elf_section_header *section = (elf_section_header *) ((char *) h + h->section_header_table_offset +
                                                        h->section_header_table_entry_size * section_index);
  if (section->type == 8) // bss
    return (char*)section->address + value;
  return (char *) h + section->offset + value;
}

static void *get_symbol_address_by_name(const char *name)
{
  int size = symbol_table_size;
  const symbol_table_entry *p = symbol_table;
  while (size--)
  {
    unsigned char bind = p->info >> 4;
    //        unsigned char type = p->info & 0xF;
    const char *symbol_name = symbol_strings + p->name_offset;
    if (bind == 1 && !strcmp(name, symbol_name))
      return get_address(p->section_index, p->value);
    p++;
  }

  return NULL;
}

static void *get_symbol_address_by_id(unsigned int id)
{
  const symbol_table_entry *p = symbol_table + id;
  if (p->section_index == 0)
  {
    const char *name = symbol_strings + p->name_offset;
    const function_def *fp = function_map;
    while (fp->name != NULL && strcmp(fp->name, name))
      fp++;
    return fp->pointer;
  }
  return get_address(p->section_index, p->value);
}

static int apply_x86_relocation_table_item(int type, void *dest, void *symbol_address, int addend)
{
  int32_t ivalue;
  switch (type)
  {
    case 1: // absolute
      *(void **) dest = (char *) symbol_address + addend;
      break;
    case 2: // 32 bit relative
      ivalue = (int32_t) symbol_address - (int32_t) dest + addend;
      *((int32_t *) dest) += ivalue;
      break;
    default:
      return 11;
  }
  return 0;
}

static int apply_arm_relocation_table_item(int type, void *dest, void *symbol_address)
{
  switch (type)
  {
    case 2: // absolute
      *(unsigned int *) dest += (unsigned int)symbol_address;
     break;
    default:
      return 11;
  }
  return 0;
}

static int apply_relocation_table(relocation_table_entry *relocation_table, int relocation_table_size,
                                  uint32_t section_id)
{
  void *section_address = get_address(section_id, 0);
  while (relocation_table_size--)
  {
    int symbol_id = R_SYM(relocation_table->info);
    int type = R_TYPE(relocation_table->info);
    void *symbol_address = get_symbol_address_by_id(symbol_id);
    if (symbol_address == NULL)
      return 10;
    void *dest = (char *) section_address + relocation_table->offset;
    int rc;
    switch (h->instruction_set)
    {
      case 0x3E: // x86-64
        rc = apply_x86_relocation_table_item(type, dest, symbol_address, relocation_table->addend);
        if (rc)
          return rc;
        break;
      default:
        return 12;
    }
    relocation_table++;
  }
  return 0;
}

static int apply_relocation_table_wo_addens(relocation_table_entry_wo_addens *relocation_table,
                                            int relocation_table_size, uint32_t section_id)
{
  void *section_address = get_address(section_id, 0);
  while (relocation_table_size--)
  {
    int symbol_id = R_SYM(relocation_table->info);
    int type = R_TYPE(relocation_table->info);
    void *symbol_address = get_symbol_address_by_id(symbol_id);
    if (symbol_address == NULL)
      return 10;
    void *dest = (char *) section_address + relocation_table->offset;
    int rc;
    switch (h->instruction_set)
    {
      case 0x28: // ARM
        rc = apply_arm_relocation_table_item(type, dest, symbol_address);
        if (rc)
          return rc;
      break;
      default:
        return 12;
    }
    relocation_table++;
  }
  return 0;
}

int object_file_load(void *data, const function_def *_function_map, void *bss, unsigned int bss_size)
{
  h = data;
  function_map = _function_map;

  if (h->magic != 0x7F || h->elf[0] != 'E' || h->elf[1] != 'L' || h->elf[2] != 'F')
    return 1;
  if (sizeof(void *) == 4 && h->bits != 1)
    return 2;
  if (sizeof(void *) == 8 && h->bits != 2)
    return 2;
  if (h->endianess != 1)
    return 3;
  if (h->type != 1)
    return 4;

  void *section_header_table = (char *) data + h->section_header_table_offset;
  elf_section_header *strings_section = (elf_section_header *) ((char *) data + h->section_header_table_offset +
                                                                h->section_header_table_entry_size * h->
                                                                section_header_string_table_section);
  char *strings = (char *) data + strings_section->offset;

  symbol_strings = NULL;
  symbol_table = NULL;

  for (int i = 0; i < h->section_header_table_entries_count; i++)
  {
    elf_section_header *sh = section_header_table;
    const char *name = strings + sh->name_offset;
    if (sh->type == 3 && !strcmp(name, ".strtab"))
      symbol_strings = (char *) data + sh->offset;

    if (sh->type == 2) // symbol table
    {
      symbol_table = (symbol_table_entry *) ((char *) data + sh->offset);
      symbol_table_size = sh->size / sizeof(symbol_table_entry);
    }

    if (sh->type == 8) // bss
    {
      if (sh->size > bss_size)
        return 7;
#ifdef _M_X64
      sh->address = (uint64_t)bss;
#else
      sh->address = (uint32_t)bss;
#endif
      sh->size = (sh->size / sizeof(void*) + 1) * sizeof(void*);
      bss = (char*)bss + sh->size;
      bss_size -= sh->size;
    }

    section_header_table = (char *) section_header_table + h->section_header_table_entry_size;
  }

  if (symbol_strings == NULL)
    return 5;

  if (symbol_table == NULL)
    return 6;

  section_header_table = (char *) data + h->section_header_table_offset;

  for (int i = 0; i < h->section_header_table_entries_count; i++)
  {
    elf_section_header *sh = section_header_table;

    if (sh->type == 4) // Relocation entries with addends
    {
      relocation_table_entry *relocation_table = (relocation_table_entry *) ((char *) data + sh->offset);
      int relocation_table_size = sh->size / sizeof(relocation_table_entry);
      int rc = apply_relocation_table(relocation_table, relocation_table_size, sh->info);
      if (rc != 0)
        return rc;
    }

    if (sh->type == 9) // Relocation entries, no addends
    {
      relocation_table_entry_wo_addens *relocation_table = (relocation_table_entry_wo_addens *) ((char *) data + sh->offset);
      int relocation_table_size = sh->size / sizeof(relocation_table_entry_wo_addens);
      int rc = apply_relocation_table_wo_addens(relocation_table, relocation_table_size, sh->info);
      if (rc != 0)
        return rc;
    }

    section_header_table = (char *) section_header_table + h->section_header_table_entry_size;
  }

  return 0;
}

int object_file_call(const char *function)
{
  int (*entry_point)(void) = get_symbol_address_by_name(function);
  if (entry_point == NULL)
    return -1;

  return entry_point();
}
