#include "mouse.h"

int mouse_hook_id = 2;
uint8_t mouse_bytes[3];
uint8_t current_byte;
struct packet mouse_packet;

static bool valid = false;
static uint8_t byte_index = 0;

bool(is_valid)() {
  return valid;
}
uint8_t getByteIndex() {
  return byte_index;
}

void resetByteIndex() {
  byte_index = 0;
}

int (mouse_subscribe_int)(uint8_t *bit_no){
  if (bit_no == NULL) return 1;

  *bit_no = BIT(mouse_hook_id);
  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id)) return 1;

  return 0;
}

int (mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id)) return 1;
  return 0;
}

void (mouse_ih)(){
  if(kbc_read_output(KBC_WRITE_CMD, &current_byte, true, 1)) {
    valid = false;
    return;
  }
  valid = true;
}

void mouse_sync_bytes() {
  if (byte_index == 0 && (current_byte & MOUSE_FIRST_BYTE)) {
    mouse_bytes[byte_index]= current_byte;
    byte_index++;
    return;
  }
  if (byte_index > 0) {
    mouse_bytes[byte_index] = current_byte;
    byte_index++;
    return;
  }
}

void (mouse_bytes_to_packet)(){

  for (int i = 0 ; i < 3 ; i++) {
    mouse_packet.bytes[i] = mouse_bytes[i];
  }

  mouse_packet.lb = mouse_bytes[0] & MOUSE_LB;
  mouse_packet.mb = mouse_bytes[0] & MOUSE_MB;
  mouse_packet.rb = mouse_bytes[0] & MOUSE_RB;
  mouse_packet.x_ov = mouse_bytes[0] & MOUSE_X_OVERFLOW;
  mouse_packet.y_ov = mouse_bytes[0] & MOUSE_Y_OVERFLOW;
  mouse_packet.delta_x = (mouse_bytes[0] & MOUSE_X_SIGNAL) ? (0xFF00 | mouse_bytes[1]) : mouse_bytes[1];
  mouse_packet.delta_y = (mouse_bytes[0] & MOUSE_Y_SIGNAL) ? (0xFF00 | mouse_bytes[2]) : mouse_bytes[2];
}

int (mouse_write)(uint8_t command) {

  uint8_t attemps = MAX_ATTEMPS;
  uint8_t mouse_response;

  do {
    attemps--;
    if (kbc_write_command(KBC_IN_CMD, MOUSE_WRITE_BYTE)) return 1;
    if (kbc_write_command(KBC_OUT_CMD, command)) return 1;
    tickdelay(micros_to_ticks(DELAY));
    if (util_sys_inb(KBC_OUT_CMD, &mouse_response)) return 1;
    if (mouse_response == ACK) return 0;
  } while (mouse_response != ACK && attemps);

  return 1;
}

