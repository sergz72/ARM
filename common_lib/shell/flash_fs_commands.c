#include <flash_fs_commands.h>
#include <shell.h>
#include <flash_fs.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include "board.h"

static int dir_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem dir_command_items[] = {
    {NULL, NULL, dir_handler}
};
static const ShellCommand dir_command = {
    dir_command_items,
    "dir",
    NULL
};

static int mount_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem mount_command_items[] = {
    {NULL, NULL, mount_handler}
};
static const ShellCommand mount_command = {
    mount_command_items,
    "mount",
    NULL
};

static int unmount_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem unmount_command_items[] = {
    {NULL, NULL, unmount_handler}
};
static const ShellCommand unmount_command = {
    unmount_command_items,
    "unmount",
    NULL
};

static int format_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem format_command_items[] = {
    {NULL, NULL, format_handler}
};
static const ShellCommand format_command = {
    format_command_items,
    "format",
    NULL
};

static int cp_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static int cp_switch_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item);
static void cp_flags_init(void);
static const ShellCommandItem cp_command_items[] = {
    {NULL, cp_switch_handler},
    {NULL, param_handler},
    {NULL, param_handler, NULL},
    {NULL, NULL, cp_handler}
};
static const ShellCommand cp_command = {
    cp_command_items,
    "cp",
    "cp [-f] source_file_name dest_file_name",
    cp_flags_init
};

static int create_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem create_command_items[] = {
    {NULL, param_handler},
    {NULL, param_handler, NULL},
    {NULL, NULL, create_handler}
};
static const ShellCommand create_command = {
    create_command_items,
    "create",
    "create file_name contents"
};

static int type_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem type_command_items[] = {
    {NULL, param_handler},
    {NULL, NULL, type_handler}
};
static const ShellCommand type_command = {
    type_command_items,
    "type",
    "type file_name"
};

static int delete_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem delete_command_items[] = {
    {NULL, param_handler},
    {NULL, NULL, delete_handler}
};
static const ShellCommand delete_command = {
    delete_command_items,
    "del",
    "del file_name"
};

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem write_command_items[] = {
    {NULL, param_handler},
    {NULL, param_handler, NULL},
    {NULL, NULL, write_handler}
};
static const ShellCommand write_command = {
    write_command_items,
    "write",
    "write file_name contents"
};

static int rename_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem rename_command_items[] = {
    {NULL, param_handler},
    {NULL, param_handler, NULL},
    {NULL, NULL, rename_handler}
};
static const ShellCommand rename_command = {
    rename_command_items,
    "ren",
    "ren file_name new_file_name"
};

static int cp_overwrite;

static void cp_flags_init(void)
{
  cp_overwrite = 0;
}

static int mount_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc = FlashFSMount();
  if (rc)
    pfunc("%s\n", strerror(rc));
  return rc;
}

static int format_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  char *s;
  pfunc("Are you sure(y/N)?");
  s = gfunc();
  if ((*s == 'y' || *s == 'Y') && s[1] == 0)
  {
    int rc = FlashFSFormat();
    if (rc)
      pfunc("%s\n", strerror(rc));
    return rc;
  }
  return EINTR;
}

static int unmount_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc = FlashFSUnmount();
  if (rc)
    pfunc("%s\n", strerror(rc));
  return rc;
}

static int dir_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  FlashFSFileFind ff;
  int rc;
  unsigned int free, files, blocks, bytes;

  rc = FlashFSFindFirst(&ff);
  if (rc && rc != ENOENT)
  {
    pfunc("%s\n", strerror(rc));
    return rc;
  }
  else
  {
    free = FlashFSGetFreeSize();
    files = blocks = bytes = 0;
    if (!rc)
    {
      do
      {
        pfunc("%-15s %d\n", ff.name, ff.size);
        files++;
        bytes += ff.size;
        blocks += ff.size / FLASH_FS_MINIMUM_ERASE_SIZE;
        if (ff.size % FLASH_FS_MINIMUM_ERASE_SIZE)
          blocks++;
      } while (!FlashFSFindNext(&ff));
    }
    pfunc("%d files %d blocks %d bytes %d bytes free\n", files, blocks, bytes, free);
  }
  return 0;
}

