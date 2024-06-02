#include "KBC.h"

static uint8_t scancode_array[2];
static bool flag_two_byte = false;
static int index_ = 0;

int(kbc_read_output)(uint8_t port, uint8_t *output, bool mouse)
{
  uint8_t status;
  uint8_t i = 0;

   while(i < ATTEMPS) {

    if (util_sys_inb(KBD_STAT_REG, &status) != 0) {
      printf("Error: Could not read status!\n");
      return 1;
    }

    if ((status & KBD_FULL_OUT_BUF) != 0) { 

      if (status & (KBD_PARITY_ERR | KBD_TIMEOUT_ERR)) { 
        printf("Error: Parity or Timeout error!\n");
        return 1;
      }

      if (util_sys_inb(port, output) != 0) {
        printf("Error: Could not read output!\n");
        return 1;
      }

      if (!mouse) { 
        if ((status & BIT(5)) != 0) {
          printf("Error: Not a keyboard scancode!\n");
          return 1;
        }

        return 0;

      } else {
        if ((status & BIT(5)) == 0) { 
          printf("Error: Not a mouse scancode!\n");
          return 1;
        }

        return 0;
        
      }
    }
    
    tickdelay(micros_to_ticks(DELAY));
    i++;
  }

  return 1;
}

int(kbc_write_command)(uint8_t port, uint8_t command)
{
  uint8_t status;
  uint8_t i = 0;

  while (i < ATTEMPS)
  {
    if (util_sys_inb(KBD_STAT_REG, &status) != 0)
    {
      printf("Error: Could not read status!\n");
      return 1;
    }

    if (!(status & KBD_FULL_IN_BUF))
    {
      if (sys_outb(port, command))
      {
        printf("Error: Could not write command!\n");
        return 1;
      }
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY));
    i++;
  }

  return 1;
}

int(getScanCodeSize)(uint8_t scancode)
{
  if (scancode == KBD_TWO_BYTE)
    return 2;
  return 1;
}

bool is_two_byte_scancode(uint8_t scancode)
{
  return (getScanCodeSize(scancode) == 2);
}

int print_scancode(uint8_t scancode)
{
  if (is_two_byte_scancode(scancode) && !flag_two_byte)
  {
    scancode_array[index_++] = scancode;
    flag_two_byte = true;
    return 0;
  }

  if (flag_two_byte)
  {
    flag_two_byte = false;
    scancode_array[index_] = scancode;
    index_ = 0;

    if (kbd_print_scancode(!(KBD_MAKE_CODE & scancode_array[1]), 2, scancode_array))
      return 1;

    memset(scancode_array, 0, 2);
    
    return 0;
  }

  if (kbd_print_scancode(!(KBD_MAKE_CODE & scancode), 1, &scancode))
    return 1;

  return 0;
}
