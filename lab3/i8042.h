#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define KBD_IRQ 1         // interrupt request
#define KBD_STAT_REG 0x64
#define KBD_IN_BUF 0x60
#define KBD_OUT_BUF 0x60

#define KBD_PARITY_ERR BIT(7)
#define KBD_TIMEOUT_ERR BIT(6)
#define KBD_INHIBIT_FLAG BIT(4)
#define KBD_IN_BUF_FULL BIT(1)
#define KBD_OUT_BUF_FULL BIT(0)

#define KBD_ESC_BREAK_CODE 0x81
#define KBD_MAKE_CODE BIT(7)  
#define KBD_TWO_BYTE 0xE0

#endif