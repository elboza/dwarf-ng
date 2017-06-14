/*
 dw_elf_utils.c : elf file format utils.

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
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include"libdwarf.h"

void dw_elf_decode_file_cpu(char *s,int cpu)
{
	static char *cpu_table[]={"No machine","AT&T WE 32100","SUN SPARC","Intel 80386","Motorola m68k family","Motorola m88k family","Intel 80860","MIPS R3000 big-endian","IBM System/370","MIPS R3000 little-endian","?","?","?","?","?","HPPA","?","Fujitsu VPP500","Sun's \"v8plus\"","Intel 80960","PowerPC","PowerPC 64-bit","IBM S390","23","24","25","26","27","28","29","30","31","32","33","34","35","NEC V800 series","Fujitsu FR20","TRW RH-32","Motorola RCE","ARM","Digital Alpha","Hitachi SH","SPARC v9 64-bit","Siemens Tricore","Argonaut RISC Core","Hitachi H8/300","Hitachi H8/300H","Hitachi H8S","Hitachi H8/500","Intel Merced","Stanford MIPS-X","Motorola Coldfire","Motorola M68HC12","Fujitsu MMA Multimedia Accelerator","Siemens PCP","Sony nCPU embeeded RISC","Denso NDR1 microprocessor","Motorola Start*Core processor","Toyota ME16 processor","STMicroelectronic ST100 processor","Advanced Logic Corp. Tinyj emb.fam","AMD x86-64 architecture","Sony DSP Processor"};
	strncpy(s,cpu_table[cpu],VARNAME_LEN);
	//return cpu_table[cpu];
}
int max_sh()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf64->e_shnum)-1;
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf32->e_shnum)-1;
			break;
	}
	return x;
}
int max_ph()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf64->e_phnum)-1;
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf32->e_phnum)-1;
			break;
	}
	return x;
}
struct _var* elf_get_s_val(struct _structvar *ptr)
{
	int sect,num;
	char varname[VARNAME_LEN];
	struct _var *var;
	if(!ptr) return NULL;
	strcpy(varname,"");
	num=ptr->num;
	if((strcmp(ptr->name,"elf"))==0) sect=SECT_ELF;
	if((strcmp(ptr->name,"ph"))==0) sect=SECT_PH;
	if((strcmp(ptr->name,"sh"))==0) sect=SECT_SH;
	if(ptr->next) {strncpy(varname,ptr->next->name,VARNAME_LEN);if(ptr->next->num!=-1) {printf("invalid section.\n");return NULL;}}
	var=get_elf_var(sect,num,varname);
	return var;
}
struct _var* get_elf_var(int sect,int num,char *varname)
{
	struct _var *var;
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	Elf32_Phdr *ph32;
	Elf64_Phdr *ph64;
	Elf32_Shdr *sh32;
	Elf64_Shdr *sh64;
	int shentsize,n,phentsize;
	off_t shoffset,phoffset;
	if(!fc_ptr) {printf("no file opened.\n"); return NULL;}
	var=createtmpvar();
	if(!var) return NULL;
	switch(sect) {
		case SECT_ELF:
			if(num!=-1) {printf("wrong section.\n"); destroytmpvar(var);return NULL;}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					elf64=(Elf64_Ehdr*) fc_ptr->faddr;
					if((strcmp(varname,"e_ident"))==0) {var->type=VART_WORD;var->val.s=(char*)malloc(10);if(!var->val.s) die("error allocating str mem");strncpy(var->val.s,(char *)elf64->e_ident,4);}
					else if((strcmp(varname,"e_type"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_type);}
					else if((strcmp(varname,"e_machine"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_machine);}
					else if((strcmp(varname,"e_version"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf64->e_version);}
					else if((strcmp(varname,"e_entry"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf64->e_entry);}
					else if((strcmp(varname,"e_phoff"))==0) {var->type=VART_NUM;var->val.num=get_data64(elf64->e_phoff);}
					else if((strcmp(varname,"e_shoff"))==0) {var->type=VART_NUM;var->val.num=get_data64(elf64->e_shoff);}
					else if((strcmp(varname,"e_flags"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf64->e_flags);}
					else if((strcmp(varname,"e_ehsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_ehsize);}
					else if((strcmp(varname,"e_phentsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_phentsize);}
					else if((strcmp(varname,"e_phnum"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_phnum);}
					else if((strcmp(varname,"e_shentsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_shentsize);}
					else if((strcmp(varname,"e_shnum"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_shnum);}
					else if((strcmp(varname,"e_shstrndx"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf64->e_shstrndx);}
					break;
				case bit32:
				default:
					elf32=(Elf32_Ehdr*) fc_ptr->faddr;
					if((strcmp(varname,"e_ident"))==0) {var->type=VART_WORD;var->val.s=(char*)malloc(10);if(!var->val.s) die("error allocating str mem");strncpy(var->val.s,(char *)elf32->e_ident,4);}
					else if((strcmp(varname,"e_type"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_type);}
					else if((strcmp(varname,"e_machine"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_machine);}
					else if((strcmp(varname,"e_version"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf32->e_version);}
					else if((strcmp(varname,"e_entry"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf32->e_entry);}
					else if((strcmp(varname,"e_phoff"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf32->e_phoff);}
					else if((strcmp(varname,"e_shoff"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf32->e_shoff);}
					else if((strcmp(varname,"e_flags"))==0) {var->type=VART_NUM;var->val.num=get_data32(elf32->e_flags);}
					else if((strcmp(varname,"e_ehsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_ehsize);}
					else if((strcmp(varname,"e_phentsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_phentsize);}
					else if((strcmp(varname,"e_phnum"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_phnum);}
					else if((strcmp(varname,"e_shentsize"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_shentsize);}
					else if((strcmp(varname,"e_shnum"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_shnum);}
					else if((strcmp(varname,"e_shstrndx"))==0) {var->type=VART_NUM;var->val.num=get_data16(elf32->e_shstrndx);}
					break;
			}
		case SECT_PH:
			if(num>max_ph()) {printf("out of ph bound.\n"); return NULL;}
			phoffset=elf_get_ph_offs();
			phentsize=elf_get_ph_entsize();
			for(n=0;n<num;n++)
					{
						phoffset=phoffset+phentsize;
					}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					ph64=(Elf64_Phdr*)(fc_ptr->faddr+phoffset);
					if((strcmp(varname,"p_type"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph64->p_type);}
					else if((strcmp(varname,"p_flags"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph64->p_flags);}
					else if((strcmp(varname,"p_offset"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_offset);}
					else if((strcmp(varname,"p_vaddr"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_vaddr);}
					else if((strcmp(varname,"p_paddr"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_paddr);}
					else if((strcmp(varname,"p_filesz"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_filesz);}
					else if((strcmp(varname,"p_memsz"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_memsz);}
					else if((strcmp(varname,"p_align"))==0) {var->type=VART_NUM;var->val.num=get_data64(ph64->p_align);}
					break;
				case bit32:
				default:
					ph32=(Elf32_Phdr*)(fc_ptr->faddr+phoffset);
					if((strcmp(varname,"p_type"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_type);}
					else if((strcmp(varname,"p_flags"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_flags);}
					else if((strcmp(varname,"p_offset"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_offset);}
					else if((strcmp(varname,"p_vaddr"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_vaddr);}
					else if((strcmp(varname,"p_paddr"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_paddr);}
					else if((strcmp(varname,"p_filesz"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_filesz);}
					else if((strcmp(varname,"p_memsz"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_memsz);}
					else if((strcmp(varname,"p_align"))==0) {var->type=VART_NUM;var->val.num=get_data32(ph32->p_align);}
					break;
			}
			break;
		case SECT_SH:
			if(num>max_sh()) {printf("out of sh bound.\n"); return NULL;}
			shoffset=elf_get_sh_offs();
			shentsize=elf_get_sh_entsize();
			for(n=0;n<num;n++)
					{
						shoffset=shoffset+shentsize;
					}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					sh64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
					if((strcmp(varname,"sh_name"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh64->sh_name);}
					else if((strcmp(varname,"sh_type"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh64->sh_type);}
					else if((strcmp(varname,"sh_flags"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_flags);}
					else if((strcmp(varname,"sh_addr"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_addr);}
					else if((strcmp(varname,"sh_offset"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_offset);}
					else if((strcmp(varname,"sh_size"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_size);}
					else if((strcmp(varname,"sh_link"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh64->sh_link);}
					else if((strcmp(varname,"sh_info"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh64->sh_info);}
					else if((strcmp(varname,"sh_addralign"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_addralign);}
					else if((strcmp(varname,"sh_entsize"))==0) {var->type=VART_NUM;var->val.num=get_data64(sh64->sh_entsize);}
					break;
				case bit32:
				default:
					sh32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
					if((strcmp(varname,"sh_name"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_name);}
					else if((strcmp(varname,"sh_type"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_type);}
					else if((strcmp(varname,"sh_flags"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_flags);}
					else if((strcmp(varname,"sh_addr"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_addr);}
					else if((strcmp(varname,"sh_offset"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_offset);}
					else if((strcmp(varname,"sh_size"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_size);}
					else if((strcmp(varname,"sh_link"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_link);}
					else if((strcmp(varname,"sh_info"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_info);}
					else if((strcmp(varname,"sh_addralign"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_addralign);}
					else if((strcmp(varname,"sh_entsize"))==0) {var->type=VART_NUM;var->val.num=get_data32(sh32->sh_entsize);}
					break;
			}
			break;
		default:
			
			break;
	}
	return var;
}
off_t elf_get_ph_offs()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data64(elf64->e_phoff);
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data32(elf32->e_phoff);
			break;
	}
	return x;
}
off_t elf_get_sh_offs()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data64(elf64->e_shoff);
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data32(elf32->e_shoff);
			break;
	}
	return x;
}
int elf_get_ph_entsize()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf64->e_phentsize);
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf32->e_phentsize);
			break;
	}
	return x;
}
int elf_get_sh_entsize()
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	int x=0;
	switch(fc_ptr->file_bit_class) {
		case bit64:
			elf64=(Elf64_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf64->e_shentsize);
			break;
		case bit32:
		default:
			elf32=(Elf32_Ehdr*) fc_ptr->faddr;
			x=get_data16(elf32->e_shentsize);
			break;
	}
	return x;
}
void elf_set_s_val(struct _structvar *ptr,struct _var *val)
{
	int sect,num;
	char varname[VARNAME_LEN];
	if(!ptr) return;
	strcpy(varname,"");
	num=ptr->num;
	if((strcmp(ptr->name,"elf"))==0) sect=SECT_ELF;
	if((strcmp(ptr->name,"ph"))==0) sect=SECT_PH;
	if((strcmp(ptr->name,"sh"))==0) sect=SECT_SH;
	if(ptr->next) {strncpy(varname,ptr->next->name,VARNAME_LEN);if(ptr->next->num!=-1) {printf("invalid section.\n");return;}}
	set_elf_var(sect,num,varname,val);
}
void set_elf_var(int sect,int num,char *varname,struct _var *val)
{
	Elf32_Ehdr *elf32;
	Elf64_Ehdr *elf64;
	Elf32_Phdr *ph32;
	Elf64_Phdr *ph64;
	Elf32_Shdr *sh32;
	Elf64_Shdr *sh64;
	int shentsize,n,phentsize;
	off_t shoffset,phoffset;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	if(!val) return;
	switch(sect) {
		case SECT_ELF:
			if(num!=-1) {printf("wrong section.\n"); return;}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					elf64=(Elf64_Ehdr*) fc_ptr->faddr;
					if((strcmp(varname,"e_ident"))==0) {if(val->type==VART_WORD){strncpy((char *)elf64->e_ident,val->val.s,4);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_type"))==0) {if(val->type==VART_NUM){elf64->e_type=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_machine"))==0) {if(val->type==VART_NUM){elf64->e_machine=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_version"))==0) {if(val->type==VART_NUM){elf64->e_version=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_entry"))==0) {if(val->type==VART_NUM){elf64->e_entry=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phoff"))==0) {if(val->type==VART_NUM){elf64->e_phoff=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shoff"))==0) {if(val->type==VART_NUM){elf64->e_shoff=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_flags"))==0) {if(val->type==VART_NUM){elf64->e_flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_ehsize"))==0) {if(val->type==VART_NUM){elf64->e_ehsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phentsize"))==0) {if(val->type==VART_NUM){elf64->e_phentsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phnum"))==0) {if(val->type==VART_NUM){elf64->e_phnum=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shentsize"))==0) {if(val->type==VART_NUM){elf64->e_shentsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shnum"))==0) {if(val->type==VART_NUM){elf64->e_shnum=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shstrndx"))==0) {if(val->type==VART_NUM){elf64->e_shstrndx=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				case bit32:
				default:
					elf32=(Elf32_Ehdr*) fc_ptr->faddr;
					if((strcmp(varname,"e_ident"))==0) {if(val->type==VART_WORD){strncpy((char *)elf32->e_ident,val->val.s,4);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_type"))==0) {if(val->type==VART_NUM){elf32->e_type=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_machine"))==0) {if(val->type==VART_NUM){elf32->e_machine=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_version"))==0) {if(val->type==VART_NUM){elf32->e_version=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_entry"))==0) {if(val->type==VART_NUM){elf32->e_entry=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phoff"))==0) {if(val->type==VART_NUM){elf32->e_phoff=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shoff"))==0) {if(val->type==VART_NUM){elf32->e_shoff=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_flags"))==0) {if(val->type==VART_NUM){elf32->e_flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_ehsize"))==0) {if(val->type==VART_NUM){elf32->e_ehsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phentsize"))==0) {if(val->type==VART_NUM){elf32->e_phentsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_phnum"))==0) {if(val->type==VART_NUM){elf32->e_phnum=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shentsize"))==0) {if(val->type==VART_NUM){elf32->e_shentsize=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shnum"))==0) {if(val->type==VART_NUM){elf32->e_shnum=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"e_shstrndx"))==0) {if(val->type==VART_NUM){elf32->e_shstrndx=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					break;
			}
		case SECT_PH:
			if(num>max_ph()) {printf("out of ph bound.\n"); return;}
			phoffset=elf_get_ph_offs();
			phentsize=elf_get_ph_entsize();
			for(n=0;n<num;n++)
					{
						phoffset=phoffset+phentsize;
					}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					ph64=(Elf64_Phdr*)(fc_ptr->faddr+phoffset);
					if((strcmp(varname,"p_type"))==0) {if(val->type==VART_NUM){ph64->p_type=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_offset"))==0) {if(val->type==VART_NUM){ph64->p_offset=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_vaddr"))==0) {if(val->type==VART_NUM){ph64->p_vaddr=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_paddr"))==0) {if(val->type==VART_NUM){ph64->p_paddr=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_filesz"))==0) {if(val->type==VART_NUM){ph64->p_filesz=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_memsz"))==0) {if(val->type==VART_NUM){ph64->p_memsz=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_flags"))==0) {if(val->type==VART_NUM){ph64->p_flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_align"))==0) {if(val->type==VART_NUM){ph64->p_align=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				case bit32:
				default:
					ph32=(Elf32_Phdr*)(fc_ptr->faddr+phoffset);
					if((strcmp(varname,"p_type"))==0) {if(val->type==VART_NUM){ph32->p_type=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_offset"))==0) {if(val->type==VART_NUM){ph32->p_offset=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_vaddr"))==0) {if(val->type==VART_NUM){ph32->p_vaddr=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_paddr"))==0) {if(val->type==VART_NUM){ph32->p_paddr=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_filesz"))==0) {if(val->type==VART_NUM){ph32->p_filesz=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_memsz"))==0) {if(val->type==VART_NUM){ph32->p_memsz=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_flags"))==0) {if(val->type==VART_NUM){ph32->p_flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"p_align"))==0) {if(val->type==VART_NUM){ph32->p_align=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
			}
			break;
		case SECT_SH:
			if(num>max_sh()) {printf("out of sh bound.\n"); return;}
			shoffset=elf_get_sh_offs();
			shentsize=elf_get_sh_entsize();
			for(n=0;n<num;n++)
					{
						shoffset=shoffset+shentsize;
					}
			switch(fc_ptr->file_bit_class) {
				case bit64:
					sh64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
					if((strcmp(varname,"sh_anme"))==0) {if(val->type==VART_NUM){sh64->sh_name=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_type"))==0) {if(val->type==VART_NUM){sh64->sh_type=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_flags"))==0) {if(val->type==VART_NUM){sh64->sh_flags=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_addr"))==0) {if(val->type==VART_NUM){sh64->sh_addr=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_offset"))==0) {if(val->type==VART_NUM){sh64->sh_offset=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_size"))==0) {if(val->type==VART_NUM){sh64->sh_size=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_link"))==0) {if(val->type==VART_NUM){sh64->sh_link=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_info"))==0) {if(val->type==VART_NUM){sh64->sh_info=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_addralign"))==0) {if(val->type==VART_NUM){sh64->sh_addralign=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_entsize"))==0) {if(val->type==VART_NUM){sh64->sh_entsize=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				case bit32:
				default:
					sh32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
					if((strcmp(varname,"sh_anme"))==0) {if(val->type==VART_NUM){sh32->sh_name=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_type"))==0) {if(val->type==VART_NUM){sh32->sh_type=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_flags"))==0) {if(val->type==VART_NUM){sh32->sh_flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_addr"))==0) {if(val->type==VART_NUM){sh32->sh_addr=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_offset"))==0) {if(val->type==VART_NUM){sh32->sh_offset=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_size"))==0) {if(val->type==VART_NUM){sh32->sh_size=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_link"))==0) {if(val->type==VART_NUM){sh32->sh_link=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_info"))==0) {if(val->type==VART_NUM){sh32->sh_info=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_addralign"))==0) {if(val->type==VART_NUM){sh32->sh_addralign=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sh_entsize"))==0) {if(val->type==VART_NUM){sh32->sh_entsize=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
			}
			break;
		default:
			
			break;
	}
}
off_t get_elf_offset(struct _structvar *ptr,int endoffset)
{
	int sect,num;
	off_t offs;
	if(!ptr) return 0;
	num=ptr->num;
	if((strcmp(ptr->name,"elf"))==0) sect=SECT_ELF;
	if((strcmp(ptr->name,"ph"))==0) sect=SECT_PH;
	if((strcmp(ptr->name,"sh"))==0) sect=SECT_SH;
	if(ptr->next) {printf("invalid section.\n"); return 0;}
	switch (sect) {
		case SECT_ELF:
			if(ptr->num!=-1) {printf("invalid section.\n"); return 0;}
			break;
		case SECT_PH:
		case SECT_SH:
			if(ptr->num==-1) {printf("invalid section.\n"); return 0;}
			break;
		default:
			return 0;
			break;
	}
	offs=get_elf_offset2(sect,num,endoffset);
	return offs;
}
off_t get_elf_offset2(int sect,int num,int endoffset)
{
	//Elf32_Ehdr *elf32;
	//Elf64_Ehdr *elf64;
	//Elf32_Phdr *ph32;
	//Elf64_Phdr *ph64;
	//Elf32_Shdr *sh32;
	//Elf64_Shdr *sh64;
	int shentsize,n,phentsize;
	off_t shoffset,phoffset,offs;
	if(!fc_ptr) {printf("no file opened.\n"); return 0;}
	offs=0;
	switch(sect) {
		case SECT_ELF:
			if(num!=-1) {printf("wrong section.\n"); return 0;}
			offs=0;
			if(endoffset) {
			switch(fc_ptr->file_bit_class) {
				case bit64:
					//elf64=(Elf64_Ehdr*) fc_ptr->faddr;
					offs=sizeof(Elf64_Ehdr);
					break;
				case bit32:
				default:
					//elf32=(Elf32_Ehdr*) fc_ptr->faddr;
					offs=sizeof(Elf32_Ehdr);
					break;
			}
			}
			break;
		case SECT_PH:
			if(num>max_ph()) {printf("out of ph bound.\n"); return 0;}
			phoffset=elf_get_ph_offs();
			phentsize=elf_get_ph_entsize();
			for(n=0;n<num;n++)
					{
						phoffset=phoffset+phentsize;
					}
					offs=phoffset;
			if(endoffset) {
			switch(fc_ptr->file_bit_class) {
				case bit64:
					//ph64=(Elf64_Phdr*)(fc_ptr->faddr+phoffset);
					offs=offs+sizeof(Elf64_Phdr);
					break;
				case bit32:
				default:
					//ph32=(Elf32_Phdr*)(fc_ptr->faddr+phoffset);
					offs=offs+sizeof(Elf32_Phdr);
					break;
			}
			}
			break;
		case SECT_SH:
			if(num>max_sh()) {printf("out of sh bound.\n"); return 0;}
			shoffset=elf_get_sh_offs();
			shentsize=elf_get_sh_entsize();
			for(n=0;n<num;n++)
					{
						shoffset=shoffset+shentsize;
					}
					offs=shoffset;
			if(endoffset) {
			switch(fc_ptr->file_bit_class) {
				case bit64:
					//sh64=(Elf64_Shdr*)(fc_ptr->faddr+shoffset);
					offs=offs+sizeof(Elf64_Shdr);
					break;
				case bit32:
				default:
					//sh32=(Elf32_Shdr*)(fc_ptr->faddr+shoffset);
					offs=offs+sizeof(Elf32_Shdr);
					break;
			}
			}
			break;
		default:
			offs=0;
			break;
	}
	return offs;
}
void create_elf_hdr(int sect,off_t offs,int grow,int update)
{
	if(!fc_ptr) return;
	switch(sect) {
		case SECT_ELF:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(Elf64_Ehdr),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(Elf32_Ehdr),grow);
					break;
			}
			break;
		case SECT_PH:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(Elf64_Phdr),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(Elf32_Phdr),grow);
					break;
			}
			if(update)
			{
				printf("update....still coding.\n");
			}
			break;
		case SECT_SH:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(Elf64_Shdr),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(Elf32_Shdr),grow);
					break;
			}
			if(update)
			{
				printf("update....still coding\n");
			}
			break;
		default:
			
			break;
	}
}