static int cp_switch_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item)
{
  const char *p;

  if ((*cp)->length)
  {
    p = command + (*cp)->offset;
    if (*p == '-')
    {
      p++;
      if ((*cp)->length != 2 || *p != 'f')
        return 1;
      cp_overwrite = 1;
      (*cp)++;
    }
  }
  (*item)++;
  return 0;
}

static void oom(printf_func pfunc)
{
  pfunc("Out of memory");
}

static int cp_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  FlashFSFileFind ff;
  char *contents;

  if (argc != 2)
  {
    pfunc("cp: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFindFile(argv[0], &ff);
  if (rc)
  {
    pfunc("%s\n", strerror(rc));
    return rc;
  }
  contents = malloc(ff.size + 1);
  if (!contents)
  {
    oom(pfunc);
    return ENOMEM;
  }
  rc = FlashFSFileRead(ff.id, contents);
  if (rc)
  {
    free(contents);
    pfunc("%s\n", strerror(rc));
    return rc;
  }
  rc = FlashFSFindFile(argv[1], &ff);
  if (rc)
  {
    rc = FlashFSFileCreate(argv[1], contents, ff.size);
    if (rc)
      pfunc("%s\n", strerror(rc));
    free(contents);
    FlashFSSync();
    return rc;
  }
  if (cp_overwrite)
  {
    rc = FlashFSFileWrite(argv[1], contents, ff.size);
    if (rc)
      pfunc("%s\n", strerror(rc));
    else
      FlashFSSync();
  }
  else
  {
    rc = EEXIST;
    pfunc("%s\n", strerror(rc));
  }
  free(contents);
  return rc;
}

static int create_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  if (argc != 2)
  {
    pfunc("create: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFileCreate(argv[0], argv[1], strlen(argv[1]));
  if (rc)
    pfunc("%s\n", strerror(rc));
  else
    FlashFSSync();
  return rc;
}

static int type_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  FlashFSFileFind ff;
  char *contents;

  if (argc != 1)
  {
    pfunc("type: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFindFile(argv[0], &ff);
  if (rc)
  {
    pfunc("%s\n", strerror(rc));
    return rc;
  }
  contents = malloc(ff.size + 1);
  if (!contents)
  {
    oom(pfunc);
    return ENOMEM;
  }
  rc = FlashFSFileRead(ff.id, contents);
  if (rc)
  {
    free(contents);
    pfunc("%s\n", strerror(rc));
    return rc;
  }
  contents[ff.size] = 0;
  pfunc("%s\n", contents);
  free(contents);
  return 0;
}

static int delete_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  if (argc != 1)
  {
    pfunc("delete: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFileDelete(argv[0]);
  if (rc)
    pfunc("%s\n", strerror(rc));
  else
    FlashFSSync();
  return rc;
}

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  if (argc != 2)
  {
    pfunc("write: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFileWrite(argv[0], argv[1], strlen(argv[1]));
  if (rc)
    pfunc("%s\n", strerror(rc));
  else
    FlashFSSync();
  return rc;
}

static int rename_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int rc;
  if (argc != 2)
  {
    pfunc("rename: wrong number of arguments");
    return 1;
  }
  rc = FlashFSFileRename(argv[0], argv[1]);
  if (rc)
    pfunc("%s\n", strerror(rc));
  else
    FlashFSSync();
  return 0;
}

void register_flash_fs_commands(void)
{
  shell_register_command(&dir_command);
  shell_register_command(&cp_command);
  shell_register_command(&create_command);
  shell_register_command(&type_command);
  shell_register_command(&write_command);
  shell_register_command(&rename_command);
  shell_register_command(&delete_command);
  shell_register_command(&mount_command);
  shell_register_command(&format_command);
  shell_register_command(&unmount_command);
}
