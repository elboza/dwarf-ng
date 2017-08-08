/*
 sh_switchers.c : dwarf's shell switchers file functions.

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
#include<stdio.h>
#include<string.h>
#include"libdwarf.h"
#include"dw_readline_completion.h"
#include"output.h"
#include "sh_elf_utils.h"
#include "sh_macho_utils.h"
#include "sh_pe_utils.h"

void sw_do_info(void)
{
	off_t len;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	len=filesize(fc_ptr->fd);
	printf("%s : %lld bytes, ",fc_ptr->name,(long long)len);
	switch(fc_ptr->file_type) {
		case FT_MZEXE:
			printf("MZ EXE 16 bit little endian\n");
			printf("main header: mz\n");
			break;
		case FT_PE:
			printf("PE Porable Executable %s cpu %d bit %s\n",decode_file_cpu(fc_ptr->file_cpu_type),decode_file_bit(fc_ptr->file_bit_class),decode_file_endian(fc_ptr->file_endian));
			printf("mzexe header: mz\n");
			printf("pe header: pe\n");
			printf("sections: sect[0..%d]\n",get_max_pesect());
			break;
		case FT_ELF:
			printf("ELF %s cpu %d bit %s\n",decode_file_cpu(fc_ptr->file_cpu_type),decode_file_bit(fc_ptr->file_bit_class),decode_file_endian(fc_ptr->file_endian));
			printf("main header: elf\n");
			printf("program header: ph [0..%d]\n",max_ph());
			printf("section header: sh [0..%d]\n",max_sh());
			break;
		case FT_MACHO:
			printf("Mach-O %s cpu %d bit %s\n",decode_file_cpu(fc_ptr->file_cpu_type),decode_file_bit(fc_ptr->file_bit_class),decode_file_endian(fc_ptr->file_endian));
			printf("main header: mac\n");
			printf("load commands: lc[0..%d]\n",max_lc(-1));
			break;
		case FT_FATMACHO:
			printf("Fat Mach-O\n");
			printf("main header: fat\n");
			printf("arch headers: arch [0..%d]\n",max_fat_arch());
			break;
		case FT_FAT:
			printf("FAT fs\n");
			break;
		case FT_FAT16:
			printf("FAT16\n");
			break;
		case FT_FAT32:
			printf("FAT32\n");
			break;
		case FT_EXT:
			printf("EXT fs\n");
			break;
		case FT_MBR:
			printf("MBR Master Boot Record\n");
			break;
		default:
			printf("unknown file type.\n");
			break;
	}
}
off_t get_offset(struct _structvar *ptr,int endoffset)
{
	off_t offs;
	if(!fc_ptr) {printf("no file opened.\n"); return 0;}
	switch(fc_ptr->file_type) {
		case FT_ELF:
			offs=get_elf_offset(ptr,endoffset);
			break;
		case FT_FATMACHO:
			offs=get_fatmacho_offset(ptr,endoffset);
			break;
		case FT_MACHO:
			offs=get_macho_offset(ptr,endoffset);
			break;
		case FT_PE:
		case FT_MZEXE:
			offs=get_pe_offset(ptr,endoffset);
			break;
		default:
			offs=0;
			break;
	}
	return offs;
}
void add_sh_completion()
{
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	switch(fc_ptr->file_type) {
		case FT_ELF:
			add_elf_completion();
			break;
		case FT_FATMACHO:
			add_fatmacho_completion();
			break;
		case FT_MACHO:
			add_macho_completion();
			break;
		case FT_PE:
		case FT_MZEXE:
			add_pe_completion();
			break;
		default:
			
			break;
	}
}
void do_create(char *ss,off_t offs,int grow,int update)
{
	int sect;
	sect=SECT_NULL;
	if(!fc_ptr) {fprintf(stderr,"no file opened.\n"); return;}
	switch(fc_ptr->file_type) {
		case FT_ELF:
			if((strcmp(ss,"elf"))==0) sect=SECT_ELF;
			if((strcmp(ss,"ph"))==0) sect=SECT_PH;
			if((strcmp(ss,"sh"))==0) sect=SECT_SH;
			//create_elf_hdr(sect,offs,grow,update);
			break;
		case FT_FATMACHO:
			if((strcmp(ss,"fat"))==0) sect=SECT_FATMACHO;
			if((strcmp(ss,"arch"))==0) sect=SECT_FATARCH;
			//create_fatmacho_hdr(sect,offs,grow,update);
			break;
		case FT_MACHO:
			if((strcmp(ss,"mac"))==0) sect=SECT_MAC;
			if((strcmp(ss,"lc"))==0) sect=SECT_LC;
			if((strcmp(ss,"sect"))==0) sect=SECT_MACSECT;
			//create_macho_hdr(sect,offs,grow,update);
			break;
		case FT_PE:
			if((strcmp(ss,"mz"))==0) sect=SECT_MZ;
			if((strcmp(ss,"pe"))==0) sect=SECT_PE;
			if((strcmp(ss,"sect"))==0) sect=SECT_PESECT;
			//create_pe_hdr(sect,offs,grow,update);
			break;
		default:
			printf("unknown file type, cannot create header.\n");
			break;
	}
	printf("%s %lld %d %d %d",ss,(long long)offs,grow,update,sect);
}
char* decode_file_type(int x){
	char *file_type[]={"Unknown","MZ-exe","PE","ELF","Macho","FAT Macho","FAT","FAT16","FAT32","EXT","MBR"};
	return file_type[x];
}
void dw_create_section(char *s,off_t x,int xb){
	int sect;
	sect=SECT_NULL;
	if(!fc_ptr) {fprintf(stderr,"no file open\n"); return;}
	printf("create sect....\n");
	switch(fc_ptr->file_type) {
		case FT_ELF:
			if((strcmp(s,"elf"))==0) sect=SECT_ELF;
			if((strcmp(s,"ph"))==0) sect=SECT_PH;
			if((strcmp(s,"sh"))==0) sect=SECT_SH;
			//create_elf_hdr(sect,offs,grow,update);
			break;
		case FT_FATMACHO:
			if((strcmp(s,"fat"))==0) sect=SECT_FATMACHO;
			if((strcmp(s,"arch"))==0) sect=SECT_FATARCH;
			//create_fatmacho_hdr(sect,offs,grow,update);
			break;
		case FT_MACHO:
			if((strcmp(s,"mac"))==0) sect=SECT_MAC;
			if((strcmp(s,"lc"))==0) sect=SECT_LC;
			if((strcmp(s,"sect"))==0) sect=SECT_MACSECT;
			//create_macho_hdr(sect,offs,grow,update);
			break;
		case FT_PE:
			if((strcmp(s,"mz"))==0) sect=SECT_MZ;
			if((strcmp(s,"pe"))==0) sect=SECT_PE;
			if((strcmp(s,"sect"))==0) sect=SECT_PESECT;
			//create_pe_hdr(sect,offs,grow,update);
			break;
		default:
			printf("unknown file type, cannot create header.\n");
			break;
	}
	printf("%s %lld\n",s,(long long)x);
}