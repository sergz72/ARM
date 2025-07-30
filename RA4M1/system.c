#include "board.h"
#include <system.h>
#include <string.h>
#define BSP_LINKER_C
#include <bsp_linker_info.h>

void SystemRuntimeInit (const unsigned int external)
{
  /* Initialize C runtime environment. */
  for (unsigned int i = 0; i < g_init_info.zero_count; i++)
  {
    if (external == g_init_info.p_zero_list[i].type.external)
    {
      memset(g_init_info.p_zero_list[i].p_base, 0U,
             ((unsigned int) g_init_info.p_zero_list[i].p_limit - (unsigned int) g_init_info.p_zero_list[i].p_base));
    }
  }

  for (unsigned int i = 0; i < g_init_info.copy_count; i++)
  {
    if (external == g_init_info.p_copy_list[i].type.external)
    {
      memcpy(g_init_info.p_copy_list[i].p_base, g_init_info.p_copy_list[i].p_load,
             ((unsigned int) g_init_info.p_copy_list[i].p_limit - (unsigned int) g_init_info.p_copy_list[i].p_base));
    }
  }
}
