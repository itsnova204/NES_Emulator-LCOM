#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define KBC_IRQ   1

#define MULTIPLE_BYTES 0xE0
#define MAKE_CODE BIT(7)
#define BREAK_ESC 0x81


#define KBC_OUT_BUF 0x60
#define KBC_STATUS_REG 0x64

#define KBC_CMD_OUT_REG 0x60
#define KBC_CMD_IN_REG 0x64
#define KBC_READ_CMD 0x20 
#define KBC_SET_CMD 0x60

#define KBC_SEND_NEW_CMD 0x60

#define KBC_MULTICODE 0xE0
#define MAKE_CODE       BIT(7)

//keyboard status codes
#define KBC_ERR_PARITY BIT(7)
#define KBC_ERR_TIMEOUT BIT(6)
#define KBC_AUX BIT(5)

#define KBC_IBF BIT(1)
#define KBC_OUT_BUF_FULL BIT(0)


#define PARITY_ERR    1
#define TIMEOUT_ERR   2
#define IN_BUF_FULL   3
#define OUT_BUF_NOT_FULL  4
#define OUT_BUF_FULL 0

#endif

