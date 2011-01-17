/*
 sh_pe_utils.c : dwarf's shell pe utils functions file.

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
#include "../libdwarf/libdwarf.h"
#include"dw_readline_completion.h"
#include"sh_pe_utils.h"
#include"output.h"
#include"../../config.h"

void print_mz_hdr(void)
{
	_IMAGE_DOS_HEADER *mz;
	char ss[30];
	if(!fc_ptr) return;
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	printf("e_magic: 0x%x (%d)\n",get_data16(mz->e_magic),get_data16(mz->e_magic));
	printf("e_cblp: 0x%x (%d)\n",get_data16(mz->e_cblp),get_data16(mz->e_cblp));
	printf("e_cp: 0x%x (%d)\n",get_data16(mz->e_cp),get_data16(mz->e_cp));
	printf("e_crlc: 0x%x (%d)\n",get_data16(mz->e_crlc),get_data16(mz->e_crlc));
	printf("e_cparhdr: 0x%x (%d)\n",get_data16(mz->e_cparhdr),get_data16(mz->e_cparhdr));
	printf("e_minalloc: 0x%x (%d)\n",get_data16(mz->e_minalloc),get_data16(mz->e_minalloc));
	printf("e_maxalloc: 0x%x (%d)\n",get_data16(mz->e_maxalloc),get_data16(mz->e_maxalloc));
	printf("e_ss: 0x%x (%d)\n",get_data16(mz->e_ss),get_data16(mz->e_ss));
	printf("e_sp: 0x%x (%d)\n",get_data16(mz->e_sp),get_data16(mz->e_sp));
	printf("e_csum: 0x%x (%d)\n",get_data16(mz->e_csum),get_data16(mz->e_csum));
	printf("e_ip: 0x%x (%d)\n",get_data16(mz->e_ip),get_data16(mz->e_ip));
	printf("e_cs: 0x%x (%d)\n",get_data16(mz->e_cs),get_data16(mz->e_cs));
	printf("e_lfarlc: 0x%x (%d)\n",get_data16(mz->e_lfarlc),get_data16(mz->e_lfarlc));
	printf("e_ovno: 0x%x (%d)\n",get_data16(mz->e_ovno),get_data16(mz->e_ovno));
	strncpy(ss,(char*)mz->e_res,4);
	printf("e_res: %s\n",ss);
	printf("e_oemid: 0x%x (%d)\n",get_data16(mz->e_oemid),get_data16(mz->e_oemid));
	printf("e_oeminfo: 0x%x (%d)\n",get_data16(mz->e_oeminfo),get_data16(mz->e_oeminfo));
	strncpy(ss,(char*)mz->e_res2,20);
	printf("e_res2: %s\n",ss);
	printf("e_lfanew: 0x%x (%d)\n",get_data32(mz->e_lfanew),get_data32(mz->e_lfanew));
}
void print_pe_hdr(void)
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	int x;
	
	if(!fc_ptr) return;
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	x=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+x);
	printf("Signature: 0x%x (%d)\n",get_data32(pe->Signature),get_data32(pe->Signature));
	printf("* FileHeader\n");
	printf("* OptionalHeader\n");
}
void print_fileheader_hdr()
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	int x;
	
	if(!fc_ptr) return;
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	x=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+x);
	printf("Machine: 0x%x (%d)\n",get_data16(pe->FileHeader.Machine),get_data16(pe->FileHeader.Machine));
	printf("NumberOfSections: 0x%x (%d)\n",get_data16(pe->FileHeader.NumberOfSections),get_data16(pe->FileHeader.NumberOfSections));
	printf("TimeDateStamp: 0x%x (%d)\n",get_data32(pe->FileHeader.TimeDateStamp),get_data32(pe->FileHeader.TimeDateStamp));
	printf("PointerToSymbolTable: 0x%x (%d)\n",get_data32(pe->FileHeader.PointerToSymbolTable),get_data32(pe->FileHeader.PointerToSymbolTable));
	printf("NumberOfSymbols: 0x%x (%d)\n",get_data32(pe->FileHeader.NumberOfSymbols),get_data32(pe->FileHeader.NumberOfSymbols));
	printf("SizeOfOptionalHeader: 0x%x (%d)\n",get_data16(pe->FileHeader.SizeOfOptionalHeader),get_data16(pe->FileHeader.SizeOfOptionalHeader));
	printf("Characteristics: 0x%x (%d)\n",get_data16(pe->FileHeader.Characteristics),get_data16(pe->FileHeader.Characteristics));
}
void print_optionalheader_hdr()
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_NT_HEADERS64 *pe64;
	int x;
	
	if(!fc_ptr) return;
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	x=get_data32(mz->e_lfanew);
	switch(fc_ptr->file_bit_class) {
		case bit64:
		pe64=(_IMAGE_NT_HEADERS64*)(fc_ptr->faddr+x);
		printf("Magic: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.Magic),get_data16(pe64->OptionalHeader.Magic));
		printf("MajorLinkerVersion: 0x%x (%d)\n",get_data8(pe64->OptionalHeader.MajorLinkerVersion),get_data8(pe64->OptionalHeader.MajorLinkerVersion));
		printf("MinorLinkerVersion: 0x%x (%d)\n",get_data8(pe64->OptionalHeader.MinorLinkerVersion),get_data8(pe64->OptionalHeader.MinorLinkerVersion));
		printf("SizeOfCode: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SizeOfCode),get_data32(pe64->OptionalHeader.SizeOfCode));
		printf("SizeOfInitializedData: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SizeOfInitializedData),get_data32(pe64->OptionalHeader.SizeOfInitializedData));
		printf("SizeOfUninitializedData: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SizeOfUninitializedData),get_data32(pe64->OptionalHeader.SizeOfUninitializedData));
		printf("AddressOfEntryPoint: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.AddressOfEntryPoint),get_data32(pe64->OptionalHeader.AddressOfEntryPoint));
		printf("BaseOfCode: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.BaseOfCode),get_data32(pe64->OptionalHeader.BaseOfCode));
		//printf("BaseOfData: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.BaseOfData),get_data32(pe64->OptionalHeader.BaseOfData));
		printf("ImageBase: 0x%x (%d)\n",get_data64(pe64->OptionalHeader.ImageBase),get_data64(pe64->OptionalHeader.ImageBase));
		printf("SectionAlignment: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SectionAlignment),get_data32(pe64->OptionalHeader.SectionAlignment));
		printf("FileAlignment: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.FileAlignment),get_data32(pe64->OptionalHeader.FileAlignment));
		printf("MajorOperatingSystemVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MajorOperatingSystemVersion),get_data16(pe64->OptionalHeader.MajorOperatingSystemVersion));
		printf("MinorOperatingSystemVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MinorOperatingSystemVersion),get_data16(pe64->OptionalHeader.MinorOperatingSystemVersion));
		printf("MajorImageVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MajorImageVersion),get_data16(pe64->OptionalHeader.MajorImageVersion));
		printf("MinorImageVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MinorImageVersion),get_data16(pe64->OptionalHeader.MinorImageVersion));
		printf("MajorSubsystemVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MajorSubsystemVersion),get_data16(pe64->OptionalHeader.MajorSubsystemVersion));
		printf("MinorSubsystemVersion: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.MinorSubsystemVersion),get_data16(pe64->OptionalHeader.MinorSubsystemVersion));
		printf("Win32VersionValue: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.Win32VersionValue),get_data32(pe64->OptionalHeader.Win32VersionValue));
		printf("SizeOfImage: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SizeOfImage),get_data32(pe64->OptionalHeader.SizeOfImage));
		printf("SizeOfHeaders: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.SizeOfHeaders),get_data32(pe64->OptionalHeader.SizeOfHeaders));
		printf("CheckSum: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.CheckSum),get_data32(pe64->OptionalHeader.CheckSum));
		printf("Subsystem: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.Subsystem),get_data16(pe64->OptionalHeader.Subsystem));
		printf("DllCharacteristics: 0x%x (%d)\n",get_data16(pe64->OptionalHeader.DllCharacteristics),get_data16(pe64->OptionalHeader.DllCharacteristics));
		printf("SizeOfStackReserve: 0x%x (%d)\n",get_data64(pe64->OptionalHeader.SizeOfStackReserve),get_data64(pe64->OptionalHeader.SizeOfStackReserve));
		printf("SizeOfStackCommit: 0x%x (%d)\n",get_data64(pe64->OptionalHeader.SizeOfStackCommit),get_data64(pe64->OptionalHeader.SizeOfStackCommit));
		printf("SizeOfHeapReserve: 0x%x (%d)\n",get_data64(pe64->OptionalHeader.SizeOfHeapReserve),get_data64(pe64->OptionalHeader.SizeOfHeapReserve));
		printf("SizeOfHeapCommit: 0x%x (%d)\n",get_data64(pe64->OptionalHeader.SizeOfHeapCommit),get_data64(pe64->OptionalHeader.SizeOfHeapCommit));
		printf("LoaderFlags: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.LoaderFlags),get_data32(pe64->OptionalHeader.LoaderFlags));
		printf("NumberOfRvaAndSizes: 0x%x (%d)\n",get_data32(pe64->OptionalHeader.NumberOfRvaAndSizes),get_data32(pe64->OptionalHeader.NumberOfRvaAndSizes));
			break;
		case bit32:
		default:
		pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+x);
		printf("Magic: 0x%x (%d)\n",get_data16(pe->OptionalHeader.Magic),get_data16(pe->OptionalHeader.Magic));
		printf("MajorLinkerVersion: 0x%x (%d)\n",get_data8(pe->OptionalHeader.MajorLinkerVersion),get_data8(pe->OptionalHeader.MajorLinkerVersion));
		printf("MinorLinkerVersion: 0x%x (%d)\n",get_data8(pe->OptionalHeader.MinorLinkerVersion),get_data8(pe->OptionalHeader.MinorLinkerVersion));
		printf("SizeOfCode: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfCode),get_data32(pe->OptionalHeader.SizeOfCode));
		printf("SizeOfInitializedData: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfInitializedData),get_data32(pe->OptionalHeader.SizeOfInitializedData));
		printf("SizeOfUninitializedData: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfUninitializedData),get_data32(pe->OptionalHeader.SizeOfUninitializedData));
		printf("AddressOfEntryPoint: 0x%x (%d)\n",get_data32(pe->OptionalHeader.AddressOfEntryPoint),get_data32(pe->OptionalHeader.AddressOfEntryPoint));
		printf("BaseOfCode: 0x%x (%d)\n",get_data32(pe->OptionalHeader.BaseOfCode),get_data32(pe->OptionalHeader.BaseOfCode));
		printf("BaseOfData: 0x%x (%d)\n",get_data32(pe->OptionalHeader.BaseOfData),get_data32(pe->OptionalHeader.BaseOfData));
		printf("ImageBase: 0x%x (%d)\n",get_data32(pe->OptionalHeader.ImageBase),get_data32(pe->OptionalHeader.ImageBase));
		printf("SectionAlignment: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SectionAlignment),get_data32(pe->OptionalHeader.SectionAlignment));
		printf("FileAlignment: 0x%x (%d)\n",get_data32(pe->OptionalHeader.FileAlignment),get_data32(pe->OptionalHeader.FileAlignment));
		printf("MajorOperatingSystemVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MajorOperatingSystemVersion),get_data16(pe->OptionalHeader.MajorOperatingSystemVersion));
		printf("MinorOperatingSystemVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MinorOperatingSystemVersion),get_data16(pe->OptionalHeader.MinorOperatingSystemVersion));
		printf("MajorImageVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MajorImageVersion),get_data16(pe->OptionalHeader.MajorImageVersion));
		printf("MinorImageVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MinorImageVersion),get_data16(pe->OptionalHeader.MinorImageVersion));
		printf("MajorSubsystemVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MajorSubsystemVersion),get_data16(pe->OptionalHeader.MajorSubsystemVersion));
		printf("MinorSubsystemVersion: 0x%x (%d)\n",get_data16(pe->OptionalHeader.MinorSubsystemVersion),get_data16(pe->OptionalHeader.MinorSubsystemVersion));
		printf("Reserved1: 0x%x (%d)\n",get_data32(pe->OptionalHeader.Reserved1),get_data32(pe->OptionalHeader.Reserved1));
		printf("SizeOfImage: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfImage),get_data32(pe->OptionalHeader.SizeOfImage));
		printf("SizeOfHeaders: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfHeaders),get_data32(pe->OptionalHeader.SizeOfHeaders));
		printf("CheckSum: 0x%x (%d)\n",get_data32(pe->OptionalHeader.CheckSum),get_data32(pe->OptionalHeader.CheckSum));
		printf("Subsystem: 0x%x (%d)\n",get_data16(pe->OptionalHeader.Subsystem),get_data16(pe->OptionalHeader.Subsystem));
		printf("DllCharacteristics: 0x%x (%d)\n",get_data16(pe->OptionalHeader.DllCharacteristics),get_data16(pe->OptionalHeader.DllCharacteristics));
		printf("SizeOfStackReserve: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfStackReserve),get_data32(pe->OptionalHeader.SizeOfStackReserve));
		printf("SizeOfStackCommit: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfStackCommit),get_data32(pe->OptionalHeader.SizeOfStackCommit));
		printf("SizeOfHeapReserve: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfHeapReserve),get_data32(pe->OptionalHeader.SizeOfHeapReserve));
		printf("SizeOfHeapCommit: 0x%x (%d)\n",get_data32(pe->OptionalHeader.SizeOfHeapCommit),get_data32(pe->OptionalHeader.SizeOfHeapCommit));
		printf("LoaderFlags: 0x%x (%d)\n",get_data32(pe->OptionalHeader.LoaderFlags),get_data32(pe->OptionalHeader.LoaderFlags));
		printf("NumberOfRvaAndSizes: 0x%x (%d)\n",get_data32(pe->OptionalHeader.NumberOfRvaAndSizes),get_data32(pe->OptionalHeader.NumberOfRvaAndSizes));
		break;
		}
	}
void print_pe_s_var(struct _fmt *fmt,struct _structvar *ptr)
{
	int sect=SECT_NULL;
	struct _var *var;
	if((strcmp(ptr->name,"mz"))==0) sect=SECT_MZ;
	if((strcmp(ptr->name,"pe"))==0) sect=SECT_PE;
	if((strcmp(ptr->name,"sect"))==0) sect=SECT_PESECT;
	if(!ptr->next) {
		switch(sect) {
			case SECT_MZ:
				if(ptr->num!=-1) {printf("invalid section.\n"); return;}
				print_mz_hdr();
				break;
			case SECT_PE:
				if(ptr->num!=-1) {printf("invalid section.\n"); return;}
				print_pe_hdr();
				break;
			case SECT_PESECT:
				if(ptr->num==-1) {print_pe_sectlist();return;}else{print_pe_sect(ptr->num);return;}
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(!ptr->next->next){
		if((((strcmp(ptr->next->name,"FileHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_FILEHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
			print_fileheader_hdr();return;
		}
		if((((strcmp(ptr->next->name,"OptionalHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_OPTIONALHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
			print_optionalheader_hdr();return;
		}
		}
		if(ptr->next->num!=-1) {printf("invalid section.\n"); return;}
		var=pe_get_s_val(ptr);
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
void print_pe_sectlist()
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_NT_HEADERS64 *pe64;
	_IMAGE_SECTION_HEADER *sect;
	int x,n,maxsect,peoff;
	off_t offset;
	char ss[15];
	if(!fc_ptr) return;
	maxsect=get_max_pesect();
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	peoff=get_data32(mz->e_lfanew);
	switch(fc_ptr->file_bit_class) {
		case bit64:
			sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64));
			for(n=0;n<=maxsect;n++)
			{
				strncpy(ss,(char*)sect->Name,8);
				printf("sect[%d] [%s]\n",n,ss);
				sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64)+(sizeof(_IMAGE_SECTION_HEADER)*(n+1)));
			}
			break;
		case bit32:
		default:
			sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS));
			for(n=0;n<=maxsect;n++)
			{
				strncpy(ss,(char*)sect->Name,8);
				printf("sect[%d] [%s]\n",n,ss);
				sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS)+(sizeof(_IMAGE_SECTION_HEADER)*(n+1)));
			}
			break;
	}
}
void print_pe_sect(int num)
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_NT_HEADERS64 *pe64;
	_IMAGE_SECTION_HEADER *sect;
	int x,n,peoff;
	off_t offset;
	char ss[15];
	if(!fc_ptr) return;
	if(num>get_max_pesect()) {printf("out of sect bounds.\n");return;}
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	peoff=get_data32(mz->e_lfanew);
	switch(fc_ptr->file_bit_class) {
		case bit64:
			sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64));
			for(n=0;n<=num;n++)
			{
				sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64)+(sizeof(_IMAGE_SECTION_HEADER)*n));
			}
			break;
		case bit32:
		default:
			sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS));
			for(n=0;n<=num;n++)
			{
				sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS)+(sizeof(_IMAGE_SECTION_HEADER)*n));
			}
			break;
	}
	strncpy(ss,(char*)sect->Name,8);
	printf("Name: %s\n",ss);
	printf("PhysicalAddress/VirtualSize: 0x%x (%d)\n",get_data32(sect->Misc.PhysicalAddress),get_data32(sect->Misc.PhysicalAddress));
	printf("VirtualAddress: 0x%x (%d)\n",get_data32(sect->VirtualAddress),get_data32(sect->VirtualAddress));
	printf("SizeOfRawData: 0x%x (%d)\n",get_data32(sect->SizeOfRawData),get_data32(sect->SizeOfRawData));
	printf("PointerToRawData: 0x%x (%d)\n",get_data32(sect->PointerToRawData),get_data32(sect->PointerToRawData));
	printf("PointerToRelocations: 0x%x (%d)\n",get_data32(sect->PointerToRelocations),get_data32(sect->PointerToRelocations));
	printf("PointerToLinenumbers: 0x%x (%d)\n",get_data32(sect->PointerToLinenumbers),get_data32(sect->PointerToLinenumbers));
	printf("NumberOfRelocations: 0x%x (%d)\n",get_data16(sect->NumberOfRelocations),get_data16(sect->NumberOfRelocations));
	printf("NumberOfLinenumbers: 0x%x (%d)\n",get_data16(sect->NumberOfLinenumbers),get_data16(sect->NumberOfLinenumbers));
	printf("Characteristics: 0x%x (%d)\n",get_data32(sect->Characteristics),get_data32(sect->Characteristics));
}
void add_pe_completion()
{
	int n,x;
	char s[255];
#ifdef HAVE_LIBREADLINE
	add_completion("mz",NULL,comp_discardable);
	add_completion("mz::e_magic",NULL,comp_discardable);
	add_completion("mz::e_cblp",NULL,comp_discardable);
	add_completion("mz::e_cp",NULL,comp_discardable);
	add_completion("mz::e_crlc",NULL,comp_discardable);
	add_completion("mz::e_cparhdr",NULL,comp_discardable);
	add_completion("mz::e_minalloc",NULL,comp_discardable);
	add_completion("mz::e_maxalloc",NULL,comp_discardable);
	add_completion("mz::e_ss",NULL,comp_discardable);
	add_completion("mz::e_sp",NULL,comp_discardable);
	add_completion("mz::e_csum",NULL,comp_discardable);
	add_completion("mz::e_ip",NULL,comp_discardable);
	add_completion("mz::e_cs",NULL,comp_discardable);
	add_completion("mz::e_lfarlc",NULL,comp_discardable);
	add_completion("mz::e_ovno",NULL,comp_discardable);
	add_completion("mz::e_res",NULL,comp_discardable);
	add_completion("mz::e_oemid",NULL,comp_discardable);
	add_completion("mz::e_oeminfo",NULL,comp_discardable);
	add_completion("mz::e_res2",NULL,comp_discardable);
	add_completion("mz::e_lfanew",NULL,comp_discardable);
	add_completion("pe",NULL,comp_discardable);
	add_completion("pe::Signature",NULL,comp_discardable);
	add_completion("pe::FileHeader",NULL,comp_discardable);
	add_completion("pe::OptionalHeader",NULL,comp_discardable);
	add_completion("pe::FileHeader::Machine",NULL,comp_discardable);
	add_completion("pe::FileHeader::NumberOfSections",NULL,comp_discardable);
	add_completion("pe::FileHeader::TimeDateStamp",NULL,comp_discardable);
	add_completion("pe::FileHeader::PointerToSymbolTable",NULL,comp_discardable);
	add_completion("pe::FileHeader::NumberOfSymbols",NULL,comp_discardable);
	add_completion("pe::FileHeader::SizeOfOptionalHeader",NULL,comp_discardable);
	add_completion("pe::FileHeader::Characteristics",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::Magic",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MajorLinkerVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MinorLinkerVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfCode",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfInitializedData",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfUninitializedData",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::AddressOfEntryPoint",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::BaseOfCode",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::BaseOfData",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::ImageBase",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SectionAlignment",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::FileAlignment",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MajorOperatingSystemVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MinorOperatingSystemVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MajorImageVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MinorImageVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MajorSubsystemVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::MinorSubsystemVersion",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::Reserved1",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfImage",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfHeaders",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::CheckSum",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::Subsystem",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::DllCharacteristics",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfStackReserve",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfStackCommit",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfHeapReserve",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::SizeOfHeapCommit",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::LoaderFlags",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::NumberOfRvaAndSizes",NULL,comp_discardable);
	add_completion("pe::OptionalHeader::Win32VersionValue",NULL,comp_discardable);
	add_completion("sect",NULL,comp_discardable);
	x=get_max_pesect();
	for(n=0;n<=x;n++)
	{
		sprintf(s,"sect[%d]",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::Name",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::PhysicalAddress",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::VirtualAddress",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::SizeOfRawData",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::PointerToRawData",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::PointerToRelocations",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::PointerToLinenumbers",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::NumberOfRelocations",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::NumberOfLinenumbers",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"sect[%d]::Characteristics",n);
		add_completion(s,NULL,comp_discardable);
	}
#endif
}
