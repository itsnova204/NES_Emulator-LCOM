#ifndef _LCOM_VBE_H_
#define _LCOM_VBE_H_

/**
 * @defgroup VBE VBE
 * @brief Constants for programming the VBE
 * 
*/

#define VBE_FUNCTION_INVOKE             0x4F
#define VBE_FUNCTION_SUPPORTED          0x4F
#define VBE_FUNCTION_SUCCESSFULLY       0x00
#define VBE_FUNCTION_CALL_FAILED        0x01
#define VBE_FUNCTION_HW_NOT_SUPPORTED   0x02
#define VBE_FUNCTION_INVALID_IN_MODE    0x03

#define VBE_MODE_INDEXED                0x105 //1024x768    8 per pixel
#define VBE_MODE_DC_15                  0x110 //640x480     15((1:)5:5:5)
#define VBE_MODE_DC_24                  0x115 //800x600     24(8:8:8)
#define VBE_MODE_DC_16                  0x11A //1280x1024   16(5:6:5)
#define VBE_MODE_DC_32                  0x14C //1152x864    32((8:)8:8:8)

#define VBE_SET_MODE                    0x4F02
#define VBE_GET_MODE_INFO               0x4F01
#define VBE_GET_CONTROLLER_INFO         0x4F00

#define VBE_CGA_MODE                    0x03

#define LINEAR_FRAME_BUF                BIT(14)

#define DIRECT_COLOR                    0x06
#define INDEXED_COLOR                   0x105

#endif
