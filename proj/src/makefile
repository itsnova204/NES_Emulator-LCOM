# name of the program (Minix service)
PROG=proj 

# source code files to be compiled
.PATH: ${.CURDIR}/nes_emu

SRCS = ./main.c ./mapper.c ./cartridge.c ./bus.c ./p6502.c ./controler.c ./ppu.c


# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_

# list of library dependencies
DPADD += ${LIBLCF}
LDADD += -llcf -lxpm -llm

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>