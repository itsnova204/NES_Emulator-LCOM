#ifndef _LCOM_VBE_H_
#define _LCOM_VBE_H_

#define VBE_FUNCTION_INVOKE             0x4F
#define VBE_FUNCTION_SUPPORTED          0x4F
#define VBE_FUNCTION_SUCCESSFULLY       0x00
#define VBE_FUNCTION_CALL_FAILED        0x01
#define VBE_FUNCTION_HW_NOT_SUPPORTED   0x02
#define VBE_FUNCTION_INVALID_IN_MODE    0x03

#define VBE_MODE_INDEXED                0x105 //1024x768
#define VBE_MODE_DC_15                  0x110 //640x480
#define VBE_MODE_DC_24                  0x115 //800x600
#define VBE_MODE_DC_16                  0x11A //1280x1024
#define VBE_MODE_DC_32                  0x14C //1152x864

#define VBE_SET_MODE                    0x4F02
#define VBE_GET_MODE_INFO               0x4F01
#define VBE_GET_CONTROLLER_INFO         0x4F00

#define VBE_CGA_MODE                    0x03

#define LINEAR_FRAME_BUF                BIT(14)

#endif
