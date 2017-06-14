/*
 dw_macho_utils.c : Mach-O file format utils.

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

void dw_macho_decode_file_cpu(char *s,int cpu)
{
	switch(cpu) {
		case CPU_TYPE_VAX:
			strcpy(s,"VAX");
			break;
		case CPU_TYPE_MC680x0:
			strcpy(s,"MC680x0");
			break;
		case CPU_TYPE_I386:
			strcpy(s,"Intel I386");
			break;
		case CPU_TYPE_X86_64:
			strcpy(s,"Intel IA64");
			break;
		case CPU_TYPE_MC98000:
			strcpy(s,"MC98000");
			break;
		case CPU_TYPE_HPPA:
			strcpy(s,"HPPA");
			break;
		case CPU_TYPE_ARM:
			strcpy(s,"ARM");
			break;
		case CPU_TYPE_MC88000:
			strcpy(s,"MC88000");
			break;
		case CPU_TYPE_SPARC:
			strcpy(s,"SPARC");
			break;
		case CPU_TYPE_I860:
			strcpy(s,"I860");
			break;
		case CPU_TYPE_POWERPC:
			strcpy(s,"PowerPC (PPC32)");
			break;
		case CPU_TYPE_POWERPC64:
			strcpy(s,"PowerPC (PPC64)");
			break;
		default:
			strcpy(s,"unknown");
			break;
	}
}
char* get_lc_type(int type)
{
	static char *lc_table[]={"unknown","LC_SEGMENT","LC_SYMTAB","LC_SYMSEG","LC_THREAD","LC_UNIXTHREAD","LC_LOADFVMLIB","LC_IDFVMLIB","LC_IDENT","LC_FVMFILE","LC_PREPAGE","LC_DYSYMTAB","LC_LOAD_DYLIB","LC_ID_DYLIB","LC_LOAD_DYLINKER","LC_ID_DYLINKER","LC_PREBOUND_DYLIB","LC_ROUTINES","LC_SUB_FRAMEWORK","LC_SUB_UMBRELLA","LC_SUB_CLIENT","LC_SUB_LIBRARY","LC_TWOLEVEL_HINTS","LC_PREBIND_CKSUM","LC_LOAD_WEAK_DYLIB","LC_SEGMENT_64","LC_ROUTINES_64","LC_UUID","LC_RPATH","LC_CODE_SIGNATURE","LC_SEGMENT_SPLIT_INFO","LC_REEXPORT_DYLIB","LC_LAZY_LOAD_DYLIB","LC_ENCRYPTION_INFO 0x21","LC_DYLD_INFO"};
	if(type==LC_LOAD_WEAK_DYLIB) return "LC_LOAD_WEAK_DYLIB";
	if(type==LC_RPATH) return "LC_RPATH";
	if(type==LC_REEXPORT_DYLIB) return "LC_REEXPORT_DYLIB";
	if(type==LC_DYLD_INFO_ONLY) return "LC_DYLD_INFO_ONLY";
	if((type<0)||(type>0x22)) return "unknown";
	return lc_table[type];
}
int max_fat_arch(void)
{
	int x;
	struct fat_header *fat;
	fat=(struct fat_header*) fc_ptr->faddr;
	x=get_data32(fat->nfat_arch)-1;
	return x;
}
struct _var* fatmacho_get_s_val(struct _structvar *ptr)
{
	int sect,num;
	char varname[VARNAME_LEN];
	struct _var *var;
	if(!ptr) return NULL;
	strcpy(varname,"");
	num=ptr->num;
	if((strcmp(ptr->name,"fat"))==0) sect=SECT_FATMACHO;
	if((strcmp(ptr->name,"arch"))==0) sect=SECT_FATARCH;
	if(ptr->next) {strncpy(varname,ptr->next->name,VARNAME_LEN);if(ptr->next->num!=-1) {printf("invalid section.\n");return NULL;}}
	var=get_fatmacho_var(sect,num,varname);
	return var;
}
struct _var* get_fatmacho_var(int sect,int num,char *varname)
{
	struct _var *var;
	struct fat_header *fat;
	struct fat_arch *arch;
	int n,archentsize;
	off_t archoffset;
	if(!fc_ptr) {printf("no file opened.\n"); return NULL;}
	var=createtmpvar();
	if(!var) return NULL;
	switch(sect) {
		case SECT_FATMACHO:
			if(num!=-1) {printf("wrong section.\n"); destroytmpvar(var);return NULL;}
			fat=(struct fat_header*) fc_ptr->faddr;
			if((strcmp(varname,"magic"))==0) {var->type=VART_NUM;var->val.num=get_data32(fat->magic);}
			if((strcmp(varname,"nfat_arch"))==0) {var->type=VART_NUM;var->val.num=get_data32(fat->nfat_arch);}
			break;
		case SECT_FATARCH:
			if(num>max_fat_arch()) {printf("out of bound.\n"); return NULL;}
			archoffset=sizeof(struct fat_header);
			archentsize=sizeof(struct fat_arch);
			for(n=0;n<num;n++)
			{
				archoffset=archoffset+archentsize;
			}
			arch=(struct fat_arch*)(fc_ptr->faddr+archoffset);
			if((strcmp(varname,"cputype"))==0) {var->type=VART_NUM;var->val.num=get_data32(arch->cputype);}
			if((strcmp(varname,"cpusubtype"))==0) {var->type=VART_NUM;var->val.num=get_data32(arch->cpusubtype);}
			if((strcmp(varname,"offset"))==0) {var->type=VART_NUM;var->val.num=get_data32(arch->offset);}
			if((strcmp(varname,"size"))==0) {var->type=VART_NUM;var->val.num=get_data32(arch->size);}
			if((strcmp(varname,"align"))==0) {var->type=VART_NUM;var->val.num=get_data32(arch->align);}
			break;
		default:
			
			break;
	}
	return var;
}
void fatmacho_set_s_val(struct _structvar *ptr,struct _var *val)
{
	int sect,num;
	char varname[VARNAME_LEN];
	//struct _var *var;
	if(!ptr) return;
	strcpy(varname,"");
	num=ptr->num;
	if((strcmp(ptr->name,"fat"))==0) sect=SECT_FATMACHO;
	if((strcmp(ptr->name,"arch"))==0) sect=SECT_FATARCH;
	if(ptr->next) {strncpy(varname,ptr->next->name,VARNAME_LEN);if(ptr->next->num!=-1) {printf("invalid section.\n");return;}}
	set_fatmacho_var(sect,num,varname,val);
}
void set_fatmacho_var(int sect,int num,char *varname,struct _var *val)
{
	struct fat_header *fat;
	struct fat_arch *arch;
	int n,archentsize;
	off_t archoffset;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	if(!val) return;
	switch(sect) {
		case SECT_FATMACHO:
			if(num!=-1) {printf("wrong section.\n"); return;}
			fat=(struct fat_header*) fc_ptr->faddr;
			if((strcmp(varname,"magic"))==0) {if(val->type==VART_NUM){fat->magic=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"nfat_arch"))==0) {if(val->type==VART_NUM){fat->nfat_arch=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		case SECT_FATARCH:
			if(num>max_fat_arch()) {printf("out of bound.\n"); return;}
			archoffset=sizeof(struct fat_header);
			archentsize=sizeof(struct fat_arch);
			for(n=0;n<num;n++)
			{
				archoffset=archoffset+archentsize;
			}
			arch=(struct fat_arch*)(fc_ptr->faddr+archoffset);
			if((strcmp(varname,"cputype"))==0) {if(val->type==VART_NUM){arch->cputype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"cpusubtype"))==0) {if(val->type==VART_NUM){arch->cpusubtype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"offset"))==0) {if(val->type==VART_NUM){arch->offset=get_data32(val->val.num);}else{printf("wrong var type.\n");};}
			if((strcmp(varname,"size"))==0) {if(val->type==VART_NUM){arch->size=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			if((strcmp(varname,"align"))==0) {if(val->type==VART_NUM){arch->align=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
			break;
		default:
			
			break;
	}
}
int max_lc(int arch)
{
	int x;
	off_t arch_offs;
	struct mach_header *mac32;
	struct mach_header_64 *mac64;
	if(!fc_ptr) return 0;
	arch_offs=get_arch_off(arch);
	switch(fc_ptr->file_endian) {
		case bit64:
			mac64=(struct mach_header_64*) (fc_ptr->faddr+arch_offs);
			x=get_data32(mac64->ncmds)-1;
			break;
		case bit32:
		default:
			mac32=(struct mach_header*) (fc_ptr->faddr+arch_offs);
			x=get_data32(mac32->ncmds)-1;
			break;
	}
	return x;
}
int max_sect(int arch,int lc)
{
	int x,n;
	off_t offs;
	//struct mach_header *mac32;
	//struct mach_header_64 *mac64;
	struct load_command *lcmd;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	if(!fc_ptr) return 0;
	if(lc>max_lc(arch)) {printf("out of lc bound.\n"); return 0;}
	offs=get_arch_off(arch);
	switch(fc_ptr->file_bit_class) {
		case bit64:
			offs=offs+sizeof(struct mach_header_64);
			for(n=0;n<lc;n++)
			{
				lcmd=(struct load_command*) (offs+fc_ptr->faddr);
				offs=offs+get_data32(lcmd->cmdsize);
			}
			seg64=(struct segment_command_64*)(fc_ptr->faddr+offs);
			if(get_data32(seg64->cmd)==LC_SEGMENT_64) x=get_data32(seg64->nsects)-1; else x=0;
			break;
		case bit32:
		default:
			offs=offs+sizeof(struct mach_header);
			for(n=0;n<lc;n++)
			{
				lcmd=(struct load_command*) (offs+fc_ptr->faddr);
				offs=offs+get_data32(lcmd->cmdsize);
			}
			seg32=(struct segment_command*)(fc_ptr->faddr+offs);
			if(get_data32(seg32->cmd)==LC_SEGMENT) x=get_data32(seg32->nsects)-1; else x=0;
			break;
	}
	return x;
}
struct _var* macho_get_s_val(struct _structvar *ptr)
{
	int lc_sect,lc,arch,sect;
	char varname[VARNAME_LEN];
	struct _var *var;
	if(!ptr) return NULL;
	arch=-1;
	strcpy(varname,"");
	if(!ptr->next) return NULL;
	if((ptr->next)&&(!ptr->next->next))
	{
		if((strcmp(ptr->name,"mac"))==0) {sect=SECT_MAC;lc=-1;lc_sect=-1;}
		if((strcmp(ptr->name,"lc"))==0) {sect=SECT_LC;lc=ptr->num;lc_sect=-1;}
		arch=-1;
		strncpy(varname,ptr->next->name,VARNAME_LEN);
	}
	if((ptr->next)&&(ptr->next->next))
	{
		if(ptr->next->next->next) return NULL;
		if(((strcmp(ptr->next->name,"sect"))==0)&&(((strcmp(ptr->name,"lc"))==0)))
		{
			sect=SECT_MACSECT;
			lc=ptr->num;
			lc_sect=ptr->next->num;
			strncpy(varname,ptr->next->next->name,VARNAME_LEN);
		}
	}
	var=get_macho_var(sect,arch,lc,lc_sect,varname);
	return var;
}
struct _var* get_macho_var(int sect,int arch,int lc,int lc_sect,char *varname)
{
	off_t offs;
	int x,n;
	struct _var *var;
	struct mach_header *mac32;
	struct mach_header_64 *mac64;
	struct load_command *lcmd;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	struct section *sect32;
	struct section_64 *sect64;
	//struct uuid_command *uuid;
	////struct lc_str *lcstr;
	//struct twolevel_hints_command *twolevel;
	//struct dylib_command *dylib;
	//struct dylinker_command *dylinker;
	//struct symtab_command *symtab;
	//struct dysymtab_command *dysymtab;
	//struct thread_command *thread;

	if(!fc_ptr) return NULL;
	var=createtmpvar();
	if(!var) return NULL;
	if(lc>max_lc(arch)) {printf("out of lc bound.\n"); return NULL;}
	if(lc_sect>max_sect(arch,lc)) {printf("out of sect bound.\n"); return NULL;}
	offs=get_arch_off(arch);
	switch(sect) {
		case SECT_MAC:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					mac64=(struct mach_header_64*) (fc_ptr->faddr+offs);
					if((strcmp(varname,"magic"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->magic);}
					if((strcmp(varname,"cputype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->cputype);}
					if((strcmp(varname,"cpusubtype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->cpusubtype);}
					if((strcmp(varname,"filetype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->filetype);}
					if((strcmp(varname,"ncmds"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->ncmds);}
					if((strcmp(varname,"sizeofcmds"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->sizeofcmds);}
					if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->flags);}
					if((strcmp(varname,"reserved"))==0){var->type=VART_NUM;var->val.num=get_data32(mac64->reserved);}
				break;
				case bit32:
				default:
					mac32=(struct mach_header*) (fc_ptr->faddr+offs);
					if((strcmp(varname,"magic"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->magic);}
					if((strcmp(varname,"cputype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->cputype);}
					if((strcmp(varname,"cpusubtype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->cpusubtype);}
					if((strcmp(varname,"filetype"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->filetype);}
					if((strcmp(varname,"ncmds"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->ncmds);}
					if((strcmp(varname,"sizeofcmds"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->sizeofcmds);}
					if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(mac32->flags);}
				break;
			}
			break;
		case SECT_LC:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=(off_t)(offs+sizeof(struct mach_header_64));
					offs=(off_t)(offs+fc_ptr->faddr);
					lcmd=(struct load_command*)offs;
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*)offs;
						offs=(off_t)(offs+get_data32(lcmd->cmdsize));
					}
					switch(get_data32(lcmd->cmd)){
						case LC_SEGMENT_64:
							seg64=(struct segment_command_64*)offs;
							if((strcmp(varname,"cmd"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->cmd);}
							if((strcmp(varname,"cmdsize"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->cmdsize);}
							if((strcmp(varname,"segname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);strncpy(var->val.s,seg64->segname,16);}
							if((strcmp(varname,"vmaddr"))==0){var->type=VART_NUM;var->val.num=get_data64(seg64->vmaddr);}
							if((strcmp(varname,"vmsize"))==0){var->type=VART_NUM;var->val.num=get_data64(seg64->vmsize);}
							if((strcmp(varname,"fileoff"))==0){var->type=VART_NUM;var->val.num=get_data64(seg64->fileoff);}
							if((strcmp(varname,"filesize"))==0){var->type=VART_NUM;var->val.num=get_data64(seg64->filesize);}
							if((strcmp(varname,"maxprot"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->maxprot);}
							if((strcmp(varname,"initprot"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->initprot);}
							if((strcmp(varname,"nsects"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->nsects);}
							if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(seg64->flags);}
							break;
						case LC_LOAD_DYLIB:
						case LC_ID_DYLIB:
							//break;
						case LC_LOAD_DYLINKER:
						case LC_ID_DYLINKER:
							//break;
						case LC_SYMTAB:
							//break;
						case LC_DYSYMTAB:
							//break;
						case LC_UNIXTHREAD:
							//break;
						default:
							printf("bazingaaa\n");
							break;	
					}
					break;
				case bit32:
				default:
					offs=(off_t)(offs+sizeof(struct mach_header));
					offs=(off_t)(offs+fc_ptr->faddr);
					lcmd=(struct load_command*)offs;
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*)offs;
						offs=(off_t)(offs+get_data32(lcmd->cmdsize));
					}
					switch(get_data32(lcmd->cmd)){
						case LC_SEGMENT:
							seg32=(struct segment_command*)offs;
							if((strcmp(varname,"cmd"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->cmd);}
							if((strcmp(varname,"cmdsize"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->cmdsize);}
							if((strcmp(varname,"segname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);strncpy(var->val.s,seg32->segname,16);}
							if((strcmp(varname,"vmaddr"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->vmaddr);}
							if((strcmp(varname,"vmsize"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->vmsize);}
							if((strcmp(varname,"fileoff"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->fileoff);}
							if((strcmp(varname,"filesize"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->filesize);}
							if((strcmp(varname,"maxprot"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->maxprot);}
							if((strcmp(varname,"initprot"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->initprot);}
							if((strcmp(varname,"nsects"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->nsects);}
							if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(seg32->flags);}
							break;
						case LC_LOAD_DYLIB:
						case LC_ID_DYLIB:
							//break;
						case LC_LOAD_DYLINKER:
						case LC_ID_DYLINKER:
							//break;
						case LC_SYMTAB:
							//break;
						case LC_DYSYMTAB:
							//break;
						case LC_UNIXTHREAD:
							//break;
						default:
							printf("bazingaaa\n");
							break;	
					}
					break;
			}
			break;
		case SECT_MACSECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=offs+sizeof(struct mach_header_64);
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*) (offs+fc_ptr->faddr);
						x=get_data32(lcmd->cmdsize);
						offs=offs+x;
					}
					seg64=(struct segment_command_64*)(offs+fc_ptr->faddr);
					if(seg64->nsects==0) {printf("no section in this load command.\n"); return NULL;}
					offs=offs+sizeof(struct segment_command_64);
					for(n=0;n<lc_sect;n++)
					{
						offs=offs+sizeof(struct section_64);
					}
					sect64=(struct section_64*)(offs+fc_ptr->faddr);
					if((strcmp(varname,"sectname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);if(var->val.s) strncpy(var->val.s,sect64->sectname,16);}
					if((strcmp(varname,"segname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);if(var->val.s) strncpy(var->val.s,sect64->segname,16);}
					if((strcmp(varname,"addr"))==0){var->type=VART_NUM;var->val.num=get_data64(sect64->addr);}
					if((strcmp(varname,"size"))==0){var->type=VART_NUM;var->val.num=get_data64(sect64->size);}
					if((strcmp(varname,"offset"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->offset);}
					if((strcmp(varname,"align"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->align);}
					if((strcmp(varname,"reloff"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->reloff);}
					if((strcmp(varname,"nreloc"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->nreloc);}
					if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->flags);}
					if((strcmp(varname,"reserved1"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->reserved1);}
					if((strcmp(varname,"reserved2"))==0){var->type=VART_NUM;var->val.num=get_data32(sect64->reserved2);}
					break;
				case bit32:
				default:
					offs=offs+sizeof(struct mach_header);
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*) (offs+fc_ptr->faddr);
						x=get_data32(lcmd->cmdsize);
						offs=offs+x;
					}
					seg32=(struct segment_command*)(offs+fc_ptr->faddr);
					if(seg32->nsects==0) {printf("no section in this load command.\n"); return NULL;}
					offs=offs+sizeof(struct segment_command);
					for(n=0;n<lc_sect;n++)
					{
						offs=offs+sizeof(struct section);
					}
					sect32=(struct section*)(offs+fc_ptr->faddr);
					if((strcmp(varname,"sectname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);if(var->val.s) strncpy(var->val.s,sect32->sectname,16);}
					if((strcmp(varname,"segname"))==0){var->type=VART_WORD;var->val.s=(char*)malloc(20);if(var->val.s) strncpy(var->val.s,sect32->segname,16);}
					if((strcmp(varname,"addr"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->addr);}
					if((strcmp(varname,"size"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->size);}
					if((strcmp(varname,"offset"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->offset);}
					if((strcmp(varname,"align"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->align);}
					if((strcmp(varname,"reloff"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->reloff);}
					if((strcmp(varname,"nreloc"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->nreloc);}
					if((strcmp(varname,"flags"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->flags);}
					if((strcmp(varname,"reserved1"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->reserved1);}
					if((strcmp(varname,"reserved2"))==0){var->type=VART_NUM;var->val.num=get_data32(sect32->reserved2);}
					break;
				}
			break;
		default:
			
			break;
	}
	return var;
}
off_t get_arch_off(int arch)
{
	int bit_endian,n;
	off_t offs;
	struct fat_arch *p_arch;
	if(arch==-1) return 0;
	if(!fc_ptr) return 0;
	if(arch>max_fat_arch()) {printf("out of arch bound.\n"); return 0;}
	bit_endian=fc_ptr->file_endian;
	fc_ptr->file_endian=big_endian;
	offs=sizeof(struct fat_header);
	for(n=0;n<arch;n++)
	{
		offs=offs+sizeof(struct fat_arch);
	}
	p_arch=(struct fat_arch*)(fc_ptr->faddr+offs);
	offs=(off_t)get_data32(p_arch->offset);
	fc_ptr->file_endian=bit_endian;
	return offs;
}
void macho_set_s_val(struct _structvar *ptr,struct _var *val)
{
	int lc_sect,lc,arch,sect;
	char varname[VARNAME_LEN];
	//struct _var *var;
	if(!ptr) return;
	arch=-1;
	strcpy(varname,"");
	if(!ptr->next) return;
	if((ptr->next)&&(!ptr->next->next))
	{
		if((strcmp(ptr->name,"mac"))==0) {sect=SECT_MAC;lc=-1;lc_sect=-1;}
		if((strcmp(ptr->name,"lc"))==0) {sect=SECT_LC;lc=ptr->num;lc_sect=-1;}
		arch=-1;
		strncpy(varname,ptr->next->name,VARNAME_LEN);
	}
	if((ptr->next)&&(ptr->next->next))
	{
		if(ptr->next->next->next) return;
		if(((strcmp(ptr->next->name,"sect"))==0)&&(((strcmp(ptr->name,"lc"))==0)))
		{
			sect=SECT_MACSECT;
			lc=ptr->num;
			lc_sect=ptr->next->num;
			strncpy(varname,ptr->next->next->name,VARNAME_LEN);
		}
	}
	set_macho_var(sect,arch,lc,lc_sect,varname,val);
}
void set_macho_var(int sect,int arch,int lc,int lc_sect,char *varname,struct _var *val)
{
	off_t offs;
	int x,n;
	//struct _var *var;
	struct mach_header *mac32;
	struct mach_header_64 *mac64;
	struct load_command *lcmd;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	struct section *sect32;
	struct section_64 *sect64;
	//struct uuid_command *uuid;
	////struct lc_str *lcstr;
	//struct twolevel_hints_command *twolevel;
	//struct dylib_command *dylib;
	//struct dylinker_command *dylinker;
	//struct symtab_command *symtab;
	//struct dysymtab_command *dysymtab;
	//struct thread_command *thread;

	if(!fc_ptr) return;
	if(lc>max_lc(arch)) {printf("out of lc bound.\n"); return;}
	if(lc_sect>max_sect(arch,lc)) {printf("out of sect bound.\n"); return;}
	offs=get_arch_off(arch);
	switch(sect) {
		case SECT_MAC:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					mac64=(struct mach_header_64*) (fc_ptr->faddr+offs);
					if((strcmp(varname,"magic"))==0) {if(val->type==VART_NUM){mac64->magic=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"cputype"))==0) {if(val->type==VART_NUM){mac64->cputype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"cpusubtype"))==0) {if(val->type==VART_NUM){mac64->cpusubtype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"filetype"))==0) {if(val->type==VART_NUM){mac64->filetype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"ncmds"))==0) {if(val->type==VART_NUM){mac64->ncmds=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sizeofcmds"))==0) {if(val->type==VART_NUM){mac64->sizeofcmds=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){mac64->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reserved"))==0) {if(val->type==VART_NUM){mac64->reserved=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
				break;
				case bit32:
				default:
					mac32=(struct mach_header*) (fc_ptr->faddr+offs);
					if((strcmp(varname,"magic"))==0) {if(val->type==VART_NUM){mac32->magic=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"cputype"))==0) {if(val->type==VART_NUM){mac32->cputype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"cpusubtype"))==0) {if(val->type==VART_NUM){mac32->cpusubtype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"filetype"))==0) {if(val->type==VART_NUM){mac32->filetype=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"ncmds"))==0) {if(val->type==VART_NUM){mac32->ncmds=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"sizeofcmds"))==0) {if(val->type==VART_NUM){mac32->sizeofcmds=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){mac32->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
				break;
			}
			break;
		case SECT_LC:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=(off_t)(offs+sizeof(struct mach_header_64));
					offs=(off_t)(offs+fc_ptr->faddr);
					lcmd=(struct load_command*)offs;
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*)offs;
						offs=(off_t)(offs+get_data32(lcmd->cmdsize));
					}
					switch(get_data32(lcmd->cmd)){
						case LC_SEGMENT_64:
							seg64=(struct segment_command_64*)offs;
							if((strcmp(varname,"cmd"))==0) {if(val->type==VART_NUM){seg64->cmd=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"cmdsize"))==0) {if(val->type==VART_NUM){seg64->cmdsize=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"segname"))==0) {if(val->type==VART_WORD){strncpy(seg64->segname,val->val.s,16);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"vmaddr"))==0) {if(val->type==VART_NUM){seg64->vmaddr=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"vmsize"))==0) {if(val->type==VART_NUM){seg64->vmsize=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"fileoff"))==0) {if(val->type==VART_NUM){seg64->fileoff=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"filesize"))==0) {if(val->type==VART_NUM){seg64->filesize=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"maxprot"))==0) {if(val->type==VART_NUM){seg64->maxprot=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"initprot"))==0) {if(val->type==VART_NUM){seg64->initprot=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"nsects"))==0) {if(val->type==VART_NUM){seg64->nsects=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){seg64->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							break;
						case LC_LOAD_DYLIB:
						case LC_ID_DYLIB:
							//break;
						case LC_LOAD_DYLINKER:
						case LC_ID_DYLINKER:
							//break;
						case LC_SYMTAB:
							//break;
						case LC_DYSYMTAB:
							//break;
						case LC_UNIXTHREAD:
							//break;
						default:
							printf("bazingaaa\n");
							break;	
					}
					break;
				case bit32:
				default:
					offs=(off_t)(offs+sizeof(struct mach_header));
					offs=(off_t)(offs+fc_ptr->faddr);
					lcmd=(struct load_command*)offs;
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*)offs;
						offs=(off_t)(offs+get_data32(lcmd->cmdsize));
					}
					switch(get_data32(lcmd->cmd)){
						case LC_SEGMENT:
							seg32=(struct segment_command*)offs;
							if((strcmp(varname,"cmd"))==0) {if(val->type==VART_NUM){seg32->cmd=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"cmdsize"))==0) {if(val->type==VART_NUM){seg32->cmdsize=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"segname"))==0) {if(val->type==VART_WORD){strncpy(seg32->segname,val->val.s,16);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"vmaddr"))==0) {if(val->type==VART_NUM){seg32->vmaddr=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"vmsize"))==0) {if(val->type==VART_NUM){seg32->vmsize=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"fileoff"))==0) {if(val->type==VART_NUM){seg32->fileoff=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"filesize"))==0) {if(val->type==VART_NUM){seg32->filesize=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"maxprot"))==0) {if(val->type==VART_NUM){seg32->maxprot=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"initprot"))==0) {if(val->type==VART_NUM){seg32->initprot=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"nsects"))==0) {if(val->type==VART_NUM){seg32->nsects=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){seg32->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
							break;
						case LC_LOAD_DYLIB:
						case LC_ID_DYLIB:
							//break;
						case LC_LOAD_DYLINKER:
						case LC_ID_DYLINKER:
							//break;
						case LC_SYMTAB:
							//break;
						case LC_DYSYMTAB:
							//break;
						case LC_UNIXTHREAD:
							//break;
						default:
							printf("bazingaaa\n");
							break;	
					}
					break;
			}
			break;
		case SECT_MACSECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=offs+sizeof(struct mach_header_64);
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*) (offs+fc_ptr->faddr);
						x=get_data32(lcmd->cmdsize);
						offs=offs+x;
					}
					seg64=(struct segment_command_64*)(offs+fc_ptr->faddr);
					if(seg64->nsects==0) {printf("no section in this load command.\n"); return;}
					offs=offs+sizeof(struct segment_command_64);
					for(n=0;n<lc_sect;n++)
					{
						offs=offs+sizeof(struct section_64);
					}
					sect64=(struct section_64*)(offs+fc_ptr->faddr);
					if((strcmp(varname,"sectname"))==0) {if(val->type==VART_WORD){strncpy(sect64->sectname,val->val.s,16);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"segname"))==0) {if(val->type==VART_WORD){strncpy(sect64->segname,val->val.s,16);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"addr"))==0) {if(val->type==VART_NUM){sect64->addr=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"size"))==0) {if(val->type==VART_NUM){sect64->size=get_data64(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"offset"))==0) {if(val->type==VART_NUM){sect64->offset=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"align"))==0) {if(val->type==VART_NUM){sect64->align=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reloff"))==0) {if(val->type==VART_NUM){sect64->reloff=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"nreloc"))==0) {if(val->type==VART_NUM){sect64->nreloc=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){sect64->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reserved1"))==0) {if(val->type==VART_NUM){sect64->reserved1=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reserved2"))==0) {if(val->type==VART_NUM){sect64->reserved2=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				case bit32:
				default:
					offs=offs+sizeof(struct mach_header);
					for(n=0;n<lc;n++)
					{
						lcmd=(struct load_command*) (offs+fc_ptr->faddr);
						x=get_data32(lcmd->cmdsize);
						offs=offs+x;
					}
					seg32=(struct segment_command*)(offs+fc_ptr->faddr);
					if(seg32->nsects==0) {printf("no section in this load command.\n"); return;}
					offs=offs+sizeof(struct segment_command);
					for(n=0;n<lc_sect;n++)
					{
						offs=offs+sizeof(struct section);
					}
					sect32=(struct section*)(offs+fc_ptr->faddr);
					if((strcmp(varname,"sectname"))==0) {if(val->type==VART_WORD){strncpy(sect32->sectname,val->val.s,16);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"segname"))==0) {if(val->type==VART_WORD){strncpy(sect32->segname,val->val.s,16);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"addr"))==0) {if(val->type==VART_NUM){sect32->addr=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"size"))==0) {if(val->type==VART_NUM){sect32->size=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"offset"))==0) {if(val->type==VART_NUM){sect32->offset=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"align"))==0) {if(val->type==VART_NUM){sect32->align=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reloff"))==0) {if(val->type==VART_NUM){sect32->reloff=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"nreloc"))==0) {if(val->type==VART_NUM){sect32->nreloc=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"flags"))==0) {if(val->type==VART_NUM){sect32->flags=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reserved1"))==0) {if(val->type==VART_NUM){sect32->reserved1=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					if((strcmp(varname,"reserved2"))==0) {if(val->type==VART_NUM){sect32->reserved2=get_data32(val->val.num);}else{printf("wrong var type.\n");}}
					break;
				}
			break;
		default:
			
			break;
	}
}
off_t get_macho_offset(struct _structvar *ptr,int endoffset)
{
	int lc_sect,lc,arch,sect;
	off_t offs;
	if(!ptr) return 0;
	offs=0;
	arch=-1;
	if((strcmp(ptr->name,"mac"))==0) sect=SECT_MAC;
	if((strcmp(ptr->name,"lc"))==0) sect=SECT_LC;
	if(ptr->next)
	{
		if(((strcmp(ptr->next->name,"sect"))==0)&&(((strcmp(ptr->name,"lc"))==0)))
		{
			sect=SECT_MACSECT;
		}
	}
	switch(sect){
		case SECT_MAC:
			if((ptr->next)||(ptr->num!=-1)) {printf("invalid section.\n"); return 0;}
			lc=-1;
			lc_sect=-1;
			break;
		case SECT_LC:
			if((ptr->next)||(ptr->num==-1)) {printf("invalid section.\n"); return 0;}
			lc=ptr->num;
			lc_sect=-1;
			break;
		case SECT_MACSECT:
			if((ptr->next->next)||(ptr->next->num==-1)) {printf("invalid section.\n"); return 0;}
			lc=ptr->num;
			lc_sect=ptr->next->num;
			break;
		default:
			return 0;
			break;
	}
	offs=get_macho_offset2(sect,arch,lc,lc_sect,endoffset);
	return offs;
}
off_t get_macho_offset2(int sect,int arch,int lc_num,int sec_num,int endoffset)
{
	off_t offs;
	int x,n;
	//struct mach_header *mac32;
	//struct mach_header_64 *mac64;
	struct load_command *lcmd;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	struct section *sect32;
	struct section_64 *sect64;

	if(!fc_ptr) return 0;
	if(lc_num>max_lc(arch)) {printf("out of lc bound.\n"); return 0;}
	if(sec_num>max_sect(arch,lc_num)) {printf("out of sect bound.\n"); return 0;}
	offs=get_arch_off(arch);
	switch(sect) {
		case SECT_MAC:
			if(endoffset){
			switch(fc_ptr->file_bit_class) {
				case bit64:
					//mac64=(struct mach_header_64*) (fc_ptr->faddr+offs);
					offs=offs+sizeof(struct mach_header_64);
				break;
				case bit32:
				default:
					//mac32=(struct mach_header*) (fc_ptr->faddr+offs);
					offs=offs+sizeof(struct mach_header);
				break;
			}
			}
			break;
		case SECT_LC:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=(off_t)(offs+sizeof(struct mach_header_64));
					lcmd=(struct load_command*)(offs+fc_ptr->faddr);
					x=get_data32(lcmd->cmdsize);
					for(n=0;n<lc_num;n++)
					{
						lcmd=(struct load_command*)((char*)lcmd+x);
						offs=offs+x;
						x=get_data32(lcmd->cmdsize);
					}
					if(endoffset) offs=offs+x;
					break;
				case bit32:
				default:
					offs=(off_t)(offs+sizeof(struct mach_header));
					lcmd=(struct load_command*)(offs+fc_ptr->faddr);
					x=get_data32(lcmd->cmdsize);
					for(n=0;n<lc_num;n++)
					{
						lcmd=(struct load_command*)((char*)lcmd+x);
						offs=offs+x;
						x=get_data32(lcmd->cmdsize);
					}
					if(endoffset) offs=offs+x;
					break;
			}
			break;
		case SECT_MACSECT:
			switch(fc_ptr->file_bit_class) {
				case bit64:
					offs=(off_t)(offs+sizeof(struct mach_header_64));
					lcmd=(struct load_command*)(offs+fc_ptr->faddr);
					x=get_data32(lcmd->cmdsize);
					for(n=0;n<lc_num;n++)
					{
						lcmd=(struct load_command*)((char*)lcmd+x);
						offs=offs+x;
						x=get_data32(lcmd->cmdsize);
					}
					x=get_data32(lcmd->cmd);
					if(x!=LC_SEGMENT_64) {printf("bad section.\n"); return 0;}
					seg64=(struct segment_command_64*)lcmd;
					if(seg64->nsects==0) {printf("no sections.\n"); return 0;}
					offs=offs+sizeof(struct segment_command_64);
					sect64=(struct section_64*)(offs+fc_ptr->faddr);
					x=sizeof(struct section_64);
					for(n=0;n<sec_num;n++)
					{
						sect64=(struct section_64*)((char*)sect64+x);
						offs=offs+x;
						x=sizeof(struct section_64);
					}
					if(endoffset) offs=offs+x;
					break;
				case bit32:
				default:
					offs=(off_t)(offs+sizeof(struct mach_header));
					lcmd=(struct load_command*)(offs+fc_ptr->faddr);
					x=get_data32(lcmd->cmdsize);
					for(n=0;n<lc_num;n++)
					{
						lcmd=(struct load_command*)((char*)lcmd+x);
						offs=offs+x;
						x=get_data32(lcmd->cmdsize);
					}
					x=get_data32(lcmd->cmd);
					if(x!=LC_SEGMENT) {printf("bad section.\n"); return 0;}
					seg32=(struct segment_command*)lcmd;
					if(seg32->nsects==0) {printf("no sections.\n"); return 0;}
					offs=offs+sizeof(struct segment_command);
					sect32=(struct section*)(offs+fc_ptr->faddr);
					x=sizeof(struct section);
					for(n=0;n<sec_num;n++)
					{
						sect32=(struct section*)((char*)sect32+x);
						offs=offs+x;
						x=sizeof(struct section);
					}
					if(endoffset) offs=offs+x;
					break;
			}
			break;
		default:
			
			break;
	}
	return offs;
}
off_t get_fatmacho_offset(struct _structvar *ptr,int endoffset)
{
	int sect,num;
	off_t offs;
	offs=0;
	if(!ptr) return 0;
	num=ptr->num;
	if((strcmp(ptr->name,"fat"))==0) sect=SECT_FATMACHO;
	if((strcmp(ptr->name,"arch"))==0) sect=SECT_FATARCH;
	if(ptr->next) {printf("invalid section.\n"); return 0;}
	switch(sect){
		case SECT_FATMACHO:
			if(ptr->num!=-1) {printf("invalid section.\n"); return 0;}
			break;
		case SECT_FATARCH:
			if(ptr->num==-1) {printf("invalid section.\n"); return 0;}
			break;
		default:
			offs=0;
			break;
	}
	offs=get_fatmacho_offset2(sect,num,endoffset);
	return offs;
}
off_t get_fatmacho_offset2(int sect,int num,int endoffset)
{
	//struct fat_header *fat;
	//struct fat_arch *arch;
	int n,archentsize;
	off_t archoffset,offs;
	if(!fc_ptr) {printf("no file opened.\n"); return 0;}
	switch(sect) {
		case SECT_FATMACHO:
			if(num!=-1) {printf("wrong section.\n"); return 0;}
			//fat=(struct fat_header*) fc_ptr->faddr;
			offs=0;
			if(endoffset) {offs=offs+sizeof(struct fat_header);}
			break;
		case SECT_FATARCH:
			if(num>max_fat_arch()) {printf("out of bound.\n"); return 0;}
			archoffset=sizeof(struct fat_header);
			archentsize=sizeof(struct fat_arch);
			for(n=0;n<num;n++)
			{
				archoffset=archoffset+archentsize;
			}
			//arch=(struct fat_arch*)(fc_ptr->faddr+archoffset);
			offs=archoffset;
			if(endoffset) {offs=offs+sizeof(struct fat_arch);}
			break;
		default:
			offs=0;
			break;
	}
	return offs;
}
void create_fatmacho_hdr(int sect,off_t offs,int grow,int update)
{
	if(!fc_ptr) return;
	switch(sect) {
		case SECT_FATMACHO:
			inject_byte(0,offs,sizeof(struct fat_arch),grow);
			break;
		case SECT_FATARCH:
			inject_byte(0,offs,sizeof(struct fat_arch),grow);
			break;
		default:
			
			break;
	}
}
void create_macho_hdr(int sect,off_t offs,int grow,int update)
{
	if(!fc_ptr) return;
	switch(sect) {
		case SECT_MAC:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(struct mach_header_64),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(struct mach_header),grow);
					break;
			}
			break;
		case SECT_LC:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(struct segment_command_64),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(struct segment_command),grow);
					break;
			}
			if(update)
			{
				printf("update....still coding.\n");
			}
			break;
		case SECT_MACSECT:
			switch(fc_ptr->file_bit_class){
				case bit64:
					inject_byte(0,offs,sizeof(struct section_64),grow);
					break;
				case bit32:
				default:
					inject_byte(0,offs,sizeof(struct section),grow);
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
