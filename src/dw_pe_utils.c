/*
 dw_pe_utils.c : pe file format utils.

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
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include"libdwarf.h"

void dw_pe_decode_file_cpu(char *s,int cpu)
{
	switch(cpu) {
		case IMAGE_FILE_MACHINE_UNKNOWN:
			strcpy(s,"unknown");
			break;
		case IMAGE_FILE_MACHINE_AM33:
			strcpy(s,"Matsushita AM33");
			break;
		case IMAGE_FILE_MACHINE_AMD64:
			strcpy(s,"x64");
			break;
		case IMAGE_FILE_MACHINE_ARM:
			strcpy(s,"ARM little endian");
			break;
		case IMAGE_FILE_MACHINE_EBC:
			strcpy(s,"EFI byte code");
			break;
		case IMAGE_FILE_MACHINE_I386:
			strcpy(s,"Intel 386");
			break;
		case IMAGE_FILE_MACHINE_IA64:
			strcpy(s,"Intel Itanium processor family");
			break;
		case IMAGE_FILE_MACHINE_M32R:
			strcpy(s,"Mitsubishi M32R little endian");
			break;
		case IMAGE_FILE_MACHINE_MIPS16:
			strcpy(s,"MIPS16");
			break;
		case IMAGE_FILE_MACHINE_MIPSFPU:
			strcpy(s,"MIPS with FPU");
			break;
		case IMAGE_FILE_MACHINE_MIPSFPU16:
			strcpy(s,"MIPS16 with FPU");
			break;
		case IMAGE_FILE_MACHINE_POWERPC:
			strcpy(s,"Power PC little endian");
			break;
		case IMAGE_FILE_MACHINE_POWERPCFP:
			strcpy(s,"Power PC with floating point support");
			break;
		case IMAGE_FILE_MACHINE_R4000:
			strcpy(s,"MIPS little endian");
			break;
		case IMAGE_FILE_MACHINE_SH3:
			strcpy(s,"Hitachi SH3");
			break;
		case IMAGE_FILE_MACHINE_SH3DSP:
			strcpy(s,"Hitachi SH3 DSP");
			break;
		case IMAGE_FILE_MACHINE_SH4:
			strcpy(s,"Hitachi SH4");
			break;
		case IMAGE_FILE_MACHINE_SH5:
			strcpy(s,"Hitachi SH5");
			break;
		case IMAGE_FILE_MACHINE_THUMB:
			strcpy(s,"Thumb");
			break;
		case IMAGE_FILE_MACHINE_WCEMIPSV2:
			strcpy(s,"MIPS little-endian WCE v2");
			break;
		default:
			strcpy(s,"unknown");
			break;
	}
}
int get_max_pesect()
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	int x;
	
	if(!fc_ptr) return 0;
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	x=get_data32(mz->e_lfanew);
	pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+x);
	x=pe->FileHeader.NumberOfSections-1;
	return x;
}
struct _var* pe_get_s_val(struct _structvar *ptr)
{
	int sect=SECT_NULL;
	struct _var *var;
	var=NULL;
	if((strcmp(ptr->name,"mz"))==0) sect=SECT_MZ;
	if((strcmp(ptr->name,"pe"))==0) sect=SECT_PE;
	if((strcmp(ptr->name,"sect"))==0) sect=SECT_PESECT;
	if((ptr->next)&&(!ptr->next->next)) {
		switch(sect) {
			case SECT_MZ:
				if((ptr->num!=-1)||(ptr->next->num!=-1)) {printf("invalid section.\n"); return NULL;}
				var=get_pe_var(sect,ptr->num,ptr->next->name);
				break;
			case SECT_PE:
				if((ptr->num!=-1)||(ptr->next->num!=-1)) {printf("invalid section.\n"); return NULL;}
				var=get_pe_var(sect,ptr->num,ptr->next->name);
				break;
			case SECT_PESECT:
				if((ptr->num==-1)||(ptr->next->num!=-1)) {printf("invalid section.\n"); return NULL;}
				var=get_pe_var(sect,ptr->num,ptr->next->name);
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(!ptr->next->next->next){
		if((((strcmp(ptr->next->name,"FileHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_FILEHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return NULL;}
			var=get_pe_var(sect,ptr->num,ptr->next->next->name);
		}
		if((((strcmp(ptr->next->name,"OptionalHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_OPTIONALHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return NULL;}
			var=get_pe_var(sect,ptr->num,ptr->next->next->name);
		}
		}
		else {printf("invalid section.\n"); return NULL;}
	}
		return var;
}
struct _var* get_pe_var(int sect,int num,char *varname)
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_NT_HEADERS64 *pe64;
	_IMAGE_SECTION_HEADER *pe_sect;
	int n,peoff;
	//off_t offset;
	//char ss[15];
	struct _var *var;
	
	var=createtmpvar();
	if(!var) {return NULL;}
	if(!fc_ptr) return NULL;
	//int maxsect=get_max_pesect();
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	peoff=get_data32(mz->e_lfanew);
	switch(sect) {
		case SECT_MZ:
			if((strcmp(varname,"e_magic"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_magic);}
			if((strcmp(varname,"e_cblp"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_cblp);}
			if((strcmp(varname,"e_cp"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_cp);}
			if((strcmp(varname,"e_crlc"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_crlc);}
			if((strcmp(varname,"e_cparhdr"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_cparhdr);}
			if((strcmp(varname,"e_minalloc"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_minalloc);}
			if((strcmp(varname,"e_maxalloc"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_maxalloc);}
			if((strcmp(varname,"e_ss"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_ss);}
			if((strcmp(varname,"e_sp"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_sp);}
			if((strcmp(varname,"e_csum"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_csum);}
			if((strcmp(varname,"e_ip"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_ip);}
			if((strcmp(varname,"e_cs"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_cs);}
			if((strcmp(varname,"e_lfarlc"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_lfarlc);}
			if((strcmp(varname,"e_ovno"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_ovno);}
			//if((strcmp(varname,"e_res"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_res);}
			if((strcmp(varname,"e_oemid"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_oemid);}
			if((strcmp(varname,"e_oeminfo"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_oeminfo);}
			//if((strcmp(varname,"e_res2"))==0) {var->type=VART_NUM;var->val.num=get_data16(mz->e_res2);}
			if((strcmp(varname,"e_lfanew"))==0) {var->type=VART_NUM;var->val.num=get_data32(mz->e_lfanew);}
			break;
		case SECT_PE:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			if((strcmp(varname,"Signature"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->Signature);}
			break;
		case SECT_PE_FILEHEADER:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			if((strcmp(varname,"Machine"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->FileHeader.Machine);}
			if((strcmp(varname,"NumberOfSections"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->FileHeader.NumberOfSections);}
			if((strcmp(varname,"TimeDateStamp"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->FileHeader.TimeDateStamp);}
			if((strcmp(varname,"PointerToSymbolTable"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->FileHeader.PointerToSymbolTable);}
			if((strcmp(varname,"NumberOfSymbols"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->FileHeader.NumberOfSymbols);}
			if((strcmp(varname,"SizeOfOptionalHeader"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->FileHeader.SizeOfOptionalHeader);}
			if((strcmp(varname,"Characteristics"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->FileHeader.Characteristics);}
			break;
		case SECT_PE_OPTIONALHEADER:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			pe64=(_IMAGE_NT_HEADERS64*)(fc_ptr->faddr+peoff);
			switch(fc_ptr->file_bit_class) {
				case bit64:
					if((strcmp(varname,"Magic"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.Magic);}
					if((strcmp(varname,"MajorLinkerVersion"))==0) {var->type=VART_NUM;var->val.num=get_data8(pe64->OptionalHeader.MajorLinkerVersion);}
					if((strcmp(varname,"MinorLinkerVersion"))==0) {var->type=VART_NUM;var->val.num=get_data8(pe64->OptionalHeader.MinorLinkerVersion);}
					if((strcmp(varname,"SizeOfCode"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SizeOfCode);}
					if((strcmp(varname,"SizeOfInitializedData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SizeOfInitializedData);}
					if((strcmp(varname,"SizeOfUninitializedData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SizeOfUninitializedData);}
					if((strcmp(varname,"AddressOfEntryPoint"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.AddressOfEntryPoint);}
					if((strcmp(varname,"BaseOfCode"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.BaseOfCode);}
					//if((strcmp(varname,"BaseOfData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.BaseOfData);}
					if((strcmp(varname,"ImageBase"))==0) {var->type=VART_NUM;var->val.num=get_data64(pe64->OptionalHeader.ImageBase);}
					if((strcmp(varname,"SectionAlignment"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SectionAlignment);}
					if((strcmp(varname,"FileAlignment"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.FileAlignment);}
					if((strcmp(varname,"MajorOperatingSystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MajorOperatingSystemVersion);}
					if((strcmp(varname,"MinorOperatingSystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MinorOperatingSystemVersion);}
					if((strcmp(varname,"MajorImageVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MajorImageVersion);}
					if((strcmp(varname,"MinorImageVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MinorImageVersion);}
					if((strcmp(varname,"MajorSubsystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MajorSubsystemVersion);}
					if((strcmp(varname,"MinorSubsystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.MinorSubsystemVersion);}
					if((strcmp(varname,"Win32VersionValue"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.Win32VersionValue);}
					if((strcmp(varname,"SizeOfImage"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SizeOfImage);}
					if((strcmp(varname,"SizeOfHeaders"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.SizeOfHeaders);}
					if((strcmp(varname,"CheckSum"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.CheckSum);}
					if((strcmp(varname,"Subsystem"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.Subsystem);}
					if((strcmp(varname,"DllCharacteristics"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe64->OptionalHeader.DllCharacteristics);}
					if((strcmp(varname,"SizeOfStackReserve"))==0) {var->type=VART_NUM;var->val.num=get_data64(pe64->OptionalHeader.SizeOfStackReserve);}
					if((strcmp(varname,"SizeOfStackCommit"))==0) {var->type=VART_NUM;var->val.num=get_data64(pe64->OptionalHeader.SizeOfStackCommit);}
					if((strcmp(varname,"SizeOfHeapReserve"))==0) {var->type=VART_NUM;var->val.num=get_data64(pe64->OptionalHeader.SizeOfHeapReserve);}
					if((strcmp(varname,"SizeOfHeapCommit"))==0) {var->type=VART_NUM;var->val.num=get_data64(pe64->OptionalHeader.SizeOfHeapCommit);}
					if((strcmp(varname,"LoaderFlags"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.LoaderFlags);}
					if((strcmp(varname,"NumberOfRvaAndSizes"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe64->OptionalHeader.NumberOfRvaAndSizes);}
					break;
				case bit32:
				default:
					if((strcmp(varname,"Magic"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.Magic);}
					if((strcmp(varname,"MajorLinkerVersion"))==0) {var->type=VART_NUM;var->val.num=get_data8(pe->OptionalHeader.MajorLinkerVersion);}
					if((strcmp(varname,"MinorLinkerVersion"))==0) {var->type=VART_NUM;var->val.num=get_data8(pe->OptionalHeader.MinorLinkerVersion);}
					if((strcmp(varname,"SizeOfCode"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfCode);}
					if((strcmp(varname,"SizeOfInitializedData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfInitializedData);}
					if((strcmp(varname,"SizeOfUninitializedData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfUninitializedData);}
					if((strcmp(varname,"AddressOfEntryPoint"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.AddressOfEntryPoint);}
					if((strcmp(varname,"BaseOfCode"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.BaseOfCode);}
					if((strcmp(varname,"BaseOfData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.BaseOfData);}
					if((strcmp(varname,"ImageBase"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.ImageBase);}
					if((strcmp(varname,"SectionAlignment"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SectionAlignment);}
					if((strcmp(varname,"FileAlignment"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.FileAlignment);}
					if((strcmp(varname,"MajorOperatingSystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MajorOperatingSystemVersion);}
					if((strcmp(varname,"MinorOperatingSystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MinorOperatingSystemVersion);}
					if((strcmp(varname,"MajorImageVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MajorImageVersion);}
					if((strcmp(varname,"MinorImageVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MinorImageVersion);}
					if((strcmp(varname,"MajorSubsystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MajorSubsystemVersion);}
					if((strcmp(varname,"MinorSubsystemVersion"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.MinorSubsystemVersion);}
					if((strcmp(varname,"Reserved1"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.Reserved1);}
					if((strcmp(varname,"SizeOfImage"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfImage);}
					if((strcmp(varname,"SizeOfHeaders"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfHeaders);}
					if((strcmp(varname,"CheckSum"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.CheckSum);}
					if((strcmp(varname,"Subsystem"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.Subsystem);}
					if((strcmp(varname,"DllCharacteristics"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe->OptionalHeader.DllCharacteristics);}
					if((strcmp(varname,"SizeOfStackReserve"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfStackReserve);}
					if((strcmp(varname,"SizeOfStackCommit"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfStackCommit);}
					if((strcmp(varname,"SizeOfHeapReserve"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfHeapReserve);}
					if((strcmp(varname,"SizeOfHeapCommit"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.SizeOfHeapCommit);}
					if((strcmp(varname,"LoaderFlags"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.LoaderFlags);}
					if((strcmp(varname,"NumberOfRvaAndSizes"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe->OptionalHeader.NumberOfRvaAndSizes);}
					break;
			}
			break;
		case SECT_PESECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
				case bit32:
				default:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
			}
			if((strcmp(varname,"Name"))==0) {var->type=VART_WORD;var->val.s=(char*)malloc(10);if(!var->val.s) die("error allocating str mem");strncpy(var->val.s,(char*)pe_sect->Name,8);}
			if((strcmp(varname,"PhysicalAddress"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->Misc.PhysicalAddress);}
			if((strcmp(varname,"VirtualAddress"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->VirtualAddress);}
			if((strcmp(varname,"SizeOfRawData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->SizeOfRawData);}
			if((strcmp(varname,"PointerToRawData"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->PointerToRawData);}
			if((strcmp(varname,"PointerToRelocations"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->PointerToRelocations);}
			if((strcmp(varname,"PointerToLinenumbers"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->PointerToLinenumbers);}
			if((strcmp(varname,"NumberOfRelocations"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe_sect->NumberOfRelocations);}
			if((strcmp(varname,"NumberOfLinenumbers"))==0) {var->type=VART_NUM;var->val.num=get_data16(pe_sect->NumberOfLinenumbers);}
			if((strcmp(varname,"Characteristics"))==0) {var->type=VART_NUM;var->val.num=get_data32(pe_sect->Characteristics);}
			break;
		default:
			destroytmpvar(var);
			var=NULL;
			break;
	}
	return var;
}
void pe_set_s_val(struct _structvar *ptr,struct _var *val)
{
	int sect=SECT_NULL;
	//struct _var *var;
	//var=NULL;
	if((strcmp(ptr->name,"mz"))==0) sect=SECT_MZ;
	if((strcmp(ptr->name,"pe"))==0) sect=SECT_PE;
	if((strcmp(ptr->name,"sect"))==0) sect=SECT_PESECT;
	if((ptr->next)&&(!ptr->next->next)) {
		switch(sect) {
			case SECT_MZ:
				if((ptr->num!=-1)||(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
				set_pe_var(sect,ptr->num,ptr->next->name,val);
				break;
			case SECT_PE:
				if((ptr->num!=-1)||(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
				set_pe_var(sect,ptr->num,ptr->next->name,val);
				break;
			case SECT_PESECT:
				if((ptr->num==-1)||(ptr->next->num==-1)) {printf("invalid section.\n"); return;}
				set_pe_var(sect,ptr->num,ptr->next->name,val);
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(!ptr->next->next->next){
		if((((strcmp(ptr->next->name,"FileHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_FILEHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
			set_pe_var(sect,ptr->num,ptr->next->next->name,val);
		}
		if((((strcmp(ptr->next->name,"OptionalHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_OPTIONALHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return;}
			set_pe_var(sect,ptr->num,ptr->next->next->name,val);
		}
		}
		else {printf("invalid section.\n"); return;}
	}
}
void set_pe_var(int sect,int num,char *varname,struct _var *val)
{
	_IMAGE_DOS_HEADER *mz;
	_IMAGE_NT_HEADERS *pe;
	_IMAGE_NT_HEADERS64 *pe64;
	_IMAGE_SECTION_HEADER *pe_sect;
	int n,peoff;
	//off_t offset;
	//char ss[15];
	
	if(!fc_ptr) return;
	//int maxsect=get_max_pesect();
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	peoff=get_data32(mz->e_lfanew);
	switch(sect) {
		case SECT_MZ:
			if((strcmp(varname,"e_magic"))==0) {if(val->type==VART_NUM){mz->e_magic=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_cblp"))==0) {if(val->type==VART_NUM){mz->e_cblp=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_cp"))==0) {if(val->type==VART_NUM){mz->e_cp=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_crlc"))==0) {if(val->type==VART_NUM){mz->e_crlc=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_cparhdr"))==0) {if(val->type==VART_NUM){mz->e_cparhdr=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_minalloc"))==0) {if(val->type==VART_NUM){mz->e_minalloc=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_maxalloc"))==0) {if(val->type==VART_NUM){mz->e_maxalloc=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_ss"))==0) {if(val->type==VART_NUM){mz->e_ss=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_sp"))==0) {if(val->type==VART_NUM){mz->e_sp=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_csum"))==0) {if(val->type==VART_NUM){mz->e_csum=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_ip"))==0) {if(val->type==VART_NUM){mz->e_ip=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_cs"))==0) {if(val->type==VART_NUM){mz->e_cs=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_lfarlc"))==0) {if(val->type==VART_NUM){mz->e_lfarlc=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_ovno"))==0) {if(val->type==VART_NUM){mz->e_ovno=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			//if((strcmp(varname,"e_res"))==0) {if(val->type==VART_NUM){mz->e_res=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_oemid"))==0) {if(val->type==VART_NUM){mz->e_oemid=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_oeminfo"))==0) {if(val->type==VART_NUM){mz->e_oeminfo=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			//if((strcmp(varname,"e_res2"))==0) {if(val->type==VART_NUM){mz->e_res2=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"e_lfanew"))==0) {if(val->type==VART_NUM){mz->e_lfanew=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		case SECT_PE:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			if((strcmp(varname,"Signature"))==0) {if(val->type==VART_NUM){pe->Signature=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		case SECT_PE_FILEHEADER:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			if((strcmp(varname,"Machine"))==0) {if(val->type==VART_NUM){pe->FileHeader.Machine=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"NumberOfSections"))==0) {if(val->type==VART_NUM){pe->FileHeader.NumberOfSections=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"TimeDateStamp"))==0) {if(val->type==VART_NUM){pe->FileHeader.TimeDateStamp=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"PointerToSymbolTable"))==0) {if(val->type==VART_NUM){pe->FileHeader.PointerToSymbolTable=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"NumberOfSymbols"))==0) {if(val->type==VART_NUM){pe->FileHeader.NumberOfSymbols=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"SizeOfOptionalHeader"))==0) {if(val->type==VART_NUM){pe->FileHeader.SizeOfOptionalHeader=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"Characteristics"))==0) {if(val->type==VART_NUM){pe->FileHeader.Characteristics=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		case SECT_PE_OPTIONALHEADER:
			pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			pe64=(_IMAGE_NT_HEADERS64*)(fc_ptr->faddr+peoff);
			switch(fc_ptr->file_bit_class) {
				case bit64:
					if((strcmp(varname,"Magic"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.Magic=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorLinkerVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MajorLinkerVersion=get_data8(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorLinkerVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MinorLinkerVersion=get_data8(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfCode"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfCode=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfInitializedData"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfInitializedData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfUninitializedData"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfUninitializedData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"AddressOfEntryPoint"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.AddressOfEntryPoint=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"BaseOfCode"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.BaseOfCode=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					//if((strcmp(varname,"BaseOfData"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.BaseOfData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"ImageBase"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.ImageBase=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SectionAlignment"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SectionAlignment=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"FileAlignment"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.FileAlignment=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorOperatingSystemVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MajorOperatingSystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorOperatingSystemVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MinorOperatingSystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorImageVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MajorImageVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorImageVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MinorImageVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorSubsystemVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MajorSubsystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorSubsystemVersion"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.MinorSubsystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"Win32VersionValue"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.Win32VersionValue=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfImage"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfImage=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeaders"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfHeaders=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"CheckSum"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.CheckSum=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"Subsystem"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.Subsystem=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"DllCharacteristics"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.DllCharacteristics=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfStackReserve"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfStackReserve=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfStackCommit"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfStackCommit=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeapReserve"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfHeapReserve=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeapCommit"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.SizeOfHeapCommit=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"LoaderFlags"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.LoaderFlags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"NumberOfRvaAndSizes"))==0) {if(val->type==VART_NUM){pe64->OptionalHeader.NumberOfRvaAndSizes=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				case bit32:
				default:
					if((strcmp(varname,"Magic"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.Magic=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorLinkerVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MajorLinkerVersion=get_data8(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorLinkerVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MinorLinkerVersion=get_data8(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfCode"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfCode=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfInitializedData"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfInitializedData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfUninitializedData"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfUninitializedData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"AddressOfEntryPoint"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.AddressOfEntryPoint=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"BaseOfCode"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.BaseOfCode=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"BaseOfData"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.BaseOfData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"ImageBase"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.ImageBase=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SectionAlignment"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SectionAlignment=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"FileAlignment"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.FileAlignment=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorOperatingSystemVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MajorOperatingSystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorOperatingSystemVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MinorOperatingSystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorImageVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MajorImageVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorImageVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MinorImageVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MajorSubsystemVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MajorSubsystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"MinorSubsystemVersion"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.MinorSubsystemVersion=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"Reserved1"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.Reserved1=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfImage"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfImage=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeaders"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfHeaders=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"CheckSum"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.CheckSum=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"Subsystem"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.Subsystem=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"DllCharacteristics"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.DllCharacteristics=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfStackReserve"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfStackReserve=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfStackCommit"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfStackCommit=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeapReserve"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfHeapReserve=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"SizeOfHeapCommit"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.SizeOfHeapCommit=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"LoaderFlags"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.LoaderFlags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"NumberOfRvaAndSizes"))==0) {if(val->type==VART_NUM){pe->OptionalHeader.NumberOfRvaAndSizes=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
			}
			break;
		case SECT_PESECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
				case bit32:
				default:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
			}
			if((strcmp(varname,"Name"))==0) {if(val->type==VART_WORD){strncpy((char *)pe_sect->Name,val->val.s,8);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"PhysicalAddress"))==0) {if(val->type==VART_NUM){pe_sect->Misc.PhysicalAddress=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"VirtualAddress"))==0) {if(val->type==VART_NUM){pe_sect->VirtualAddress=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"SizeOfRawData"))==0) {if(val->type==VART_NUM){pe_sect->SizeOfRawData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"PointerToRawData"))==0) {if(val->type==VART_NUM){pe_sect->PointerToRawData=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"PointerToRelocations"))==0) {if(val->type==VART_NUM){pe_sect->PointerToRelocations=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"PointerToLinenumbers"))==0) {if(val->type==VART_NUM){pe_sect->PointerToLinenumbers=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"NumberOfRelocations"))==0) {if(val->type==VART_NUM){pe_sect->NumberOfRelocations=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"NumberOfLinenumbers"))==0) {if(val->type==VART_NUM){pe_sect->NumberOfLinenumbers=get_data16(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"Characteristics"))==0) {if(val->type==VART_NUM){pe_sect->Characteristics=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		default:
			
			break;
	}
}
off_t get_pe_offset(struct _structvar *ptr,int endoffset)
{
	int sect=SECT_NULL;
	off_t offs;
	offs=0;
	if((strcmp(ptr->name,"mz"))==0) sect=SECT_MZ;
	if((strcmp(ptr->name,"pe"))==0) sect=SECT_PE;
	if((strcmp(ptr->name,"sect"))==0) sect=SECT_PESECT;
	if(!ptr->next) {
		switch(sect) {
			case SECT_MZ:
				if(ptr->num!=-1) {printf("invalid section.\n"); return 0;}
				printf("pippo ");
				offs=get_pe_offset2(sect,ptr->num,endoffset);
				break;
			case SECT_PE:
				if(ptr->num!=-1) {printf("invalid section.\n"); return 0;}
				offs=get_pe_offset2(sect,ptr->num,endoffset);
				break;
			case SECT_PESECT:
				if(ptr->num==-1) {printf("invalid section.\n"); return 0;}
				offs=get_pe_offset2(sect,ptr->num,endoffset);
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
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return 0;}
			offs=get_pe_offset2(sect,ptr->num,endoffset);
		}
		if((((strcmp(ptr->next->name,"OptionalHeader"))==0))&&(((strcmp(ptr->name,"pe"))==0)))
		{
			sect=SECT_PE_OPTIONALHEADER;
			if((ptr->num!=-1)&&(ptr->next->num!=-1)) {printf("invalid section.\n"); return 0;}
			offs=get_pe_offset2(sect,ptr->num,endoffset);
		}
		}
		else {printf("invalid section.\n"); return 0;}
	}
		return offs;
}
off_t get_pe_offset2(int sect,int num,int endoffset)
{
	_IMAGE_DOS_HEADER *mz;
	//_IMAGE_NT_HEADERS *pe;
	//_IMAGE_NT_HEADERS64 *pe64;
	_IMAGE_SECTION_HEADER *pe_sect;
	int n,peoff;
	off_t offs;
	//char ss[15];
	
	if(!fc_ptr) return 0;
	//int maxsect=get_max_pesect();
	mz=(_IMAGE_DOS_HEADER*)fc_ptr->faddr;
	peoff=get_data32(mz->e_lfanew);
	switch(sect) {
		case SECT_MZ:
			offs=0;
			if(endoffset) offs=sizeof(_IMAGE_DOS_HEADER);
			break;
		case SECT_PE:
			//pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			offs=peoff;
			if(endoffset) {
				switch(fc_ptr->file_bit_class) {
					case bit64:
						offs=offs+sizeof(_IMAGE_NT_HEADERS64);
						break;
					case bit32:
					default:
						offs=offs+sizeof(_IMAGE_NT_HEADERS);
						break;
				}
			}
			break;
		case SECT_PE_FILEHEADER:
			//pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			offs=peoff+sizeof(uint32_t);
			if(endoffset) offs=offs+sizeof(_IMAGE_FILE_HEADER);
			break;
		case SECT_PE_OPTIONALHEADER:
			//pe=(_IMAGE_NT_HEADERS*)(fc_ptr->faddr+peoff);
			//pe64=(_IMAGE_NT_HEADERS64*)(fc_ptr->faddr+peoff);
			offs=peoff+sizeof(uint32_t)+sizeof(_IMAGE_FILE_HEADER);
			if(endoffset){
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=offs+sizeof(_IMAGE_OPTIONAL_HEADER64);
					break;
				case bit32:
				default:
					offs=offs+sizeof(_IMAGE_OPTIONAL_HEADER);
					break;
			}
			}
			break;
		case SECT_PESECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS64)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
				case bit32:
				default:
					pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS));
					for(n=0;n<=num;n++)
					{
						pe_sect=(_IMAGE_SECTION_HEADER*)(fc_ptr->faddr+peoff+sizeof(_IMAGE_NT_HEADERS)+(sizeof(_IMAGE_SECTION_HEADER)*n));
					}
					break;
			}
			offs=(off_t)((off_t)pe_sect-(off_t)fc_ptr->faddr);
			if(endoffset) offs=offs+sizeof(_IMAGE_SECTION_HEADER);
			break;
		default:
			
			break;
	}
	return offs;
}
void create_pe_hdr(int sect,off_t offs,int grow,int update)
{
	if(!fc_ptr) return;
	switch(sect) {
		case SECT_MZ:
			inject_byte(0,offs,sizeof(_IMAGE_DOS_HEADER),grow);
			break;
		case SECT_PE:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(_IMAGE_NT_HEADERS64),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(_IMAGE_NT_HEADERS),grow);
					break;
			}
			if(update)
			{
				printf("update....still coding.\n");
			}
			break;
		case SECT_PESECT:
			inject_byte(0,offs,sizeof(_IMAGE_SECTION_HEADER),grow);
			if(update)
			{
				printf("update....still coding\n");
			}
			break;
		default:
			
			break;
	}
}
