#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/**
 * @defgroup i8042 i8042
 * @brief Constants for programming the i8042 controller.
 *
*/

#define KBD_IRQ             1       // interrupt request

#define KBD_STAT_REG        0x64
#define KBD_IN_BUF          0x64
#define KBD_OUT_BUF         0x60

#define KBC_READ_CMD        0x20
#define KBC_WRITE_CMD       0x60

#define KBD_PARITY_ERR      BIT(7)
#define KBD_TIMEOUT_ERR     BIT(6)
#define KBD_INHIBIT_FLAG    BIT(4)
#define KBD_FULL_IN_BUF     BIT(1)
#define KBD_FULL_OUT_BUF    BIT(0)
#define ENABLE_INT          BIT(0)

#define KBD_ESC_BREAK_CODE  0x81
#define KBD_MAKE_CODE       BIT(7)  // 1 if make
#define KBD_TWO_BYTE        0xE0

#define ATTEMPS             10
#define DELAY               10000   // 10 ms

#define MOUSE_IRQ           12

#define MOUSE_WRITE_BYTE    0xD4
#define MOUSE_FIRST         BIT(3)

#define MOUSE_ACK           0xFA    // valid byte

#define ENABLE_DATA_REPORT  0xF4
#define DISABLE_DATA_REPORT 0xF5

#endif
