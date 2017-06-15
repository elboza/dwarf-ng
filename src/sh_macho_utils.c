/*
 sh_macho_utils.c : dwarf's shell Mach-O utils functions file.

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
#include<string.h>
#include "libdwarf.h"
#include"sh_macho_utils.h"
#include"dw_readline_completion.h"
#include"output.h"
//#include"../../config.h"

void print_fatmacho_s_var(struct _fmt *fmt,struct _structvar *ptr)
{
	int sect=SECT_NULL;
	struct _var *var;
	if((strcmp(ptr->name,"fat"))==0) sect=SECT_FATMACHO;
	if((strcmp(ptr->name,"arch"))==0) sect=SECT_FATARCH;
	if(!ptr->next) {
		switch(sect) {
			case SECT_FATMACHO:
				if(ptr->num!=-1) {printf("invalid section.\n"); return;}
				print_fatmacho_hdr();
				break;
			case SECT_FATARCH:
				if(ptr->num==-1) print_fatarchlist(); else print_fatarch(ptr->num);
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(ptr->next->num!=-1) {printf("invalid section.\n"); return;}
		var=get_fatmacho_var(sect,ptr->num,ptr->next->name);
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
void print_fatmacho_hdr()
{
	struct fat_header *fat;
	fat=(struct fat_header*) fc_ptr->faddr;
	printf("magic: 0x%x (%d)\n",get_data32(fat->magic),get_data32(fat->magic));
	printf("nfat_arch: 0x%x (%d)\n",get_data32(fat->nfat_arch),get_data32(fat->nfat_arch));
}
void print_fatarch(int num)
{
	//struct fat_header *fat;
	struct fat_arch *arch;
	int archentsize,n;
	off_t archoffset;
	if(num>max_fat_arch()) {printf("out of bound.\n"); return;}
	archoffset=sizeof(struct fat_header);
	archentsize=sizeof(struct fat_arch);
	for(n=0;n<num;n++)
	{
		archoffset=archoffset+archentsize;
	}
	arch=(struct fat_arch*)(fc_ptr->faddr+archoffset);
	printf("cputype: 0x%x (%d) [%s]\n",get_data32(arch->cputype),get_data32(arch->cputype),decode_file_cpu(get_data32(arch->cputype)));
	printf("cpusubtype: 0x%x (%d)\n",get_data32(arch->cpusubtype),get_data32(arch->cpusubtype));
	printf("offset: 0x%x (%d)\n",get_data32(arch->offset),get_data32(arch->offset));
	printf("size: 0x%x (%d)\n",get_data32(arch->size),get_data32(arch->size));
	printf("align: 0x%x (%d)\n",get_data32(arch->align),get_data32(arch->align));
}
void print_fatarchlist()
{
	//struct fat_header *fat;
	struct fat_arch *arch;
	int archentsize,n;
	off_t archoffset;
	archoffset=sizeof(struct fat_header);
	archentsize=sizeof(struct fat_arch);
	for(n=0;n<=max_fat_arch();n++)
	{
		arch=(struct fat_arch*)(fc_ptr->faddr+archoffset);
		printf("arch[%d]: cpu %s (0x%x), offs 0x%x, size 0x%x\n",n,decode_file_cpu(get_data32(arch->cputype)),get_data32(arch->cputype),get_data32(arch->offset),get_data32(arch->size));
		//print more info.....
		archoffset=archoffset+archentsize;
	}
}
void print_mac_hdr(int arch)
{
	struct mach_header *mac32;
	struct mach_header_64 *mac64;
	off_t arch_offs;
	//int n,x;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	arch_offs=get_arch_off(arch);
	switch(fc_ptr->file_bit_class) {
		case bit64:
			mac64=(struct mach_header_64*) (fc_ptr->faddr+arch_offs);
			printf("magic: 0x%x (%d)\n",get_data32(mac64->magic),get_data32(mac64->magic));
			printf("cputype: 0x%x (%d)\n",get_data32(mac64->cputype),get_data32(mac64->cputype));
			printf("cpusubtype: 0x%x (%d)\n",get_data32(mac64->cpusubtype),get_data32(mac64->cpusubtype));
			printf("filetype: 0x%x (%d)\n",get_data32(mac64->filetype),get_data32(mac64->filetype));
			printf("ncmds: 0x%x (%d)\n",get_data32(mac64->ncmds),get_data32(mac64->ncmds));
			printf("sizeofcmds: 0x%x (%d)\n",get_data32(mac64->sizeofcmds),get_data32(mac64->sizeofcmds));
			printf("flags: 0x%x (%d)\n",get_data32(mac64->flags),get_data32(mac64->flags));
			printf("reserved: 0x%x (%d)\n",get_data32(mac64->reserved),get_data32(mac64->reserved));
			break;
		case bit32:
		default:
			mac32=(struct mach_header*) (fc_ptr->faddr+arch_offs);
			printf("magic: 0x%x (%d)\n",get_data32(mac32->magic),get_data32(mac32->magic));
			printf("cputype: 0x%x (%d)\n",get_data32(mac32->cputype),get_data32(mac32->cputype));
			printf("cpusubtype: 0x%x (%d)\n",get_data32(mac32->cpusubtype),get_data32(mac32->cpusubtype));
			printf("filetype: 0x%x (%d)\n",get_data32(mac32->filetype),get_data32(mac32->filetype));
			printf("ncmds: 0x%x (%d)\n",get_data32(mac32->ncmds),get_data32(mac32->ncmds));
			printf("sizeofcmds: 0x%x (%d)\n",get_data32(mac32->sizeofcmds),get_data32(mac32->sizeofcmds));
			printf("flags: 0x%x (%d)\n",get_data32(mac32->flags),get_data32(mac32->flags));
			break;
	}
}
void print_lclist(int arch)
{
	int n;
	struct load_command *lc;
	off_t offs;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	switch(fc_ptr->file_bit_class) {
		case bit64:
			offs=(off_t)get_arch_off(-1);
			offs=(off_t)(offs+sizeof(struct mach_header_64));
			offs=(off_t)(offs+fc_ptr->faddr);
			break;
		case bit32:
		default:
			offs=(off_t)get_arch_off(-1);
			offs=(off_t)(offs+sizeof(struct mach_header));
			offs=(off_t)(offs+fc_ptr->faddr);
			break;
	}
	for(n=0;n<=max_lc(arch);n++)
	{
		lc=(struct load_command *)offs;
		printf("lc[%d] [%s]\n",n,get_lc_type(get_data32(lc->cmd)));
		offs=(off_t)(offs+get_data32(lc->cmdsize));
	}
}
void print_macho_s_var(struct _fmt *fmt,struct _structvar *ptr)
{
	int sect=SECT_NULL;
	struct _var *var;
	if((strcmp(ptr->name,"mac"))==0) sect=SECT_MAC;
	if((strcmp(ptr->name,"lc"))==0) sect=SECT_LC;
	if(!ptr->next) {
		switch(sect) {
			case SECT_MAC:
				if(ptr->num!=-1) {printf("invalid section.\n"); return;}
				print_mac_hdr(-1);
				break;
			case SECT_LC:
				if(ptr->num==-1) print_lclist(-1); else print_lc(-1,ptr->num);
				break;
			default:
				printf("unknown section.\n");
				break;
		}
	}
	else
	{
		if(((strcmp(ptr->next->name,"sect"))==0)&&((strcmp(ptr->name,"lc"))==0)&&(ptr->num!=-1)&&(!ptr->next->next)) sect=SECT_MACSECT;
		if(sect==SECT_MACSECT) 
		{
			if(ptr->next->num==-1) print_sectlist(-1,ptr->num); else{print_sect(-1,ptr->num,ptr->next->num);}
		}
		else
		{
			//if(ptr->next->num!=-1) {printf("invalid section.\n"); return;}
			var=macho_get_s_val(ptr);
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
}
void print_lc(int arch,int lcnum)
{
	off_t offs;
	int n;
	//struct mach_header *mac32;
	//struct mach_header_64 *mac64;
	struct load_command *lc;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	//struct section *sect32;
	//struct section_64 *sect64;
	//struct uuid_command *uuid;
	////struct lc_str *lcstr;
	//struct twolevel_hints_command *twolevel;
	//struct dylib_command *dylib;
	//struct dylinker_command *dylinker;
	//struct symtab_command *symtab;
	//struct dysymtab_command *dysymtab;
	//struct thread_command *thread;
	
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	offs=get_arch_off(arch);
	switch(fc_ptr->file_bit_class){
		case bit64:
			offs=(off_t)(offs+sizeof(struct mach_header_64));
			offs=(off_t)(offs+fc_ptr->faddr);
			lc=(struct load_command*)offs;
			for(n=0;n<lcnum;n++)
			{
				lc=(struct load_command*)offs;
				offs=(off_t)(offs+get_data32(lc->cmdsize));
			}
			switch(get_data32(lc->cmd)){
				case LC_SEGMENT_64:
					seg64=(struct segment_command_64*)offs;
					printf("cmd: 0x%x (%d) [%s]\n",get_data32(seg64->cmd),get_data32(seg64->cmd),get_lc_type(get_data32(seg64->cmd)));
					printf("cmdsize: 0x%x (%d)\n",get_data32(seg64->cmdsize),get_data32(seg64->cmdsize));
					printf("segname: %s\n",seg64->segname);
					printf("vmaddr: 0x%"PRIx64" (%"PRId64")\n",get_data64(seg64->vmaddr),get_data64(seg64->vmaddr));
					printf("vmsize: 0x%"PRIx64" (%"PRId64")\n",get_data64(seg64->vmsize),get_data64(seg64->vmsize));
					printf("fileoff: 0x%"PRIx64" (%"PRId64")\n",get_data64(seg64->fileoff),get_data64(seg64->fileoff));
					printf("filesize: 0x%"PRIx64" (%"PRId64")\n",get_data64(seg64->filesize),get_data64(seg64->filesize));
					printf("maxprot: 0x%x (%d)\n",get_data32(seg64->maxprot),get_data32(seg64->maxprot));
					printf("initprot: 0x%x (%d)\n",get_data32(seg64->initprot),get_data32(seg64->initprot));
					printf("nsects: 0x%x (%d)\n",get_data32(seg64->nsects),get_data32(seg64->nsects));
					printf("flags: 0x%x (%d)\n",get_data32(seg64->flags),get_data32(seg64->flags));
					if(get_data32(seg64->nsects))
					{
						printf("* sect[0..%d]\n",max_sect(arch,lcnum));
					}
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
					printf("cmd: 0x%x (%d) [%s]\n",get_data32(lc->cmd),get_data32(lc->cmd),get_lc_type(get_data32(lc->cmd)));
					printf("cmdsize: 0x%x (%d)\n",get_data32(lc->cmdsize),get_data32(lc->cmdsize));
					printf("...(details hided)...\n");
					break;	
			}
			break;
		case bit32:
		default:
			offs=(off_t)(offs+sizeof(struct mach_header));
			offs=(off_t)(offs+fc_ptr->faddr);
			lc=(struct load_command*)offs;
			for(n=0;n<lcnum;n++)
			{
				lc=(struct load_command*)offs;
				offs=(off_t)(offs+get_data32(lc->cmdsize));
			}
			switch(get_data32(lc->cmd)){
				case LC_SEGMENT:
					seg32=(struct segment_command*)offs;
					printf("cmd: 0x%x (%d) [%s]\n",get_data32(seg32->cmd),get_data32(seg32->cmd),get_lc_type(get_data32(seg32->cmd)));
					printf("cmdsize: 0x%x (%d)\n",get_data32(seg32->cmdsize),get_data32(seg32->cmdsize));
					printf("segname: %s\n",seg32->segname);
					printf("vmaddr: 0x%x (%d)\n",get_data32(seg32->vmaddr),get_data32(seg32->vmaddr));
					printf("vmsize: 0x%x (%d)\n",get_data32(seg32->vmsize),get_data32(seg32->vmsize));
					printf("fileoff: 0x%x (%d)\n",get_data32(seg32->fileoff),get_data32(seg32->fileoff));
					printf("filesize: 0x%x (%d)\n",get_data32(seg32->filesize),get_data32(seg32->filesize));
					printf("maxprot: 0x%x (%d)\n",get_data32(seg32->maxprot),get_data32(seg32->maxprot));
					printf("initprot: 0x%x (%d)\n",get_data32(seg32->initprot),get_data32(seg32->initprot));
					printf("nsects: 0x%x (%d)\n",get_data32(seg32->nsects),get_data32(seg32->nsects));
					printf("flags: 0x%x (%d)\n",get_data32(seg32->flags),get_data32(seg32->flags));
					if(get_data32(seg32->nsects))
					{
						printf("* sect[0..%d]\n",max_sect(arch,lcnum));
					}
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
					printf("cmd: 0x%x (%d) [%s]\n",get_data32(lc->cmd),get_data32(lc->cmd),get_lc_type(get_data32(lc->cmd)));
					printf("cmdsize: 0x%x (%d)\n",get_data32(lc->cmdsize),get_data32(lc->cmdsize));
					printf("...(details hided)...\n");
					break;	
			}
			break;
	}
}
void print_sect(int arch,int lc,int sect)
{
	int x,n;
	off_t offs;
	//struct mach_header *mac32;
	//struct mach_header_64 *mac64;
	struct load_command *lcmd;
	struct segment_command *seg32;
	struct segment_command_64 *seg64;
	struct section *sect32;
	struct section_64 *sect64;
	if(!fc_ptr) return;
	if(lc>max_lc(arch)) {printf("out of lc bound.\n"); return;}
	if(sect>max_sect(arch,lc)) {printf("out of sect bound.\n"); return;}
	offs=get_arch_off(arch);
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
			for(n=0;n<sect;n++)
			{
				offs=offs+sizeof(struct section_64);
			}
			sect64=(struct section_64*)(offs+fc_ptr->faddr);
			printf("sectname: %s\n",sect64->sectname);
			printf("segname: %s\n",sect64->segname);
			printf("addr: 0x%"PRIx64" (%"PRId64")\n",get_data64(sect64->addr),get_data64(sect64->addr));
			printf("size: 0x%"PRIx64" (%"PRId64")\n",get_data64(sect64->size),get_data64(sect64->size));
			printf("offset: 0x%x (%d)\n",get_data32(sect64->offset),get_data32(sect64->offset));
			printf("align: 0x%x (%d)\n",get_data32(sect64->align),get_data32(sect64->align));
			printf("reloff: 0x%x (%d)\n",get_data32(sect64->reloff),get_data32(sect64->reloff));
			printf("nreloc: 0x%x (%d)\n",get_data32(sect64->nreloc),get_data32(sect64->nreloc));
			printf("flags: 0x%x (%d)\n",get_data32(sect64->flags),get_data32(sect64->flags));
			printf("reserved1: 0x%x (%d)\n",get_data32(sect64->reserved1),get_data32(sect64->reserved1));
			printf("reserved2: 0x%x (%d)\n",get_data32(sect64->reserved2),get_data32(sect64->reserved2));
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
			for(n=0;n<sect;n++)
			{
				offs=offs+sizeof(struct section);
			}
			sect32=(struct section*)(offs+fc_ptr->faddr);
			printf("sectname: %s\n",sect32->sectname);
			printf("segname: %s\n",sect32->segname);
			printf("addr: 0x%x (%d)\n",get_data32(sect32->addr),get_data32(sect32->addr));
			printf("size: 0x%x (%d)\n",get_data32(sect32->size),get_data32(sect32->size));
			printf("offset: 0x%x (%d)\n",get_data32(sect32->offset),get_data32(sect32->offset));
			printf("align: 0x%x (%d)\n",get_data32(sect32->align),get_data32(sect32->align));
			printf("reloff: 0x%x (%d)\n",get_data32(sect32->reloff),get_data32(sect32->reloff));
			printf("nreloc: 0x%x (%d)\n",get_data32(sect32->nreloc),get_data32(sect32->nreloc));
			printf("flags: 0x%x (%d)\n",get_data32(sect32->flags),get_data32(sect32->flags));
			printf("reserved1: 0x%x (%d)\n",get_data32(sect32->reserved1),get_data32(sect32->reserved1));
			printf("reserved2: 0x%x (%d)\n",get_data32(sect32->reserved2),get_data32(sect32->reserved2));
			break;
	}
}
void print_sectlist(int arch,int lc)
{
	int x,n;
	off_t offs;
	//struct mach_header *mac32;
	//struct mach_header_64 *mac64;
	struct load_command *lcmd;
	//struct segment_command *seg32;
	//struct segment_command_64 *seg64;
	struct section *sect32;
	struct section_64 *sect64;
	if(!fc_ptr) return;
	if(lc>max_lc(arch)) {printf("out of lc bound.\n"); return;}
	offs=get_arch_off(arch);
	switch(fc_ptr->file_bit_class) {
		case bit64:
			offs=offs+sizeof(struct mach_header_64);
			for(n=0;n<lc;n++)
			{
				lcmd=(struct load_command*) (offs+fc_ptr->faddr);
				x=get_data32(lcmd->cmdsize);
				offs=offs+x;
			}
			offs=offs+sizeof(struct segment_command_64);
			for(n=0;n<=max_sect(arch,lc);n++)
			{
				sect64=(struct section_64*)(offs+fc_ptr->faddr);
				printf("sect[%d] [%s] offs:0x%x size:0x%"PRIx64"\n",n,sect64->sectname,get_data32(sect64->offset),get_data64(sect64->size));
				offs=offs+sizeof(struct section_64);
			}
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
			offs=offs+sizeof(struct segment_command);
			for(n=0;n<=max_sect(arch,lc);n++)
			{
				sect32=(struct section*)(offs+fc_ptr->faddr);
				printf("sect[%d] [%s] offs:0x%x size:0x%x\n",n,sect32->sectname,get_data32(sect32->offset),get_data32(sect32->size));
				offs=offs+sizeof(struct section);
			}
			break;
	}
}
void add_fatmacho_completion()
{
	int n,x;
	char s[255];
#ifdef HAVE_LIBREADLINE
	add_completion("fat",NULL,comp_discardable);
	add_completion("fat::magic",NULL,comp_discardable);
	add_completion("fat::nfat_arch",NULL,comp_discardable);
	add_completion("arch",NULL,comp_discardable);
	x=max_fat_arch();
	for(n=0;n<=x;n++)
	{
		sprintf(s,"arch[%d]",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"arch[%d]::cputype",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"arch[%d]::cpusubtype",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"arch[%d]::offset",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"arch[%d]::size",n);
		add_completion(s,NULL,comp_discardable);
		sprintf(s,"arch[%d]::align",n);
		add_completion(s,NULL,comp_discardable);
	}
#endif
}
void add_macho_completion()
{
	int n,x,nlc,nsects,m;
	char s[255];
	struct load_command *lcmd;
	off_t offs;
	if(!fc_ptr) return;
#ifdef HAVE_LIBREADLINE
	add_completion("mac",NULL,comp_discardable);
	add_completion("mac::magic",NULL,comp_discardable);
	add_completion("mac::cputype",NULL,comp_discardable);
	add_completion("mac::cpusubtype",NULL,comp_discardable);
	add_completion("mac::filetype",NULL,comp_discardable);
	add_completion("mac::ncmds",NULL,comp_discardable);
	add_completion("mac::sizeofcmds",NULL,comp_discardable);
	add_completion("mac::flags",NULL,comp_discardable);
	if(fc_ptr){if(fc_ptr->file_bit_class==bit64){add_completion("mac::reserved",NULL,comp_discardable);}}
	nlc=max_lc(-1);
	for(n=0;n<=nlc;n++)
	{
		offs=get_macho_offset2(SECT_LC,-1,n,-1,0);
		lcmd=(struct load_command*)(offs+fc_ptr->faddr);
		x=get_data32(lcmd->cmd);
		sprintf(s,"lc[%d]",n);
		add_completion(s,NULL,comp_discardable);
		switch(x){
			case LC_SEGMENT:
			case LC_SEGMENT_64:
				sprintf(s,"lc[%d]::cmd",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::cmdsize",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::segname",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::vmaddr",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::vmsize",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::fileoff",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::filesize",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::maxprot",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::initprot",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::nsects",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::flags",n);
				add_completion(s,NULL,comp_discardable);
				nsects=max_sect(-1,n);
				if(nsects)
				{
					for(m=0;m<=nsects;m++)
					{
						sprintf(s,"lc[%d]::sect[%d]",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::sectname",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::segname",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::addr",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::size",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::offset",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::align",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::reloff",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::nreloc",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::flags",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::reserved1",n,m);
						add_completion(s,NULL,comp_discardable);
						sprintf(s,"lc[%d]::sect[%d]::reserved2",n,m);
						add_completion(s,NULL,comp_discardable);
					}
				}
				break;
			default:
				sprintf(s,"lc[%d]::cmd",n);
				add_completion(s,NULL,comp_discardable);
				sprintf(s,"lc[%d]::cmdsize",n);
				add_completion(s,NULL,comp_discardable);
				break;
		}
	}
#endif
}
