#include "keyboard.h"

int kbd_hook_id = 1;
static uint8_t scancode;
static bool valid = false;

uint8_t(get_scancode)()
{
  return scancode;
}

bool(is_valid)()
{
  return valid;
}

int(kbd_subscribe_int)(uint8_t *bit_no)
{
  if (bit_no == NULL)
    return 1;
  *bit_no = BIT(kbd_hook_id);

  return (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id));
}

int(kbd_unsubscribe_int)()
{
  return sys_irqrmpolicy(&kbd_hook_id);
}

void(kbc_ih)()
{ // handler do teclado (ler o output do kbc)
  if (kbc_read_output(KBD_OUT_BUF, &scancode, true) != 0)
  {
    valid = false;
    return;
  }

  valid = true;
}

int(kbd_restore)()
{ // restaurar o estado do teclado (ativar as interrupções)
  uint8_t cmd_byte;

  if (kbc_write_command(KBD_IN_BUF, KBC_READ_CMD) != 0)
    return 1;

  if (kbc_read_output(KBD_OUT_BUF, &cmd_byte, true) != 0)
    return 1;

  cmd_byte = cmd_byte | ENABLE_INT; // BIT(0); ativar as interrupções

  if (kbc_write_command(KBD_IN_BUF, KBC_WRITE_CMD) != 0)
    return 1;
  if (kbc_write_command(KBC_WRITE_CMD, cmd_byte) != 0)
    return 1;

  return 0;
}
