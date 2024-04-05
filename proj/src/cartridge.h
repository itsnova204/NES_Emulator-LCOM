#include "lcf_mock.h"

#pragma once

int cart_insert();
void cart_remove();
void header_parse(FILE *fp);
uint8_t ines_parse(FILE *fp);
void print_header();

uint8_t sys_readFromCard(uint16_t addr);
void sys_writeToCard(uint16_t addr, uint8_t data);
