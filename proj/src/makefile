# name of the program (Minix service)
PROG=proj 

# source code files to be compiled
.PATH: ${.CURDIR}/nes_emu
.PATH: ${.CURDIR}/drivers


SRCS = ./main.c byteQueue.c uart.c graphics.c KBC.c keyboard.c mouse.c timer.c rtc.c sprite.c utils.c xpm_image.c
SRCS += bus.c p6502.c ppu.c cartridge.c controller.c mapper.c


# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf -lxpm -llm -O3 -Wall -Wextra -pedantic

LLDADD += -Warray-bounds -Wformat-overflow -Wstringop-overflow -Wstringop-overread

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>