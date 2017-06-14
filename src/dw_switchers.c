/*
 dw_switchers.c : dwarf's switchers file functions.

 (c) 2007-2011 Fernando Iazeolla

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

void file_probe()
{
	struct mach_header *mac;
	struct fat_header *fat;
	Elf32_Ehdr *elf;
	_IMAGE_DOS_HEADER *mzexe;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_FILE_HEADER *pe2;
	int x;
	//probe if is mach-o
	if(!fc_ptr) {printf("no file loaded!\n"); return;}
	mac=(struct mach_header*)fc_ptr->faddr;
	if((mac->magic==MH_MAGIC)||(mac->magic==MH_CIGAM)||(mac->magic==MH_MAGIC_64)||(mac->magic==MH_CIGAM_64))
	{
		fc_ptr->file_type=FT_MACHO;
		if(mac->magic==MH_MAGIC) {fc_ptr->file_endian=fc_ptr->cpu_endian;fc_ptr->file_bit_class=bit32;}
		if(mac->magic==MH_CIGAM) {fc_ptr->file_endian=(~fc_ptr->cpu_endian) & 1;fc_ptr->file_bit_class=bit32;}
		if(mac->magic==MH_MAGIC_64) {fc_ptr->file_endian=fc_ptr->cpu_endian;fc_ptr->file_bit_class=bit64;}
		if(mac->magic==MH_CIGAM_64) {fc_ptr->file_endian=(~fc_ptr->cpu_endian) & 1;fc_ptr->file_bit_class=bit64;}
		fc_ptr->file_cpu_type=get_data32(mac->cputype);
		//if(verbose) printf("File type: Mach-O\n");
		return;
	}
	//probe if is FAT mach-o
	fat=(struct fat_header*)fc_ptr->faddr;
	if((fat->magic==FAT_MAGIC)||(fat->magic==FAT_CIGAM))
	{
		fc_ptr->file_type=FT_FATMACHO;
		fc_ptr->file_endian=big_endian;
		//if(verbose) printf("File type; FAT Mach-O\n");
		return;
	}
	//probe if it is an ELF
	elf=(Elf32_Ehdr*)fc_ptr->faddr;
	if((strncmp(ELFMAG,(char *)elf->e_ident,4))==0)
	{
		fc_ptr->file_type=FT_ELF;
		if(elf->e_ident[EI_DATA]==ELFDATA2LSB) fc_ptr->file_endian=little_endian;
		if(elf->e_ident[EI_DATA]==ELFDATA2MSB) fc_ptr->file_endian=big_endian;
		if(elf->e_ident[EI_CLASS]==ELFCLASS32) fc_ptr->file_bit_class=bit32; //load 32 bit arch
		if(elf->e_ident[EI_CLASS]==ELFCLASS64) fc_ptr->file_bit_class=bit64; // load 64 bit arch
		fc_ptr->file_cpu_type=get_data16(elf->e_machine);
		//probe_sh_shstrtab();
		//if(verbose) printf("File type: Elf\n");
		return;
	}
	//probe if it is a PE
	//probe if it is a MZ
	mzexe=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	x=(int)mzexe->e_magic;
	//printf("sig: %x (%x)\n",x,mzexe->e_lfanew);
	if((x==IMAGE_DOS_SIGNATURE)||(x==0x4d5a))
	{
		fc_ptr->file_type=FT_MZEXE;
		fc_ptr->file_endian=little_endian;
		//count=0;
		x=get_data32(mzexe->e_lfanew);
		pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+x);
		x=get_data32(pe->Signature);
		if(x==IMAGE_NT_SIGNATURE)
		{
			fc_ptr->file_type=FT_PE;
			pe2=(_IMAGE_FILE_HEADER*) (&pe->FileHeader);
			fc_ptr->file_cpu_type=get_data16(pe2->Machine);
			switch(fc_ptr->file_cpu_type) {
				case IMAGE_FILE_MACHINE_AMD64:
				case IMAGE_FILE_MACHINE_IA64:
					fc_ptr->file_bit_class=bit64;
					fc_ptr->file_endian=little_endian;
					break;
				case IMAGE_FILE_MACHINE_MIPS16:
				case IMAGE_FILE_MACHINE_MIPSFPU16:
					fc_ptr->file_bit_class=bit16;
					break;
				default:
					fc_ptr->file_bit_class=bit32;
					fc_ptr->file_endian=little_endian;
					break;
			}
			//count=1;
		}
		//if(verbose) {printf("File type: ");if(count) printf("PE\n"); else printf("MZ-exe");}
	}
	
}
char* decode_file_cpu(int cpu)
{
	static char s[VARNAME_LEN];
	if(!fc_ptr) return NULL;
	switch(fc_ptr->file_type) {
		case FT_ELF:
			//printf("*** %s ",dw_elf_decode_file_cpu(cpu));
			dw_elf_decode_file_cpu(s,cpu);
			break;
		case FT_FATMACHO:
		case FT_MACHO:
			dw_macho_decode_file_cpu(s,cpu);
			break;
		case FT_PE:
			dw_pe_decode_file_cpu(s,cpu);
			break;
		default:
			strcpy(s,"unknown");
			break;
	}
	return s;
}
struct _var* get_s_val(struct _structvar *ptr)
{
	struct _var *var;
	if(!fc_ptr) {printf("unknown file.\n"); return NULL;}
	switch(fc_ptr->file_type){
		case FT_ELF:
			var=elf_get_s_val(ptr);
			return var;
			break;
		case FT_FATMACHO:
			var=fatmacho_get_s_val(ptr);
			return var;
			break;
		case FT_MACHO:
			var=macho_get_s_val(ptr);
			return var;
			break;
		case FT_PE:
			
			break;
		default:
			printf("unknown file.\n");
			return NULL;
			break;
	}
	return NULL;
}
void set_s_val(struct _structvar *ptr,struct _var *val)
{
	if(!fc_ptr) {printf("unknown file.\n"); return;}
	switch(fc_ptr->file_type){
		case FT_ELF:
			elf_set_s_val(ptr,val);
			break;
		case FT_FATMACHO:
			fatmacho_set_s_val(ptr,val);
			break;
		case FT_MACHO:
			macho_set_s_val(ptr,val);
			break;
		case FT_PE:
			pe_set_s_val(ptr,val);
			break;
		default:
			printf("unknown file.\n");
			break;
	}
	destroytmpvar(val);
}
