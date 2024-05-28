#include "mouse.h"

int hook_id_mouse = 3; // entre 0 e 7 (IRQs)
uint8_t current_byte;
struct packet pp;
uint8_t i = 0; // byte index
uint8_t mouse_bytes[3];

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
    if (read_kbc_output(KBC_WRITE_CMD, &current_byte, 1))
    {
        printf("Error: read_kbc_output failed!\n");
    }
}

void(mouse_sync)()
{ // sincroniza o mouse
    if (i == 0 && (current_byte & MOUSE_FIRST) != 0)
    { // se o bit 3 do byte atual estiver ativado (igual a 1)
        mouse_bytes[i] = current_byte;
    }
    else if (i > 0)
    {
        mouse_bytes[i] = current_byte;
    }
    i++;
}

void(mouse_parse_packet)()
{ // lê os 3 bytes do pacote do mouse
    pp.bytes[0] = mouse_bytes[0];
    pp.bytes[1] = mouse_bytes[1];
    pp.bytes[2] = mouse_bytes[2];
}

int(mouse_bytes_to_packet)()
{ // converte os 3 bytes do pacote do mouse

    mouse_parse_packet();

    pp.lb = pp.bytes[0] & BIT(0);   // left
    pp.rb = pp.bytes[0] & BIT(1);   // right
    pp.mb = pp.bytes[0] & BIT(2);   // middle
    pp.x_ov = pp.bytes[0] & BIT(6); // x overflow
    pp.y_ov = pp.bytes[0] & BIT(7); // y overflow

    if ((pp.bytes[0] & BIT(4)) != 0)
    {                                   // mouse x sign
        pp.delta_x = pp.bytes[1] - 256; // complemento para 2
    }
    else
    {
        pp.delta_x = pp.bytes[1]; // valor normal
    }

    if ((pp.bytes[0] & BIT(5)) != 0)
    {                                   // mouse y sign
        pp.delta_y = pp.bytes[2] - 256; // complemento para 2
    }
    else
    {
        pp.delta_y = pp.bytes[2]; // valor normal
    }

    return 0;
}

int mouse_write(uint8_t cmd)
{
    uint8_t ack; // acknowledge (resposta) do rato
    uint8_t c = 0;

    while (c < MAX_ATTEMPS)
    {
        if ((write_kbc_command(KBD_IN_BUF, MOUSE_WRITE_BYTE) != 0))
        {
            printf("Error: write_kbc_command failed!\n");
            return 1;
        }

        if ((write_kbc_command(KBD_OUT_BUF, cmd) != 0))
        {
            printf("Error: write_kbc_command failed!\n");
            return 1;
        }

        tickdelay(micros_to_ticks(DELAY));

        if (util_sys_inb(KBD_OUT_BUF, &ack) != 0)
        {
            printf("Error: read_kbc_output failed!\n");
            return 1;
        }

        if (ack == MOUSE_ACK)
        { // se o rato respondeu
            return 0;
        }

        c++;
    }

    return 1;
}
