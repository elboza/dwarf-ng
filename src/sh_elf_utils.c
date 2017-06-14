/*
 sh_elf_utils.c : dwarf's shell elf utils functions file.

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
#include "libdwarf.h"
#include"dw_readline_completion.h"
#include"sh_elf_utils.h"
#include"output.h"
//#include"../../config.h"

void print_elf_hdr()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	char str[10];
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			strncpy(str,(char *)elf64->e_ident,4);
			printf("e_ident: %s\n",str);
			printf("e_type: 0x%x (%d)\n",get_data16(elf64->e_type),get_data16(elf64->e_type));
			printf("e_machine: 0x%x (%d)\n",get_data16(elf64->e_machine),get_data16(elf64->e_machine));
			printf("e_version: 0x%x (%d)\n",get_data16(elf64->e_version),get_data16(elf64->e_version));
			printf("e_entry: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->e_entry),get_data64(elf64->e_entry));
			printf("e_phoff: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->e_phoff),get_data64(elf64->e_phoff));
			printf("e_shoff: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->e_shoff),get_data64(elf64->e_shoff));
			printf("e_flags: 0x%x (%d)\n",get_data32(elf64->e_flags),get_data32(elf64->e_flags));
			printf("e_ehsize: 0x%x (%d)\n",get_data16(elf64->e_ehsize),get_data16(elf64->e_ehsize));
			printf("e_phentsize: 0x%x (%d)\n",get_data16(elf64->e_phentsize),get_data16(elf64->e_phentsize));
			printf("e_phnum: 0x%x (%d)\n",get_data16(elf64->e_phnum),get_data16(elf64->e_phnum));
			printf("e_shentsize: 0x%x (%d)\n",get_data16(elf64->e_shentsize),get_data16(elf64->e_shentsize));
			printf("e_shnum: 0x%x (%d)\n",get_data16(elf64->e_shnum),get_data16(elf64->e_shnum));
			printf("e_shstrndx: 0x%x (%d)\n",get_data16(elf64->e_shstrndx),get_data16(elf64->e_shstrndx));
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			strncpy(str,(char *)elf32->e_ident,4);
			printf("e_ident: %s\n",str);
			printf("e_type: 0x%x (%d)\n",get_data16(elf32->e_type),get_data16(elf32->e_type));
			printf("e_machine: 0x%x (%d)\n",get_data16(elf32->e_machine),get_data16(elf32->e_machine));
			printf("e_version: 0x%x (%d)\n",get_data16(elf32->e_version),get_data16(elf32->e_version));
			printf("e_entry: 0x%x (%d)\n",get_data32(elf32->e_entry),get_data32(elf32->e_entry));
			printf("e_phoff: 0x%x (%d)\n",get_data32(elf32->e_phoff),get_data32(elf32->e_phoff));
			printf("e_shoff: 0x%x (%d)\n",get_data32(elf32->e_shoff),get_data32(elf32->e_shoff));
			printf("e_flags: 0x%x (%d)\n",get_data32(elf32->e_flags),get_data32(elf32->e_flags));
			printf("e_ehsize: 0x%x (%d)\n",get_data16(elf32->e_ehsize),get_data16(elf32->e_ehsize));
			printf("e_phentsize: 0x%x (%d)\n",get_data16(elf32->e_phentsize),get_data16(elf32->e_phentsize));
			printf("e_phnum: 0x%x (%d)\n",get_data16(elf32->e_phnum),get_data16(elf32->e_phnum));
			printf("e_shentsize: 0x%x (%d)\n",get_data16(elf32->e_shentsize),get_data16(elf32->e_shentsize));
			printf("e_shnum: 0x%x (%d)\n",get_data16(elf32->e_shnum),get_data16(elf32->e_shnum));
			printf("e_shstrndx: 0x%x (%d)\n",get_data16(elf32->e_shstrndx),get_data16(elf32->e_shstrndx));
			break;
	}
}
void print_sh(int num)
{
	Elf32_Shdr *elf32;
	Elf64_Shdr *elf64;
	//char str[10];
	int shentsize,n;
	off_t shoffset;
	if(num>max_sh()) {printf("out of sh bound.\n"); return;}
	shoffset=elf_get_sh_offs();
	shentsize=elf_get_sh_entsize();
	for(n=0;n<num;n++)
			{
				shoffset=shoffset+shentsize;
			}
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
			printf("sh_name: 0x%x (%d) [%s]\n",get_data32(elf64->sh_name),get_data32(elf64->sh_name),print_sh_strtable((int)(get_data32(elf64->sh_name))));
			printf("sh_type: 0x%x (%d)\n",get_data32(elf64->sh_type),get_data32(elf64->sh_type));
			printf("sh_flags: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_flags),get_data64(elf64->sh_flags));
			printf("sh_addr: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_addr),get_data64(elf64->sh_addr));
			printf("sh_offset: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_offset),get_data64(elf64->sh_offset));
			printf("sh_size: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_size),get_data64(elf64->sh_size));
			printf("sh_link: 0x%x (%d)\n",get_data32(elf64->sh_link),get_data32(elf64->sh_link));
			printf("sh_info: 0x%x (%d)\n",get_data32(elf64->sh_info),get_data32(elf64->sh_info));
			printf("sh_addralign: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_addralign),get_data64(elf64->sh_addralign));
			printf("sh_entsize: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->sh_entsize),get_data64(elf64->sh_entsize));
  			break;
		case bit32:
		default:
			elf32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
			printf("sh_name: 0x%x (%d) [%s]\n",get_data32(elf32->sh_name),get_data32(elf32->sh_name),print_sh_strtable((int)(get_data32(elf32->sh_name))));
			printf("sh_type: 0x%x (%d)\n",get_data32(elf32->sh_type),get_data32(elf32->sh_type));
			printf("sh_flags: 0x%x (%d)\n",get_data32(elf32->sh_flags),get_data32(elf32->sh_flags));
			printf("sh_addr: 0x%x (%d)\n",get_data32(elf32->sh_addr),get_data32(elf32->sh_addr));
			printf("sh_offset: 0x%x (%d)\n",get_data32(elf32->sh_offset),get_data32(elf32->sh_offset));
			printf("sh_size: 0x%x (%d)\n",get_data32(elf32->sh_size),get_data32(elf32->sh_size));
			printf("sh_link: 0x%x (%d)\n",get_data32(elf32->sh_link),get_data32(elf32->sh_link));
			printf("sh_info: 0x%x (%d)\n",get_data32(elf32->sh_info),get_data32(elf32->sh_info));
			printf("sh_addralign: 0x%x (%d)\n",get_data32(elf32->sh_addralign),get_data32(elf32->sh_addralign));
			printf("sh_entsize: 0x%x (%d)\n",get_data32(elf32->sh_entsize),get_data32(elf32->sh_entsize));
			break;
	}
}
void print_ph(int num)
{
	Elf32_Phdr *elf32;
	Elf64_Phdr *elf64;
	//char str[10];
	int phentsize,n;
	off_t phoffset;
	if(num>max_ph()) {printf("out of ph bound.\n"); return;}
	phoffset=elf_get_ph_offs();
	phentsize=elf_get_ph_entsize();
	for(n=0;n<num;n++)
			{
				phoffset=phoffset+phentsize;
			}
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Phdr*)(fc_ptr->faddr+phoffset);
			printf("p_type: 0x%x (%d)\n",get_data32(elf64->p_type),get_data32(elf64->p_type));
			printf("p_flags: 0x%x (%d)\n",get_data32(elf64->p_flags),get_data32(elf64->p_flags));
			printf("p_offset: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_offset),get_data64(elf64->p_offset));
			printf("p_vaddr: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_vaddr),get_data64(elf64->p_vaddr));
			printf("p_paddr: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_paddr),get_data64(elf64->p_paddr));
			printf("p_filesz: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_filesz),get_data64(elf64->p_filesz));
			printf("p_memsz: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_memsz),get_data64(elf64->p_memsz));
			printf("p_align: 0x%"PRIx64" (%"PRId64")\n",get_data64(elf64->p_align),get_data64(elf64->p_align));
			break;
		case bit32:
		default:
			elf32=(Elf32_Phdr*)(fc_ptr->faddr+phoffset);
			printf("p_type: 0x%x (%d) [%s]\n",get_data32(elf32->p_type),get_data32(elf32->p_type),print_ph_type((int)(get_data32(elf32->p_type))));
			printf("p_flags: 0x%x (%d)\n",get_data32(elf32->p_flags),get_data32(elf32->p_flags));
			printf("p_offset: 0x%x (%d)\n",get_data32(elf32->p_offset),get_data32(elf32->p_offset));
			printf("p_vaddr: 0x%x (%d)\n",get_data32(elf32->p_vaddr),get_data32(elf32->p_vaddr));
			printf("p_paddr: 0x%x (%d)\n",get_data32(elf32->p_paddr),get_data32(elf32->p_paddr));
			printf("p_filesz: 0x%x (%d)\n",get_data32(elf32->p_filesz),get_data32(elf32->p_filesz));
			printf("p_memsz: 0x%x (%d)\n",get_data32(elf32->p_memsz),get_data32(elf32->p_memsz));
			printf("p_align: 0x%x (%d)\n",get_data32(elf32->p_align),get_data32(elf32->p_align));
			break;
	}
}
void print_shlist()
{
	Elf32_Shdr *sh32;
	Elf64_Shdr *sh64;
	int shentsize,n;
	off_t shoffset;
	shoffset=elf_get_sh_offs();
	shentsize=elf_get_sh_entsize();
	for(n=0;n<=max_sh();n++)
	{
		printf("sh[%d] ",n);
		switch(fc_ptr->file_bit_class) {
			case bit64:
				sh64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
				printf("[%s] 0x%"PRIx64"\n",print_sh_strtable((int)(get_data32(sh64->sh_name))),get_data64(sh64->sh_offset));
				break;
			case bit32:
			default:
				sh32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
				printf("[%s] 0x%x\n",print_sh_strtable((int)(get_data32(sh32->sh_name))),get_data32(sh32->sh_offset));
				break;
		}
		shoffset=shoffset+shentsize;
	}
}
void print_phlist()
{
	Elf32_Phdr *ph32;
	Elf64_Phdr *ph64;
	int phentsize,n;
	off_t phoffset;
	phoffset=elf_get_ph_offs();
	phentsize=elf_get_ph_entsize();
	for(n=0;n<=max_ph();n++)
	{
		printf("ph[%d] ",n);
		switch(fc_ptr->file_bit_class) {
			case bit64:
				ph64=(Elf64_Phdr*)(fc_ptr->faddr+phoffset);
				printf("[%s] 0x%"PRIx64"\n",print_ph_type((int)(get_data32(ph64->p_type))),get_data64(ph64->p_offset));
				break;
			case bit32:
			default:
				ph32=(Elf32_Phdr*)(fc_ptr->faddr+phoffset);
				printf("[%s] 0x%x\n",print_ph_type((int)(get_data32(ph32->p_type))),get_data32(ph32->p_offset));
				break;
		}
		phoffset=phoffset+phentsize;
	}
}
char* print_sh_strtable(int num)
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	Elf32_Shdr *sh32;
	Elf64_Shdr *sh64;
	int shentsize,n,x;
	off_t shoffset,shstrndxoffset;
	char *strtable;
	shoffset=elf_get_sh_offs();
	shentsize=elf_get_sh_entsize();
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*)fc_ptr->faddr;
			x=get_data16(elf64->e_shstrndx);
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*)fc_ptr->faddr;
			x=get_data16(elf32->e_shstrndx);
			break;
	}
	for(n=0;n<x;n++)
	{
		shoffset=shoffset+shentsize;
	}
	switch(fc_ptr->file_bit_class) {
		case bit64:
			sh64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
			shstrndxoffset=get_data64(sh64->sh_offset);
			break;
		case bit32:
		default:
			sh32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
			shstrndxoffset=get_data32(sh32->sh_offset);
			break;
	}
	strtable=(char*)(fc_ptr->faddr+shstrndxoffset+num);
	//printf("[%s]",strtable);
	return(strtable);
}
char* print_ph_type(int num)
{
	static char *ph_table[]={"PT_NULL","PT_LOAD","PT_DYNAMIC","PT_INTERP","PT_NOTE","PT_SHLIB","PT_PHDR","PT_TLS","PT_NUM",""};
	if((num<0) || (num>8)) return " ";
	return ph_table[num];
}
void print_elf_s_var(struct _fmt *fmt,struct _structvar *ptr)
{
	int sect=SECT_NULL;
	struct _var *var;
	if((strcmp(ptr->name,"elf"))==0) sect=SECT_ELF;
	if((strcmp(ptr->name,"ph"))==0) sect=SECT_PH;
	if((strcmp(ptr->name,"sh"))==0) sect=SECT_SH;
	if(!ptr->next) {
		switch(sect) {
			case SECT_ELF:
				if(ptr->num!=-1) {printf("invalid section.\n"); return;}
				print_elf_hdr();
				break;
			case SECT_PH:
				if(ptr->num==-1) print_phlist(); else print_ph(ptr->num);
				break;
			case SECT_SH:
				if(ptr->num==-1) print_shlist(); else print_sh(ptr->num);
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(ptr->next->num!=-1) {printf("invalid section.\n"); return;}
		var=get_elf_var(sect,ptr->num,ptr->next->name);
		if(!var) return;
		switch(var->type) {
			case VART_WORD:
				printf("%s\n",var->val.s);
				break;
			case VART_NUM:
				do_print_expr(fmt,var->val.num);
				break;
			default:
				printf("unknown type.\n");
				break;
		}
		destroytmpvar(var);
	}
}
void add_elf_completion()
{
	int n,x;
	char s[255];
#ifdef HAVE_LIBREADLINE
	add_completion("elf",NULL,comp_discardable);
	add_completion("elf::e_ident",NULL,comp_discardable);
	add_completion("elf::e_type",NULL,comp_discardable);
	add_completion("elf::e_machine",NULL,comp_discardable);
	add_completion("elf::e_version",NULL,comp_discardable);
	add_completion("elf::e_entry",NULL,comp_discardable);
	add_completion("elf::e_phoff",NULL,comp_discardable);
	add_completion("elf::e_shoff",NULL,comp_discardable);
	add_completion("elf::e_flags",NULL,comp_discardable);
	add_completion("elf::e_ehsize",NULL,comp_discardable);
	add_completion("elf::e_phentsize",NULL,comp_discardable);
	add_completion("elf::e_phnum",NULL,comp_discardable);
	add_completion("elf::e_shentsize",NULL,comp_discardable);
	add_completion("elf::e_shnum",NULL,comp_discardable);
	add_completion("elf::e_shstrndx",NULL,comp_discardable);
	add_completion("sh",NULL,comp_discardable);
	add_completion("ph",NULL,comp_discardable);
	x=max_ph();
	for(n=0;n<=x;n++)
	{
		sprintf(s,"ph[%d]",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_type",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_offset",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_vaddr",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_paddr",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_filesz",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_memsz",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_flags",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"ph[%d]::p_align",n);
		add_completion(s,NULL,comp_discardable);
	}
	x=max_sh();
	for(n=0;n<=x;n++)
	{
		sprintf(s,"sh[%d]",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_name",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_type",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_flags",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_addr",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_offset",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_size",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_link",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_info",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_addralign",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sh[%d]::sh_entsize",n);
		add_completion(s,NULL,comp_discardable);
	}
#endif
}
