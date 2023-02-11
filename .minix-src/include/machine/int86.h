#ifndef INT86_H
#define INT86_H

#if defined(__i386__)

#  include <sys/types.h>

/* registers used in an PC real mode call for BIOS services */
typedef struct reg86 {
  union {
    /* 32-bit (double word [dword] or long word) access */
    struct {
      u32_t ef;   /* 32-bit flags (output only) */
      u32_t vec;  /* Driver vector (input only) */
      u32_t : 32; /* unused */
      u32_t eax;  /* 32-bit general register */
      u32_t ebx;  /* 32-bit general register */
      u32_t ecx;  /* 32-bit general register */
      u32_t edx;  /* 32-bit general register */
      u32_t esi;  /* 32-bit index register */
      u32_t edi;  /* 32-bit index register */
      u32_t ebp;  /* 32-bit pointer register */
    };

    /* 16-bit (word) access */
    struct {
      u16_t f;        /* 16-bit flags (output only) */
      u16_t : 16;     /* unused */
      u16_t off, seg; /* Driver vector (input only) */
      u16_t ds, es;   /* DS and ES real mode segment registers */
      u16_t ax;       /* 16-bit general register */
      u16_t : 16;     /* unused */
      u16_t bx;       /* 16-bit general register */
      u16_t : 16;     /* unused */
      u16_t cx;       /* 16-bit general register */
      u16_t : 16;     /* unused */
      u16_t dx;       /* 16-bit general register */
      u16_t : 16;     /*unused */
      u16_t si;       /* 16-bit index register */
      u16_t : 16;     /*unused */
      u16_t di;       /* 16-bit index register */
      u16_t : 16;     /* unused */
      u16_t bp;       /* 16-bit pointer register */
      u16_t : 16;     /*unused */
    };

    /* 8-bit (byte) access */
    struct {
      u8_t intno;  /* Interrupt number (input only) */
      u8_t : 8;    /* unused */
      u16_t : 16;  /* unused */
      u32_t : 32;  /* unused */
      u32_t : 32;  /* unused */
      u8_t al, ah; /* 8-bit general registers */
      u16_t : 16;  /* unused */
      u8_t bl, bh; /* 8-bit general registers */
      u16_t : 16;  /* unused */
      u8_t cl, ch; /* 8-bit general registers */
      u16_t : 16;  /* unused */
      u8_t dl, dh; /* 8-bit general registers */
      u16_t : 16;  /* unused */
      u32_t : 32;  /* unused */
      u32_t : 32;  /* unused */
      u32_t : 32;  /* unused */
    };
  };
} reg86_t;

#endif

#endif /* _INT86_H */
