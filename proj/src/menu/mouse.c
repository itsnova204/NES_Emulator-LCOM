#include "mouse.h"

int hook_id_mouse = 3;
uint8_t currByte;
struct packet pp;
uint8_t i = 0;
uint8_t mouse_bytes[3];

static bool valid = false;

int mouse_subscribe_int(uint8_t *bit_no)
{
    if (bit_no == NULL)
        return 1;
    *bit_no = BIT(hook_id_mouse);

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != OK)
    {
        printf("Error: sys_irqsetpolicy failed!\n");
        return 1;
    }
    return 0;
}

int mouse_unsubscribe_int()
{
    if (sys_irqrmpolicy(&hook_id_mouse) != OK)
    {
        printf("Error: sys_irqrmpolicy failed!\n");
        return 1;
    }
    return 0;
}

void(mouse_int_handler)()
{
    if (kbc_read_output(KBC_WRITE_CMD, &currByte, true))
    {
        printf("Error: kbc_read_output failed!\n");
        valid = false;
        return;
    }
    valid = true;
}

bool(mouse_sync)()
{ 
    if (i == 0 && (currByte & MOUSE_FIRST) != 0)
    { 
        mouse_bytes[i] = currByte;
    }
    else if (i > 0)
    {
        mouse_bytes[i] = currByte;
    }
    i++;

    if (i == 3) {
        i = 0;
        mouse_parse_struct();
        return true;
    }
    return false;
}

void(mouse_parse_packet)()
{
    pp.bytes[0] = mouse_bytes[0];
    pp.bytes[1] = mouse_bytes[1];
    pp.bytes[2] = mouse_bytes[2];
}

int(mouse_parse_struct)()
{
    mouse_parse_packet();

    pp.lb = pp.bytes[0] & BIT(0); 
    pp.rb = pp.bytes[0] & BIT(1);  
    pp.mb = pp.bytes[0] & BIT(2); 
    pp.x_ov = pp.bytes[0] & BIT(6);
    pp.y_ov = pp.bytes[0] & BIT(7); 

    if ((pp.bytes[0] & BIT(4)) != 0)
    {                                  
        pp.delta_x = pp.bytes[1] | 0xFF00;
    }
    else
    {
        pp.delta_x = pp.bytes[1]; 
    }

    if ((pp.bytes[0] & BIT(5)) != 0)
    {                                  
        pp.delta_y = pp.bytes[2] | 0xFF00; 
    }
    else
    {
        pp.delta_y = pp.bytes[2]; 
    }

    return 0;
}

int mouse_write_command(uint8_t cmd)
{
    uint8_t ack;
    uint8_t c = 0;

    while (c < ATTEMPS)
    {
        if ((kbc_write_command(KBD_IN_BUF, MOUSE_WRITE_BYTE) != 0))
        {
            printf("Error: kbc_write_command failed!\n");
            return 1;
        }

        if ((kbc_write_command(KBD_OUT_BUF, cmd) != 0))
        {
            printf("Error: kbc_write_command failed!\n");
            return 1;
        }

        tickdelay(micros_to_ticks(DELAY));

        if (util_sys_inb(KBD_OUT_BUF, &ack) != 0)
        {
            printf("Error: kbc_read_output failed!\n");
            return 1;
        }

        if (ack == MOUSE_ACK)
        { 
            return 0;
        }

        c++;
    }

    return 1;
}

struct packet get_mouse_packet() {
    return pp;
}
