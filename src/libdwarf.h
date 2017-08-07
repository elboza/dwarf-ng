/*
libdwarf.h : the main dwarf header definitions.

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

#ifndef DW_LIBDWARF_H
#define DW_LIBDWARF_H

#include<sys/types.h>
//#include "../../config.h"
#include "config.h"
#include<inttypes.h>
#include"utils.h"
#include"vars.h"
#include"elf.h"
#include"macho.h"
#include"pe.h"
#include"dw_switchers.h"
#include "dw_elf_utils.h"
#include "dw_macho_utils.h"
#include "dw_pe_utils.h"


#define FILENAME_LEN 1024
typedef enum {FT_NULL,FT_MZEXE,FT_PE,FT_ELF,FT_MACHO,FT_FATMACHO,FT_FAT,FT_FAT16,FT_FAT32,FT_EXT,FT_MBR} filecodetype;
typedef enum {little_endian,big_endian} endian;
typedef enum {false,true} TF;
typedef enum {FD_STDIN,FD_REGULAR_FILE} fd_type;
typedef enum {DW_PATTERN_STRING,DW_PATTERN_HEX} dw_pattern_type;
typedef enum {SECT_NULL,SECT_ELF,SECT_PH,SECT_SH,SECT_LC,SECT_MAC,SECT_MZ,SECT_PE,SECT_PESECT,SECT_MACSECT,SECT_FATMACHO,SECT_FATARCH,SECT_PE_FILEHEADER,SECT_PE_OPTIONALHEADER} sectionheader;
typedef enum {E_WO_ADD,E_WO_AND,E_WO_SUB,E_WO_MUL,E_WO_RSHIFT,E_WO_LSHIFT,E_WO_XOR,E_WO_OR} e_wo_ops;
struct _cfg {
	int writable;
	int can_grow;
	char name[FILENAME_LEN];
	int work_on_copy;
	int changed_altered;
	int fd_type;
	int verbose;
	int cpu_endian;
	int file_endian;
	int file_type;
	int file_bit_class;
	int file_cpu_type;
	off_t filesize;
	char *faddr;
	char copydir[FILENAME_LEN];
	char copyname[FILENAME_LEN];
	int fd;
	off_t seek;
	off_t block;
	int colors;
	struct _cfg *prev,*next;
}cfg,*filecfg_first,*filecfg_last,*fc_ptr;

typedef enum{C_RESET,C_BYELLOW,C_BPURPLE,C_BBLUE,C_BLACK,C_BOLD,C_BRED,C_BGREEN,C_YELLOW,C_PURPLE,C_BLUE,C_RED,C_GREEN} color_scheme;
char **ptr_colors;

void dw_init(void);
void cfg_reset(void);
struct _cfg* newfilecfg(void);
int addfilecfg(struct _cfg*);
void rmfilecfg(struct _cfg*);
struct _cfg* getnewfilecfg(void);
void deleteallfilecfg(void);
void getcopyname(char *s);
void readcfg(char *s);
void file_open(char *s);
void open_stdin(void);
void close_stdin(void);
void file_close(void);
void file_close_all(void);
void dw_quit(void);
void file_save(char *s);
off_t filesize(int fd);
void prettybyte(char *s,off_t num);
void dottedbyte(char *s,off_t num);
int growth(off_t len);
int shrink(off_t len);
void extract(off_t from,off_t len,char *file);
void move(off_t from,off_t end,off_t to);
int mod_len(off_t len);
void move_r_pos(off_t from,off_t len,off_t to);
void inject_byte(int data,off_t from,off_t len,int shift);
void inject_file(char *file,off_t from,off_t len,int shift);
void set_colors(int b);
void block_func(int set,off_t offs);
void block_inc_func(off_t x);
void block_dec_func(off_t x);
void show_current_seek(void);
void set_current_seek(off_t x);
void seek_inc(off_t x);
void seek_dec(off_t x);
void seek_block_inc(void);
void seek_block_dec(void);
void seek_data(char *s);
void seek_hex_data(char *s);
int cmp_mem(char *m1,char *m2,int len);
void seek_data_in_file(char *s,int len);
void inject(off_t x,off_t size);
void dw_write_hex(struct _fmt *fmt,char *s,off_t x,int xb,int gb);
void dw_write_string(struct _fmt *fmt,char *s,off_t x,int xb,int gb,int zb);
void dw_write_pattern(struct _fmt *fmt,char *s,off_t x,int xb,int gb,int pattern_type);
void dw_write_file(struct _fmt *fmt,char *s,off_t x,int xb,int gb);
void dw_write_random(struct _fmt *fmt,off_t x,int xb,int gb);
void dw_write_le(struct _fmt *fmt,off_t num,off_t x,int xb,int gb);
void dw_write_be(struct _fmt *fmt,off_t num,off_t x,int xb,int gb);
void dw_write_number(struct _fmt *fmt,off_t num,off_t x,int xb,int gb);
void dw_write_over(struct _fmt *fmt,off_t num,off_t x,int xb,int wo_op);
void dw_write_over_2swap(off_t x,int xb);
void dw_write_over_4swap(off_t x,int xb);
void dw_write_over_8swap(off_t x,int xb);


#endif
