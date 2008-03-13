/*
 dw_Mach-O_utils.h : Mach-O utils definitions.

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


void load_macho_hd()
{
	int n_seg,n_sect,x,m_ncmds,m_seg_nsects,m_lc_cmd,m_lc_cmdsize;
	char path[255];
	struct mach_header *mac;
	struct load_command *lc;
	struct segment_command *seg;
	struct section *sect;
	struct dylib_command *m_dylib;
	struct dylinker_command *dylinker;
	struct symtab_command *symtab;
	struct dysymtab_command *dysymtab;
	mac=(struct mach_header*)faddr;
	make_table(NULL,"s",-1);
	x=get_data32(mac->magic);
	add_s_var("s","magic",TYPE_VAL,&x);
	x=get_data32(mac->cputype);
	add_s_var("s","cputype",TYPE_VAL,&x);
	x=get_data32(mac->cpusubtype);
	add_s_var("s","cpusubtype",TYPE_VAL,&x);
	x=get_data32(mac->filetype);
	add_s_var("s","filetype",TYPE_VAL,&x);
	m_ncmds=get_data32(mac->ncmds);
	add_s_var("s","ncmds",TYPE_VAL,&m_ncmds);
	x=get_data32(mac->sizeofcmds);
	add_s_var("s","sizeofcmds",TYPE_VAL,&x);
	x=get_data32(mac->flags);
	add_s_var("s","flags",TYPE_VAL,&x);
	if(m_ncmds>0)
	{
		make_table(NULL,"lc",mac->ncmds);
		lc=(struct load_command*)(faddr+sizeof(struct mach_header));
		n_seg=0;
		//printf("%d %x %d %x %d\n",lc->cmd,mac,sizeof(struct 	mach_header),lc,lc->cmdsize);
		do
		{
			//printf("%d.%d(%x)%d ",n,lc->cmd,lc,lc->cmdsize);
			m_lc_cmd=get_data32(lc->cmd);
			m_lc_cmdsize=get_data32(lc->cmdsize);
			switch(m_lc_cmd)
			{
			case LC_SEGMENT:
				//printf("LC_SEGMENT\n");
				seg=(struct segment_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				x=get_data32(seg->cmd);
				add_s_var(path,"cmd",TYPE_VAL,&x);
				x=get_data32(seg->cmdsize);
				add_s_var(path,"cmdsize",TYPE_VAL,&x);
				//x=get_data32(seg->segname);
				add_s_var(path,"segname",TYPE_STRING,&seg->segname);
				x=get_data32(seg->vmaddr);
				add_s_var(path,"vmaddr",TYPE_VAL,&x);
				x=get_data32(seg->vmsize);
				add_s_var(path,"vmsize",TYPE_VAL,&x);
				x=get_data32(seg->fileoff);
				add_s_var(path,"fileoff",TYPE_VAL,&x);
				x=get_data32(seg->filesize);
				add_s_var(path,"filesize",TYPE_VAL,&x);
				x=get_data32(seg->maxprot);
				add_s_var(path,"maxprot",TYPE_VAL,&x);
				x=get_data32(seg->initprot);
				add_s_var(path,"initprot",TYPE_VAL,&x);
				m_seg_nsects=get_data32(seg->nsects);
				add_s_var(path,"nsects",TYPE_VAL,&m_seg_nsects);
				x=get_data32(seg->flags);
				add_s_var(path,"flags",TYPE_VAL,&x);
				if(m_seg_nsects>0)
				{
					make_table(path,"sect",m_seg_nsects);
					sect=(struct section*)((char*)seg+sizeof(struct segment_command));
					n_sect=0;
					do{
						sprintf(path,"lc[%d]->sect[%d]",n_seg,n_sect);
						add_s_var(path,"sectname",TYPE_STRING,&sect->sectname);
						add_s_var(path,"segname",TYPE_STRING,&sect->segname);
						x=get_data32(sect->addr);
						add_s_var(path,"addr",TYPE_VAL,&x);
						x=get_data32(sect->size);
						add_s_var(path,"size",TYPE_VAL,&x);
						x=get_data32(sect->offset);
						add_s_var(path,"offset",TYPE_VAL,&x);
						x=get_data32(sect->align);
						add_s_var(path,"align",TYPE_VAL,&x);
						x=get_data32(sect->reloff);
						add_s_var(path,"reloff",TYPE_VAL,&x);
						x=get_data32(sect->nreloc);
						add_s_var(path,"nreloc",TYPE_VAL,&x);
						x=get_data32(sect->flags);
						add_s_var(path,"flags",TYPE_VAL,&x);
						x=get_data32(sect->reserved1);
						add_s_var(path,"reserved1",TYPE_VAL,&x);
						x=get_data32(sect->reserved2);
						add_s_var(path,"reserved2",TYPE_VAL,&x);
						sect=(struct section*)((char*)sect+sizeof(struct section));
						n_sect++;
					}while(n_sect<seg->nsects);
				}
				break;
			case LC_LOAD_DYLIB:
			case LC_ID_DYLIB:
				m_dylib=(struct dylib_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				x=get_data32(m_dylib->cmd);
				add_s_var(path,"cmd",TYPE_VAL,&x);
				x=get_data32(m_dylib->cmdsize);
				add_s_var(path,"cmdsize",TYPE_VAL,&x);
				x=get_data32(m_dylib->dylib.name.offset);
				add_s_var(path,"offset",TYPE_VAL,&x);
				x=get_data32(m_dylib->dylib.timestamp);
    			add_s_var(path,"timestamp",TYPE_VAL,&x);
    			x=get_data32(m_dylib->dylib.current_version);
    			add_s_var(path,"current_version",TYPE_VAL,&x);
    			x=get_data32(m_dylib->dylib.compatibility_version);
    			add_s_var(path,"compatibility_version",TYPE_VAL,&x);
				break;
			case LC_LOAD_DYLINKER:
			case LC_ID_DYLINKER:
				dylinker=(struct dylinker_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				x=get_data32(dylinker->cmd);
				add_s_var(path,"cmd",TYPE_VAL,&x);
				x=get_data32(dylinker->cmdsize);
				add_s_var(path,"cmdsize",TYPE_VAL,&x);
				x=get_data32(dylinker->name.offset);
				add_s_var(path,"offset",TYPE_VAL,&x);
				break;
			case LC_SYMTAB:
				symtab=(struct symtab_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				x=get_data32(symtab->cmd);
				add_s_var(path,"cmd",TYPE_VAL,&x);
				x=get_data32(symtab->cmdsize);
				add_s_var(path,"cmdsize",TYPE_VAL,&x);
				x=get_data32(symtab->symoff);
				add_s_var(path,"symoff",TYPE_VAL,&x);
				x=get_data32(symtab->nsyms);
				add_s_var(path,"nsyms",TYPE_VAL,&x);
				x=get_data32(symtab->stroff);
				add_s_var(path,"stroff",TYPE_VAL,&x);
				x=get_data32(symtab->strsize);
				add_s_var(path,"strsize",TYPE_VAL,&x);
				break;
			case LC_DYSYMTAB:
				dysymtab=(struct dysymtab_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				x=get_data32(dysymtab->cmd);
				add_s_var(path,"cmd",TYPE_VAL,&x);
				x=get_data32(dysymtab->cmdsize);
				add_s_var(path,"cmdsize",TYPE_VAL,&x);
				x=get_data32(dysymtab->ilocalsym);
				add_s_var(path,"ilocalsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nlocalsym);
    			add_s_var(path,"nlocalsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->iextdefsym);
    			add_s_var(path,"iextdefsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nextdefsym);
    			add_s_var(path,"nextdefsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->iundefsym);
    			add_s_var(path,"iundefsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nundefsym);
    			add_s_var(path,"nundefsym",TYPE_VAL,&x);
    			x=get_data32(dysymtab->tocoff);
    			add_s_var(path,"tocoff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->ntoc);
    			add_s_var(path,"ntoc",TYPE_VAL,&x);
    			x=get_data32(dysymtab->modtaboff);
    			add_s_var(path,"modtaboff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nmodtab);
    			add_s_var(path,"nmodtab",TYPE_VAL,&x);
    			x=get_data32(dysymtab->extrefsymoff);
    			add_s_var(path,"extrefsymoff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nextrefsyms);
    			add_s_var(path,"nextrefsyms",TYPE_VAL,&x);
    			x=get_data32(dysymtab->indirectsymoff);
    			add_s_var(path,"indirectsymoff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nindirectsyms);
    			add_s_var(path,"nindirectsyms",TYPE_VAL,&x);
    			x=get_data32(dysymtab->extreloff);
    			add_s_var(path,"extreloff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nextrel);
    			add_s_var(path,"nextrel",TYPE_VAL,&x);
    			x=get_data32(dysymtab->locreloff);
    			add_s_var(path,"locreloff",TYPE_VAL,&x);
    			x=get_data32(dysymtab->nlocrel);
    			add_s_var(path,"nlocrel",TYPE_VAL,&x);
			default:
				break;
			}
			lc=(struct load_command*)((char*)lc+m_lc_cmdsize);
			n_seg++;
		}while(n_seg<m_ncmds);
	}
	//make_table(NULL,"lc",4);
	//make_table("lc[2]","sect",2);
	//add_s_var("lc[2]->sect[1]","pippo",TYPE_VAL,&xx);
	printf("mach-o\n");
}
