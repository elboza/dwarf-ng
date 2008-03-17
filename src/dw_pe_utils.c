/*
 dw_pe_utils.c : PE file format utils.

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
#include"utils.h"
#include"lang.h"
#include"vars.h"


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
off_t get_offset_pe(char *s,char p)
{
	off_t offset;
	struct token tok;
	offset=get_offset_mz(s,p);
	return offset;
}
off_t get_offset_mz(char *s,char p)
{
	off_t offset;
	if((strncmp(s,"mz",2))==0)
	{
		offset=0;
		if(p=='e') offset+=sizeof(_IMAGE_DOS_HEADER);
		return offset;
	}
}
