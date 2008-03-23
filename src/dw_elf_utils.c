/*
 dw_elf_utils.h : elf file format utils.

 (c) 2007-2008 Fernando Iazeolla

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
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<stdio.h>
#include<readline/readline.h>
#include<readline/history.h>
#include"stdint.h"
#include"main.h"
#include"macho.h"
#include"elf.h"
#include"pe.h"
#include"dw_elf_utils.h"
#include"dw_macho_utils.h"
#include"dw_pe_utils.h"
#include"dw_readline_completion.h"
#include"lang.h"
#include"vars.h"
#include"utils.h"


void load_elf_hd()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph;
	Elf32_Shdr *sh;
	int x,num_ph,num_sh,m_e_phnum,m_e_shnum;
	char path[255];
	elf=(Elf32_Ehdr*)faddr;
	make_table(NULL,"s",-1);
	add_s_var("s","e_ident",TYPE_STRING,&elf->e_ident);
	x=get_data16(elf->e_type);
	add_s_var("s","e_type",TYPE_VAL,&x);
	x=get_data16(elf->e_machine);
	add_s_var("s","e_machine",TYPE_VAL,&x);
	x=get_data32(elf->e_version);
	add_s_var("s","e_version",TYPE_VAL,&x);
	x=get_data32(elf->e_entry);
	add_s_var("s","e_entry",TYPE_VAL,&x);
	x=get_data32(elf->e_phoff);
	add_s_var("s","e_phoff",TYPE_VAL,&x);
	x=get_data32(elf->e_shoff);
	add_s_var("s","e_shoff",TYPE_VAL,&x);
	x=get_data32(elf->e_flags);
	add_s_var("s","e_flags",TYPE_VAL,&x);
	x=get_data16(elf->e_ehsize);
	add_s_var("s","e_ehsize",TYPE_VAL,&x);
	x=get_data16(elf->e_phentsize);
	add_s_var("s","e_phentsize",TYPE_VAL,&x);
	x=get_data16(elf->e_phnum);
	add_s_var("s","e_phnum",TYPE_VAL,&x);
	x=get_data16(elf->e_shentsize);
	add_s_var("s","e_shentsize",TYPE_VAL,&x);
	x=get_data16(elf->e_shnum);
	add_s_var("s","e_shnum",TYPE_VAL,&x);
	x=get_data16(elf->e_shstrndx);
	add_s_var("s","e_shstrndx",TYPE_VAL,&x);
	if(elf->e_phnum>0)
	{
		make_table(NULL,"ph",elf->e_phnum);
		num_ph=0;
		ph=(Elf32_Phdr*)(faddr+elf->e_phoff);
		do
		{
			sprintf(path,"ph[%d]",num_ph);
			x=get_data32(ph->p_type);
			add_s_var(path,"p_type",TYPE_VAL,&x);
			x=get_data32(ph->p_offset);
			add_s_var(path,"p_offset",TYPE_VAL,&x);
			x=get_data32(ph->p_vaddr);
			add_s_var(path,"p_vaddr",TYPE_VAL,&x);
			x=get_data32(ph->p_paddr);
			add_s_var(path,"p_paddr",TYPE_VAL,&x);
			x=get_data32(ph->p_filesz);
			add_s_var(path,"p_filesz",TYPE_VAL,&x);
			x=get_data32(ph->p_memsz);
			add_s_var(path,"p_memsz",TYPE_VAL,&x);
			x=get_data32(ph->p_flags);
			add_s_var(path,"p_flags",TYPE_VAL,&x);
			x=get_data32(ph->p_align);
			add_s_var(path,"p_align",TYPE_VAL,&x);
			ph=(Elf32_Phdr*)((char*)ph+sizeof(Elf32_Phdr));
			num_ph++;
		}while(num_ph<elf->e_phnum);
	}
	if(elf->e_shnum>0)
	{
		make_table(NULL,"sh",elf->e_shnum);
		num_sh=0;
		sh=(Elf32_Shdr*)(faddr+elf->e_shoff);
		do
		{
			sprintf(path,"sh[%d]",num_sh);
			x=get_data32(sh->sh_name);
			add_s_var(path,"sh_name",TYPE_VAL,&x);
			x=get_data32(sh->sh_type);
			add_s_var(path,"sh_type",TYPE_VAL,&x);
			x=get_data32(sh->sh_flags);
			add_s_var(path,"sh_flags",TYPE_VAL,&x);
			x=get_data32(sh->sh_addr);
			add_s_var(path,"sh_addr",TYPE_VAL,&x);
			x=get_data32(sh->sh_offset);
			add_s_var(path,"sh_offset",TYPE_VAL,&x);
			x=get_data32(sh->sh_size);
			add_s_var(path,"sh_size",TYPE_VAL,&x);
			x=get_data32(sh->sh_link);
			add_s_var(path,"sh_link",TYPE_VAL,&x);
			x=get_data32(sh->sh_info);
			add_s_var(path,"sh_info",TYPE_VAL,&x);
			x=get_data32(sh->sh_addralign);
			add_s_var(path,"sh_addralign",TYPE_VAL,&x);
			x=get_data32(sh->sh_entsize);
			add_s_var(path,"sh_entsize",TYPE_VAL,&x);
			sh=(Elf32_Shdr*)((char*)sh+sizeof(Elf32_Shdr));
			num_sh++;
		}while(num_sh<elf->e_shnum);
	}
	//make_table(NULL,"ph",2);
	//add_s_var("ph[1]","pippo",TYPE_VAL,&xx);
	//make_table("ph[2]","ptable",4);
	//add_s_var("ph[2]->ptable[1]","pippo",TYPE_VAL,&xx);
	printf("elf\n");
}
int get_offset_elf(char *s,char p)
{
	
	return 0;
}
