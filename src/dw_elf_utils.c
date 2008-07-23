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
	int x,num_ph,num_sh,m_e_phnum,m_e_shnum,m_e_phoff,m_e_shoff;
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
	m_e_phoff=get_data32(elf->e_phoff);
	add_s_var("s","e_phoff",TYPE_VAL,&m_e_phoff);
	m_e_shoff=get_data32(elf->e_shoff);
	add_s_var("s","e_shoff",TYPE_VAL,&m_e_shoff);
	x=get_data32(elf->e_flags);
	add_s_var("s","e_flags",TYPE_VAL,&x);
	x=get_data16(elf->e_ehsize);
	add_s_var("s","e_ehsize",TYPE_VAL,&x);
	x=get_data16(elf->e_phentsize);
	add_s_var("s","e_phentsize",TYPE_VAL,&x);
	m_e_phnum=get_data16(elf->e_phnum);
	add_s_var("s","e_phnum",TYPE_VAL,&m_e_phnum);
	x=get_data16(elf->e_shentsize);
	add_s_var("s","e_shentsize",TYPE_VAL,&x);
	m_e_shnum=get_data16(elf->e_shnum);
	add_s_var("s","e_shnum",TYPE_VAL,&m_e_shnum);
	x=get_data16(elf->e_shstrndx);
	add_s_var("s","e_shstrndx",TYPE_VAL,&x);
	if(m_e_phnum>0)
	{
		make_table(NULL,"ph",m_e_phnum);
		num_ph=0;
		ph=(Elf32_Phdr*)(faddr+m_e_phoff);
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
		}while(num_ph<m_e_phnum);
	}
	if(m_e_shnum>0)
	{
		make_table(NULL,"sh",m_e_shnum);
		num_sh=0;
		sh=(Elf32_Shdr*)(faddr+m_e_shoff);
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
		}while(num_sh<m_e_shnum);
	}
	//make_table(NULL,"ph",2);
	//add_s_var("ph[1]","pippo",TYPE_VAL,&xx);
	//make_table("ph[2]","ptable",4);
	//add_s_var("ph[2]->ptable[1]","pippo",TYPE_VAL,&xx);
	//printf("elf\n");
}
int get_offset_elf(char *s,char p)
{
	off_t offset;
	char *str;
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph;
	Elf32_Shdr *sh;
	int x,num_ph,num_sh,m_e_phnum,m_e_shnum,m_e_phoff,m_e_shoff;
	char path[255];
	elf=(Elf32_Ehdr*)faddr;
	m_e_phoff=get_data32(elf->e_phoff);
	m_e_shoff=get_data32(elf->e_shoff);
	m_e_phnum=get_data16(elf->e_phnum);
	m_e_shnum=get_data16(elf->e_shnum);
	struct token tok;
	str=s;
	str=get_token(&tok,str);
	if((strcmp(s,"s"))==0)
	{
		offset=0;
		if(p=='e') offset+=sizeof(Elf32_Ehdr);
		return offset;
	}
	if((strncmp(tok.name,"ph",2))==0)
	{
		ph=(Elf32_Phdr*)(faddr+m_e_phoff);
		x=0;
		if(tok.num>=m_e_phnum) tok.num=m_e_phnum-1;
		while(x++<tok.num){ph++;};
		offset=(off_t)ph;
		if(p=='e') offset+=sizeof(Elf32_Phdr);
		return offset;
	}
	if((strncmp(tok.name,"sh",2))==0)
	{
		sh=(Elf32_Phdr*)(faddr+m_e_shoff);
		x=0;
		if(tok.num>=m_e_shnum) tok.num=m_e_shnum-1;
		while(x++<tok.num){sh++;};
		offset=(off_t)sh;
		if(p=='e') offset+=sizeof(Elf32_Shdr);
		return offset;
	}
	return 0;
}
void save_elf_hd()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph;
	Elf32_Shdr *sh;
	int x,num_ph,num_sh,m_e_phnum,m_e_shnum,m_e_phoff,m_e_shoff;
	char path[255];
	struct _p *p;
	struct _var *var;
	elf=(Elf32_Ehdr*)faddr;
	//have to insert save ident here
	p=quickparse("s");
	var=get_s_var_bypointer(p,"e_type");
	elf->e_type=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_machine");
	elf->e_machine=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_version");
	elf->e_version=get_data32(var->val);
	var=get_s_var_bypointer(p,"e_entry");
	elf->e_entry=get_data32(var->val);
	var=get_s_var_bypointer(p,"e_phoff");
	m_e_phoff=var->val;
	elf->e_phoff=get_data32(var->val);
	var=get_s_var_bypointer(p,"e_shoff");
	m_e_shoff=var->val;
	elf->e_shoff=get_data32(var->val);
	var=get_s_var_bypointer(p,"e_flags");
	elf->e_flags=get_data32(var->val);
	var=get_s_var_bypointer(p,"e_ehsize");
	elf->e_ehsize=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_phentsize");
	elf->e_phentsize=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_phnum");
	m_e_phnum=var->val;
	elf->e_phnum=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_shentsize");
	elf->e_shentsize=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_shnum");
	m_e_shnum=var->val;
	elf->e_shnum=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_shstrndx");
	elf->e_shstrndx=get_data16(var->val);
	if(m_e_phnum>0)
	{
		num_ph=0;
		ph=(Elf32_Phdr*)(faddr+m_e_phoff);
		do
		{
			sprintf(path,"ph[%d]",num_ph);
			p=quickparse(path);
			var=get_s_var_bypointer(p,"p_type");
			ph->p_type=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_offset");
			ph->p_offset=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_vaddr");
			ph->p_vaddr=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_paddr");
			ph->p_paddr=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_filesz");
			ph->p_filesz=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_memsz");
			ph->p_memsz=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_flags");
			ph->p_flags=get_data32(var->val);
			var=get_s_var_bypointer(p,"p_align");
			ph->p_align=get_data32(var->val);
			ph=(Elf32_Phdr*)((char*)ph+sizeof(Elf32_Phdr));
			num_ph++;
		}while(num_ph<m_e_phnum);
	}
	if(m_e_shnum>0)
	{
		num_sh=0;
		sh=(Elf32_Shdr*)(faddr+m_e_shoff);
		do
		{
			sprintf(path,"sh[%d]",num_sh);
			p=quickparse(path);
			var=get_s_var_bypointer(p,"sh_name");
			sh->sh_name=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_type");
			sh->sh_type=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_flags");
			sh->sh_flags=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_addr");
			sh->sh_addr=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_offset");
			sh->sh_offset=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_size");
			sh->sh_size=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_link");
			sh->sh_link=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_info");
			sh->sh_info=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_addralign");
			sh->sh_addralign=get_data32(var->val);
			var=get_s_var_bypointer(p,"sh_entsize");
			sh->sh_entsize=get_data32(var->val);
			sh=(Elf32_Shdr*)((char*)sh+sizeof(Elf32_Shdr));
			num_sh++;
		}while(num_sh<m_e_shnum);
	}
}
int get_max_ph()
{
	struct _gv *ptr;
	struct _var *vv;
	int count=0;
	vv=get_s_var("ph");
	for(ptr=vv->p.first;ptr;ptr=ptr->next) count ++;
	return count-1;
}
int get_max_sh()
{
	struct _gv *ptr;
	struct _var *vv;
	int count=0;
	vv=get_s_var("sh");
	for(ptr=vv->p.first;ptr;ptr=ptr->next) count ++;
	return count-1;
}
void populate_new_ph(int sec_pos)
{
	int x=0;
	char path[MAX_STR];
	sprintf(path,"ph[%d]",sec_pos);
	add_s_var(path,"p_type",TYPE_VAL,&x);
	add_s_var(path,"p_offset",TYPE_VAL,&x);
	add_s_var(path,"p_vaddr",TYPE_VAL,&x);
	add_s_var(path,"p_paddr",TYPE_VAL,&x);
	add_s_var(path,"p_filesz",TYPE_VAL,&x);
	add_s_var(path,"p_memsz",TYPE_VAL,&x);
	add_s_var(path,"p_flags",TYPE_VAL,&x);
	add_s_var(path,"p_align",TYPE_VAL,&x);
}
void populate_new_sh(int sec_pos)
{
	int x=0;
	char path[MAX_STR];
	sprintf(path,"sh[%d]",sec_pos);
	add_s_var(path,"sh_name",TYPE_VAL,&x);
	add_s_var(path,"sh_type",TYPE_VAL,&x);
	add_s_var(path,"sh_flags",TYPE_VAL,&x);
	add_s_var(path,"sh_addr",TYPE_VAL,&x);
	add_s_var(path,"sh_offset",TYPE_VAL,&x);
	add_s_var(path,"sh_size",TYPE_VAL,&x);
	add_s_var(path,"sh_link",TYPE_VAL,&x);
	add_s_var(path,"sh_info",TYPE_VAL,&x);
	add_s_var(path,"sh_addralign",TYPE_VAL,&x);
	add_s_var(path,"sh_entsize",TYPE_VAL,&x);
}
void add_section_ph(int num)
{
	struct _gv *x,*ptr;
	struct _var *vv;
	int count=0,i,sec_pos;
	char path[MAX_STR],tempstr[MAX_STR];
	vv=get_s_var("ph");
	x=(struct _gv*)malloc(sizeof(struct _gv));
	if(x==NULL) die("error allocating child table memory");
	x->v.name=(char*)malloc(5);
	if(x->v.name==NULL) die("error allocating child table memory");
	strcpy(x->v.name,"ph");
	x->v.type=TYPE_STRUCT;
	x->prev=NULL;
	x->next=NULL;
	x->v.p.first=NULL;
	x->v.p.last=NULL;
	sprintf(tempstr,"ph[%d]",get_max_ph()+1);
	add_completion(NULL,tempstr,comp_discardable);
	if(num==-1)
	{
		vv->p.last->next=x;
		x->prev=vv->p.last;
		vv->p.last=x; // now max_ph in already incremented !!
		sec_pos=get_max_ph();
	}
	else
	{
		sec_pos=num;
		ptr=vv->p.first;
		for(i=0;i<num;i++) if(ptr->next) ptr=ptr->next;
		if(ptr->prev==NULL)
		{
			vv->p.first->prev=x;
			x->next=vv->p.first;
			vv->p.first=x;
		}
		else
		{
			x->prev=ptr->prev;
			x->next=ptr;
			ptr->prev->next=x;
			ptr->prev=x;
		}
	}
	populate_new_ph(sec_pos);
}
void add_section_sh(int num)
{
	struct _gv *x,*ptr;
	struct _var *vv;
	int count=0,i,sec_pos;
	char path[MAX_STR],tempstr[MAX_STR];
	vv=get_s_var("sh");
	x=(struct _gv*)malloc(sizeof(struct _gv));
	if(x==NULL) die("error allocating child table memory");
	x->v.name=(char*)malloc(5);
	if(x->v.name==NULL) die("error allocating child table memory");
	strcpy(x->v.name,"sh");
	x->v.type=TYPE_STRUCT;
	x->prev=NULL;
	x->next=NULL;
	x->v.p.first=NULL;
	x->v.p.last=NULL;
	sprintf(tempstr,"sh[%d]",get_max_sh()+1);
	add_completion(NULL,tempstr,comp_discardable);
	if(num==-1)
	{
		vv->p.last->next=x;
		x->prev=vv->p.last;
		vv->p.last=x; // now max_sh in already incremented !!
		sec_pos=get_max_sh();
	}
	else
	{
		sec_pos=num;
		ptr=vv->p.first;
		for(i=0;i<num;i++) if(ptr->next) ptr=ptr->next;
		if(ptr->prev==NULL)
		{
			vv->p.first->prev=x;
			x->next=vv->p.first;
			vv->p.first=x;
		}
		else
		{
			x->prev=ptr->prev;
			x->next=ptr;
			ptr->prev->next=x;
			ptr->prev=x;
		}
	}
	populate_new_sh(sec_pos);
}
