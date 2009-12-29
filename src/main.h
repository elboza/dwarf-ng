/*
 main.h : dwarf's main header.

 (c) 2007-2009 Fernando Iazeolla

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

#ifndef DW_MAIN_H
#define DW_MAIN_H
#if HAVE_CONFIG_H
#include "../config.h"
#endif
//#define VERSION "0.1.88"
#define MAX_FILENAME 255
#define MAX_STR 255
#define MAX_CMD 1024
#define VERBOSE 1
#define NOVERBOSE 0
#define FALSE 0
#define TRUE 1
#define NO 0
#define YES 1
char filename[MAX_FILENAME],*cmd,scriptfile[MAX_FILENAME];
int forced;  /*if set force a file to be treat as a given kind reguardless of what really kind is*/
int file_type;
int file_endian;
int cpu_endian;
int file_bit_class;
void *faddr;
int sh_shstrtab;
struct m_action{
	int file;
	int shell;
	int script;
	int exec;
	};
struct m_cfg{
	int tmpworkcopy;
	int verbose;
	int do_debug;
	int file_changed;
	int flush_before_refresh;
	int ask_flush;
	int createHD_autoshift;
	int createHD_autoupdate;
	char tmpworkdir[MAX_FILENAME];
};
struct m_files{
	char originalfilename[MAX_FILENAME];
	char tmpcopy[MAX_FILENAME];
	int fd,original_fd;	//fd is the tmpcopyworking file descriptor
	int writable;
};
typedef enum {little_endian,big_endian} endian;
typedef enum {bit8,bit16,bit32,bit64} filebitclass;
//file types
typedef enum {FT_NULL,FT_ELF,FT_MZ,FT_MACHO,FT_FAT_MACHO,FT_PE} type_of_file;
//#define FT_NULL			0
//#define FT_ELF			1
//#define FT_MZ			2
//#define FT_MACHO		3
//#define FT_FAT_MACHO	4
//#define FT_PE			5

#define QUITTING	123

#endif

