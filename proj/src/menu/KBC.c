#include "KBC.h"

static uint8_t scan_code_array[2];
static bool flag_two_byte = false;
static int index_ = 0;

int(kbc_read_output)(uint8_t port, uint8_t *output, uint8_t mouse)
{
  uint8_t status;
  uint8_t i = 0;

   while(i < MAX_ATTEMPS) {

    if (util_sys_inb(KBD_STAT_REG, &status) != 0) {
      printf("Error: Could not read status!\n");
      return 1;
    }

    if ((status & KBD_FULL_OUT_BUF) != 0) { // BIT(0)

      if (status & (KBD_PARITY_ERR | KBD_TIMEOUT_ERR)) { // BIT(7) | BIT(6)
        printf("Error: Parity or Timeout error!\n");
        return 1;
      }

      if (util_sys_inb(port, output) != 0) {
        printf("Error: Could not read output!\n");
        return 1;
      }

      if (mouse == 0) { // teclado = 0; rato = 1
        if ((status & BIT(5)) != 0) { // teclado
          printf("Error: Not a keyboard scancode!\n");
          return 1;
        }
      } else {
        if ((status & BIT(5)) == 0) { // rato
          printf("Error: Not a mouse scancode!\n");
          return 1;
        }
      }

      return 0;
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

  while (i < MAX_ATTEMPS)
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

int(getScanCodeSize)(uint8_t scan_code)
{
  if (scan_code == KBD_TWO_BYTE)
    return 2;
  return 1;
}

bool is_two_byte_scan_code(uint8_t scan_code)
{
  return (getScanCodeSize(scan_code) == 2);
}

int print_scancode(uint8_t scan_code)
{
  if (is_two_byte_scan_code(scan_code) && !flag_two_byte)
  {
    scan_code_array[index_++] = scan_code;
    flag_two_byte = true;
    return 0;
  }

  if (flag_two_byte)
  {
    flag_two_byte = false;
    scan_code_array[index_] = scan_code;
    index_ = 0;

    if (kbd_print_scancode(!(KBD_MAKE_CODE & scan_code_array[1]), 2, scan_code_array))
      return 1;

    memset(scan_code_array, 0, 2);
    
    return 0;
  }

  if (kbd_print_scancode(!(KBD_MAKE_CODE & scan_code), 1, &scan_code))
    return 1;

  return 0;
}
