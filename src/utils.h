/*
 utils.h : utils header definitions.

 (c) 2007-2011-2013-2017 Fernando Iazeolla

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DW_UTILS_H
#define DW_UTILS_H

#include<stdint.h>
typedef enum {bitNULL,bit8,bit16,bit32,bit64,bit128} filebitclass;
#define DUMP_SIZE 128
#define DUMP_MAX_LINE 16
#define DUMP_LINES 8

void die(char *s);
void warn(char *s);
uint8_t peek8(off_t offset);
uint16_t peek16(off_t offset);
uint32_t peek32(off_t offset);
uint64_t peek64(off_t offset);
void poke8(uint8_t data,off_t offset);
void poke16(uint16_t data,off_t offset);
void poke32(uint32_t data,off_t offset);
void poke64(uint64_t data,off_t offset);
int probe_cpu_endian(void);
void endian_swap_16(uint16_t *x);
void endian_swap_32(uint32_t *x);
void endian_swap_64(uint64_t *x);
uint8_t get_data8(uint8_t data);
uint16_t get_data16(uint16_t data);
uint32_t get_data32(uint32_t data);
uint64_t get_data64(uint64_t data);
void get_color_str(char *s,uint8_t *c);
void extended_dump(off_t offset,off_t nbytes);
int decode_file_bit(int bits);
char* decode_file_endian(int endian);
char* decode_yes_no(int x);
char* decode_true_false(int x);
void reset_stdin(void);
char* decode_fd_type(int type);
void convert_string_hex(char *s);

#endif
