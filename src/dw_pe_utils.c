/*
 dw_pe_utils.c : PE file format utils.

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

void load_pe_hd()
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_SECTION_HEADER *sec;
	int x,m_pe_off,m_opt_header_size,m_num_sec,n;
	char path[255];
	load_mz_hd();
	make_table(NULL,"pe",-1);
	mz=(_IMAGE_DOS_HEADER*)faddr;
	m_pe_off=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(faddr+m_pe_off);
	x=get_data32(pe->Signature);
	add_s_var("pe","Signature",TYPE_VAL,&x);
	make_table("pe","FileHeader",-1);
	make_table("pe","OptionalHeader",-1);
	x=get_data16(pe->FileHeader.Machine);
	add_s_var("pe->FileHeader","Machine",TYPE_VAL,&x);
	m_num_sec=get_data16(pe->FileHeader.NumberOfSections);
	add_s_var("pe->FileHeader","NumberOfSections",TYPE_VAL,&m_num_sec);
	x=get_data32(pe->FileHeader.TimeDateStamp);
	add_s_var("pe->FileHeader","TimeDateStamp",TYPE_VAL,&x);
	x=get_data32(pe->FileHeader.PointerToSymbolTable);
	add_s_var("pe->FileHeader","PointerToSymbolTable",TYPE_VAL,&x);
	x=get_data32(pe->FileHeader.NumberOfSymbols);
	add_s_var("pe->FileHeader","NumberOfSymbols",TYPE_VAL,&x);
	m_opt_header_size=get_data16(pe->FileHeader.SizeOfOptionalHeader);
	add_s_var("pe->FileHeader","SizeOfOptionalHeader",TYPE_VAL,&m_opt_header_size);
	x=get_data16(pe->FileHeader.Characteristics);
	add_s_var("pe->FileHeader","Characteristics",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.Magic);
	add_s_var("pe->OptionalHeader","Magic",TYPE_VAL,&x);
	x=get_data8(pe->OptionalHeader.MajorLinkerVersion);
	add_s_var("pe->OptionalHeader","MajorLinkerVersion",TYPE_VAL,&x);
	x=get_data8(pe->OptionalHeader.MinorLinkerVersion);
	add_s_var("pe->OptionalHeader","MinorLinkerVersion",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfCode);
	add_s_var("pe->OptionalHeader","SizeOfCode",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfInitializedData);
	add_s_var("pe->OptionalHeader","SizeOfInitializedData",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfUninitializedData);
	add_s_var("pe->OptionalHeader","SizeOfUninitializedData",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.AddressOfEntryPoint);
	add_s_var("pe->OptionalHeader","AddressOfEntryPoint",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.BaseOfCode);
	add_s_var("pe->OptionalHeader","BaseOfCode",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.BaseOfData);
	add_s_var("pe->OptionalHeader","BaseOfData",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.ImageBase);
	add_s_var("pe->OptionalHeader","ImageBase",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SectionAlignment);
	add_s_var("pe->OptionalHeader","SectionAlignment",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.FileAlignment);
	add_s_var("pe->OptionalHeader","FileAlignment",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MajorOperatingSystemVersion);
	add_s_var("pe->OptionalHeader","MajorOperatingSystemVersion",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MinorOperatingSystemVersion);
	add_s_var("pe->OptionalHeader","MinorOperatingSystemVersion",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MajorImageVersion);
	add_s_var("pe->OptionalHeader","MajorImageVersion",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MinorImageVersion);
	add_s_var("pe->OptionalHeader","MinorImageVersion",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MajorSubsystemVersion);
	add_s_var("pe->OptionalHeader","MajorSubsystemVersion",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.MinorSubsystemVersion);
	add_s_var("pe->OptionalHeader","MinorSubsystemVersion",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.Reserved1);
	add_s_var("pe->OptionalHeader","Reserved1",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfImage);
	add_s_var("pe->OptionalHeader","SizeOfImage",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfHeaders);
	add_s_var("pe->OptionalHeader","SizeOfHeaders",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.CheckSum);
	add_s_var("pe->OptionalHeader","CheckSum",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.Subsystem);
	add_s_var("pe->OptionalHeader","Subsystem",TYPE_VAL,&x);
	x=get_data16(pe->OptionalHeader.DllCharacteristics);
	add_s_var("pe->OptionalHeader","DllCharacteristics",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfStackReserve);
	add_s_var("pe->OptionalHeader","SizeOfStackReserve",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfStackCommit);
	add_s_var("pe->OptionalHeader","SizeOfStackCommit",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfHeapReserve);
	add_s_var("pe->OptionalHeader","SizeOfHeapReserve",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.SizeOfHeapCommit);
	add_s_var("pe->OptionalHeader","SizeOfHeapCommit",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.LoaderFlags);
	add_s_var("pe->OptionalHeader","LoaderFlags",TYPE_VAL,&x);
	x=get_data32(pe->OptionalHeader.NumberOfRvaAndSizes);
	add_s_var("pe->OptionalHeader","NumberOfRvaAndSizes",TYPE_VAL,&x);
	if(m_num_sec>0)
	{
		make_table(NULL,"s",m_num_sec);
		n=0;
		sec=(_IMAGE_SECTION_HEADER*) (faddr+m_pe_off+m_opt_header_size+sizeof(_IMAGE_FILE_HEADER)+sizeof(IMAGE_NT_SIGNATURE));
		do{
			sprintf(path,"s[%d]",n);
			add_s_var(path,"Name",TYPE_STRING,&sec->Name);
			x=get_data32(sec->Misc.PhysicalAddress);
			add_s_var(path,"PhysicalAddress",TYPE_VAL,&x);
			x=get_data32(sec->VirtualAddress);
			add_s_var(path,"VirtualAddress",TYPE_VAL,&x);
			x=get_data32(sec->SizeOfRawData);
			add_s_var(path,"SizeOfRawData",TYPE_VAL,&x);
			x=get_data32(sec->PointerToRawData);
			add_s_var(path,"PointerToRawData",TYPE_VAL,&x);
			x=get_data32(sec->PointerToRelocations);
			add_s_var(path,"PointerToRelocations",TYPE_VAL,&x);
			x=get_data32(sec->PointerToLinenumbers);
			add_s_var(path,"PointerToLinenumbers",TYPE_VAL,&x);
			x=get_data16(sec->NumberOfRelocations);
			add_s_var(path,"NumberOfRelocations",TYPE_VAL,&x);
			x=get_data16(sec->NumberOfLinenumbers);
			add_s_var(path,"NumberOfLinenumbers",TYPE_VAL,&x);
			x=get_data32(sec->Characteristics);
			add_s_var(path,"Characteristics",TYPE_VAL,&x);
			n++;
			sec++;
		}while(n<m_num_sec);
	}
	//printf("PE\n");
}
void load_mz_hd()
{
    _IMAGE_DOS_HEADER *mz;
    int x;
    make_table(NULL,"mz",-1);
    mz=(_IMAGE_DOS_HEADER*)faddr;
	x=get_data16(mz->e_magic);
	add_s_var("mz","e_magic",TYPE_VAL,&x);
    x=get_data16(mz->e_cblp);
    add_s_var("mz","e_cblp",TYPE_VAL,&x);
    x=get_data16(mz->e_cp);
    add_s_var("mz","e_cp",TYPE_VAL,&x);
    x=get_data16(mz->e_crlc);
	add_s_var("mz","e_crlc",TYPE_VAL,&x);
    x=get_data16(mz->e_cparhdr);
	add_s_var("mz","e_cparhdr",TYPE_VAL,&x);
    x=get_data16(mz->e_minalloc);
    add_s_var("mz","e_minalloc",TYPE_VAL,&x);
    x=get_data16(mz->e_maxalloc);
	add_s_var("mz","e_maxalloc",TYPE_VAL,&x);
    x=get_data16(mz->e_ss);
	add_s_var("mz","e_ss",TYPE_VAL,&x);
    x=get_data16(mz->e_sp);
	add_s_var("mz","e_sp",TYPE_VAL,&x);
    x=get_data16(mz->e_csum);
	add_s_var("mz","e_csum",TYPE_VAL,&x);
    x=get_data16(mz->e_ip);
	add_s_var("mz","e_ip",TYPE_VAL,&x);
    x=get_data16(mz->e_cs);
	add_s_var("mz","e_cs",TYPE_VAL,&x);
    x=get_data16(mz->e_lfarlc);
	add_s_var("mz","e_lfarlc",TYPE_VAL,&x);
    x=get_data16(mz->e_ovno);
	add_s_var("mz","e_ovno",TYPE_VAL,&x);
//    x=get_data16(mz->e_res[4]);                    // Reserved words
//	add_s_var("mz","",TYPE_VAL,&x);
    x=get_data16(mz->e_oemid);
    add_s_var("mz","e_oemid",TYPE_VAL,&x);
    x=get_data16(mz->e_oeminfo);
	add_s_var("mz","e_oeminfo",TYPE_VAL,&x);
//    x=get_data16(mz->e_res2[10]);                  // Reserved words
//	add_s_var("mz","",TYPE_VAL,&x);
    x=get_data32(mz->e_lfanew);
    add_s_var("mz","e_lfanew",TYPE_VAL,&x);
}
int get_offset_pe(char *s,char p)
{
	off_t offset;
	char *str;
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_SECTION_HEADER *sec;
	int x,m_pe_off,m_opt_header_size,m_num_sec,n;
	struct token tok;
	offset=get_offset_mz(s,p);
	str=s;
	str=get_token(&tok,str);
	mz=(_IMAGE_DOS_HEADER*)faddr;
	m_pe_off=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(faddr+m_pe_off);
	m_opt_header_size=get_data16(pe->FileHeader.SizeOfOptionalHeader);
	m_num_sec=get_data16(pe->FileHeader.NumberOfSections);
	if((strncmp(tok.name,"pe",2))==0)
	{
		offset=(off_t)m_pe_off;
		if(p=='e') offset+=sizeof(_IMAGE_NT_HEADERS);
	}
	if((strncmp(tok.name,"s",1))==0)
	{
		sec=(_IMAGE_SECTION_HEADER*) (m_pe_off+m_opt_header_size+sizeof(_IMAGE_FILE_HEADER)+sizeof(IMAGE_NT_SIGNATURE));
		n=0;
		if(tok.num>=m_num_sec) tok.num=m_num_sec-1;
		while(n++<tok.num){sec++;};
		offset=(off_t)sec;
		if(p=='e') offset+=sizeof(_IMAGE_SECTION_HEADER);
	}
	return offset;
}
off_t get_offset_mz(char *s,char p)
{
	off_t offset;
	char *str;
	int x,m_pe_off,m_opt_header_size,m_num_sec,n;
	struct token tok;
	str=s;
	str=get_token(&tok,str);
	if((strncmp(s,"mz",2))==0)
	{
		offset=0;
		if(p=='e') offset+=sizeof(_IMAGE_DOS_HEADER);
		return offset;
	}
}
void save_mz_hd(void)
{
	_IMAGE_DOS_HEADER *mz;
	struct _var *var;
	struct _p *p;
	mz=(_IMAGE_DOS_HEADER*)faddr;
	
	p=quickparse("mz");
	var=get_s_var_bypointer(p,"e_magic");
	mz->e_magic=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_cblp");
	mz->e_cblp=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_cp");
	mz->e_cp=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_crlc");
	mz->e_crlc=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_cparhdr");
	mz->e_cparhdr=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_minalloc");
	mz->e_minalloc=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_maxalloc");
	mz->e_maxalloc=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_ss");
	mz->e_ss=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_sp");
	mz->e_sp=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_csum");
	mz->e_csum=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_ip");
	mz->e_ip=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_cs");
	mz->e_cs=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_lfarlc");
	mz->e_lfarlc=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_ovno");
	mz->e_ovno=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_oemid");
	mz->e_oemid=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_oeminfo");
	mz->e_oeminfo=get_data16(var->val);
	var=get_s_var_bypointer(p,"e_lfanew");
	mz->e_lfanew=get_data32(var->val);
}
void save_pe_hd(void)
{
	struct _var *var;
	struct _p *p;
	off_t offset;
	char *str,path[255];
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_SECTION_HEADER *sec;
	int x,m_pe_off,m_opt_header_size,m_num_sec,n;
	struct token tok;
	mz=(_IMAGE_DOS_HEADER*)faddr;
	save_mz_hd();
	m_pe_off=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(faddr+m_pe_off);
	p=quickparse("pe->FileHeader");
	var=get_s_var_bypointer(p,"Machine");
	pe->FileHeader.Machine=get_data16(var->val);
	var=get_s_var_bypointer(p,"NumberOfSections");
	pe->FileHeader.NumberOfSections=get_data16(var->val);
	var=get_s_var_bypointer(p,"TimeDateStamp");
	pe->FileHeader.TimeDateStamp=get_data32(var->val);
	var=get_s_var_bypointer(p,"PointerToSymbolTable");
	pe->FileHeader.PointerToSymbolTable=get_data32(var->val);
	var=get_s_var_bypointer(p,"NumberOfSymbols");
	pe->FileHeader.NumberOfSymbols=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfOptionalHeader");
	pe->FileHeader.SizeOfOptionalHeader=get_data16(var->val);
	var=get_s_var_bypointer(p,"Characteristics");
	pe->FileHeader.Characteristics=get_data16(var->val);
	p=quickparse("pe->OptionalHeader");
	var=get_s_var_bypointer(p,"Magic");
	pe->OptionalHeader.Magic=get_data16(var->val);
	var=get_s_var_bypointer(p,"MajorLinkerVersion");
	pe->OptionalHeader.MajorLinkerVersion=get_data8(var->val);
	var=get_s_var_bypointer(p,"MinorLinkerVersion");
	pe->OptionalHeader.MinorLinkerVersion=get_data8(var->val);
	var=get_s_var_bypointer(p,"SizeOfCode");
	pe->OptionalHeader.SizeOfCode=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfInitializedData");
	pe->OptionalHeader.SizeOfInitializedData=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfUninitializedData");
	pe->OptionalHeader.SizeOfUninitializedData=get_data32(var->val);
	var=get_s_var_bypointer(p,"AddressOfEntryPoint");
	pe->OptionalHeader.AddressOfEntryPoint=get_data32(var->val);
	var=get_s_var_bypointer(p,"BaseOfCode");
	pe->OptionalHeader.BaseOfCode=get_data32(var->val);
	var=get_s_var_bypointer(p,"BaseOfData");
	pe->OptionalHeader.BaseOfData=get_data32(var->val);
	var=get_s_var_bypointer(p,"ImageBase");
	pe->OptionalHeader.ImageBase=get_data32(var->val);
	var=get_s_var_bypointer(p,"SectionAlignment");
	pe->OptionalHeader.SectionAlignment=get_data32(var->val);
	var=get_s_var_bypointer(p,"FileAlignment");
	pe->OptionalHeader.FileAlignment=get_data32(var->val);
	var=get_s_var_bypointer(p,"MajorOperatingSystemVersion");
	pe->OptionalHeader.MajorOperatingSystemVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"MinorOperatingSystemVersion");
	pe->OptionalHeader.MinorOperatingSystemVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"MajorImageVersion");
	pe->OptionalHeader.MajorImageVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"MinorImageVersion");
	pe->OptionalHeader.MinorImageVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"MajorSubsystemVersion");
	pe->OptionalHeader.MajorSubsystemVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"MinorSubsystemVersion");
	pe->OptionalHeader.MinorSubsystemVersion=get_data16(var->val);
	var=get_s_var_bypointer(p,"Reserved1");
	pe->OptionalHeader.Reserved1=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfImage");
	pe->OptionalHeader.SizeOfImage=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfHeaders");
	pe->OptionalHeader.SizeOfHeaders=get_data32(var->val);
	var=get_s_var_bypointer(p,"CheckSum");
	pe->OptionalHeader.CheckSum=get_data32(var->val);
	var=get_s_var_bypointer(p,"Subsystem");
	pe->OptionalHeader.Subsystem=get_data16(var->val);
	var=get_s_var_bypointer(p,"DllCharacteristics");
	pe->OptionalHeader.DllCharacteristics=get_data16(var->val);
	var=get_s_var_bypointer(p,"SizeOfStackReserve");
	pe->OptionalHeader.SizeOfStackReserve=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfStackCommit");
	pe->OptionalHeader.SizeOfStackCommit=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfHeapReserve");
	pe->OptionalHeader.SizeOfHeapReserve=get_data32(var->val);
	var=get_s_var_bypointer(p,"SizeOfHeapCommit");
	pe->OptionalHeader.SizeOfHeapCommit=get_data32(var->val);
	var=get_s_var_bypointer(p,"LoaderFlags");
	pe->OptionalHeader.LoaderFlags=get_data32(var->val);
	var=get_s_var_bypointer(p,"NumberOfRvaAndSizes");
	pe->OptionalHeader.NumberOfRvaAndSizes=get_data32(var->val);
	m_opt_header_size=get_data16(pe->FileHeader.SizeOfOptionalHeader);
	m_num_sec=get_data16(pe->FileHeader.NumberOfSections);
	if(m_num_sec>0)
	{
		n=0;
		sec=(_IMAGE_SECTION_HEADER*) (faddr+m_pe_off+m_opt_header_size+sizeof(_IMAGE_FILE_HEADER)+sizeof(IMAGE_NT_SIGNATURE));
		do{
			sprintf(path,"s[%d]",n);
			p=quickparse(path);
			var=get_s_var_bypointer(p,"Name");
			strncpy(sec->Name,var->s,8);
			var=get_s_var_bypointer(p,"PhysicalAddress");
			sec->Misc.PhysicalAddress=get_data32(var->val);
			var=get_s_var_bypointer(p,"VirtualAddress");
			sec->VirtualAddress=get_data32(var->val);
			var=get_s_var_bypointer(p,"SizeOfRawData");
			sec->SizeOfRawData=get_data32(var->val);
			var=get_s_var_bypointer(p,"PointerToRawData");
			sec->PointerToRawData=get_data32(var->val);
			var=get_s_var_bypointer(p,"PointerToRelocations");
			sec->PointerToRelocations=get_data32(var->val);
			var=get_s_var_bypointer(p,"PointerToLinenumbers");
			sec->PointerToLinenumbers=get_data32(var->val);
			var=get_s_var_bypointer(p,"NumberOfRelocations");
			sec->NumberOfRelocations=get_data32(var->val);
			var=get_s_var_bypointer(p,"NumberOfLinenumbers");
			sec->NumberOfLinenumbers=get_data32(var->val);
			var=get_s_var_bypointer(p,"Characteristics");
			sec->Characteristics=get_data32(var->val);
			n++;
			sec++;
		}while(n<m_num_sec);
	}
}
int get_max_pe_sect()
{
	struct _gv *ptr;
	struct _var *vv;
	int count=0;
	vv=get_s_var("s");
	for(ptr=vv->p.first;ptr;ptr=ptr->next) count ++;
	return count-1;
}
void populate_new_pe_sect(int sec_pos)
{
	int x=0;
	char path[MAX_STR],*str="new";
	sprintf(path,"s[%d]",sec_pos);
	add_s_var(path,"Name",TYPE_STRING,str);
	add_s_var(path,"PhysicalAddress",TYPE_VAL,&x);
	add_s_var(path,"VirtualAddress",TYPE_VAL,&x);
	add_s_var(path,"SizeOfRawData",TYPE_VAL,&x);
	add_s_var(path,"PointerToRawData",TYPE_VAL,&x);
	add_s_var(path,"PointerToRelocations",TYPE_VAL,&x);
	add_s_var(path,"PointerToLinenumbers",TYPE_VAL,&x);
	add_s_var(path,"NumberOfRelocations",TYPE_VAL,&x);
	add_s_var(path,"NumberOfLinenumbers",TYPE_VAL,&x);
	add_s_var(path,"Characteristics",TYPE_VAL,&x);
}
void add_section_pe(int num)
{
	struct _gv *x,*ptr;
	struct _var *vv;
	int count=0,i,sec_pos;
	char path[MAX_STR],tempstr[MAX_STR];
	vv=get_s_var("s");
	x=(struct _gv*)malloc(sizeof(struct _gv));
	if(x==NULL) die("error allocating child table memory");
	x->v.name=(char*)malloc(5);
	if(x->v.name==NULL) die("error allocating child table memory");
	strcpy(x->v.name,"s");
	x->v.type=TYPE_STRUCT;
	x->prev=NULL;
	x->next=NULL;
	x->v.p.first=NULL;
	x->v.p.last=NULL;
	sprintf(tempstr,"s[%d]",get_max_pe_sect()+1);
	add_completion(NULL,tempstr,comp_discardable);
	if(num==-1)
	{
		vv->p.last->next=x;
		x->prev=vv->p.last;
		vv->p.last=x; // now max_sh in already incremented !!
		sec_pos=get_max_pe_sect();
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
	populate_new_pe_sect(sec_pos);
}
