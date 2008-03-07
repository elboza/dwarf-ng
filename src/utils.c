/*
 utils.c : miscellaneous functions.

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
#include"utils.h"
#include"lang.h"
#include"vars.h"

extern FILE *yyin;

void file_open(char *s)
{
	off_t len;
	fd=open(s,O_RDWR);
	if(fd==-1)
	{
		printf("error opening %s file.\nexiting...\n",s);
		exit(1);
	}
	len=lseek(fd,(off_t)0,SEEK_END);
	faddr=(char*)mmap(NULL,(size_t)len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fd,(off_t)0);
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void file_close()
{
	delete_tables();
	free_completion();
	if(fd)
	{
		//munmap(faddr);
		close(fd);
	}
}
void file_probe()
{
	struct mach_header *mac;
	Elf32_Ehdr *elf;
	_IMAGE_DOS_HEADER *mzexe;
	_IMAGE_NT_HEADERS *pe;
	int x;
	//file_open(filename);
	//probe if is mach-o
	mac=(struct mach_header*)faddr;
	if((mac->magic==MH_MAGIC)||(mac->magic==MH_CIGAM))
	{
		file_type=FT_MACHO;
		return;
	}
	//probe if it is an ELF
	elf=(Elf32_Ehdr*)faddr;
	if((strncmp(ELFMAG,elf->e_ident,4))==0)
	{
		file_type=FT_ELF;
		return;
	}
	//probe if it is a PE
	//probe if it is a MZ
	mzexe=(_IMAGE_DOS_HEADER*)faddr;
	x=(int)mzexe->e_magic;
	printf("sig: %x (%x)\n",x,mzexe->e_lfanew);
	if(x==IMAGE_DOS_SIGNATURE)
	{
		file_type=FT_MZ;
		
	}
	
}
void load_headers()
{
	//make_table("main",-1);
	switch(file_type)
	{
	case FT_MACHO:
		load_macho_hd();
		break;
	case FT_ELF:
		load_elf_hd();
		break;
	case FT_PE:
		load_pe_hd();
		break;
	default:
		
		break;
	}
}
void load_macho_hd()
{
	int n_seg,n_sect;
	char path[255];
	struct mach_header *mac;
	struct load_command *lc;
	struct segment_command *seg;
	struct section *sect;
	mac=(struct mach_header*)faddr;
	make_table(NULL,"s",-1);
	add_s_var("s","magic",TYPE_VAL,&mac->magic);
	add_s_var("s","cputype",TYPE_VAL,&mac->cputype);
	add_s_var("s","cpusubtype",TYPE_VAL,&mac->cpusubtype);
	add_s_var("s","filetype",TYPE_VAL,&mac->filetype);
	add_s_var("s","ncmds",TYPE_VAL,&mac->ncmds);
	add_s_var("s","sizeofcmds",TYPE_VAL,&mac->sizeofcmds);
	add_s_var("s","flags",TYPE_VAL,&mac->flags);
	if(mac->ncmds>0)
	{
		make_table(NULL,"lc",mac->ncmds);
		lc=(struct load_command*)(faddr+sizeof(struct mach_header));
		n_seg=0;
		//printf("%d %x %d %x %d\n",lc->cmd,mac,sizeof(struct 	mach_header),lc,lc->cmdsize);
		do
		{
			//printf("%d.%d(%x)%d ",n,lc->cmd,lc,lc->cmdsize);
			switch(lc->cmd)
			{
			case LC_SEGMENT:
				//printf("LC_SEGMENT\n");
				seg=(struct segment_command*)lc;
				sprintf(path,"lc[%d]",n_seg);
				add_s_var(path,"cmd",TYPE_VAL,&seg->cmd);
				add_s_var(path,"cmdsize",TYPE_VAL,&seg->cmdsize);
				add_s_var(path,"segname",TYPE_STRING,&seg->segname);
				add_s_var(path,"vmaddr",TYPE_VAL,&seg->vmaddr);
				add_s_var(path,"vmsize",TYPE_VAL,&seg->vmsize);
				add_s_var(path,"fileoff",TYPE_VAL,&seg->fileoff);
				add_s_var(path,"filesize",TYPE_VAL,&seg->filesize);
				add_s_var(path,"maxprot",TYPE_VAL,&seg->maxprot);
				add_s_var(path,"initprot",TYPE_VAL,&seg->initprot);
				add_s_var(path,"nsects",TYPE_VAL,&seg->nsects);
				add_s_var(path,"flags",TYPE_VAL,&seg->flags);
				if(seg->nsects>0)
				{
					make_table(path,"sect",seg->nsects);
					sect=(struct section*)((char*)seg+sizeof(struct segment_command));
					n_sect=0;
					do{
						sprintf(path,"lc[%d]->sect[%d]",n_seg,n_sect);
						add_s_var(path,"sectname",TYPE_STRING,&sect->sectname);
						add_s_var(path,"segname",TYPE_STRING,&sect->segname);
						add_s_var(path,"addr",TYPE_VAL,&sect->addr);
						add_s_var(path,"size",TYPE_VAL,&sect->size);
						add_s_var(path,"offset",TYPE_VAL,&sect->offset);
						add_s_var(path,"align",TYPE_VAL,&sect->align);
						add_s_var(path,"reloff",TYPE_VAL,&sect->reloff);
						add_s_var(path,"nreloc",TYPE_VAL,&sect->nreloc);
						add_s_var(path,"flags",TYPE_VAL,&sect->flags);
						add_s_var(path,"reserved1",TYPE_VAL,&sect->reserved1);
						add_s_var(path,"reserved2",TYPE_VAL,&sect->reserved2);
						sect=(struct section*)((char*)sect+sizeof(struct section));
						n_sect++;
					}while(n_sect<seg->nsects);
				}
				break;
			defaulf:
				break;
			}
			lc=(struct load_command*)((char*)lc+lc->cmdsize);
			n_seg++;
		}while(n_seg<mac->ncmds);
	}
	//make_table(NULL,"lc",4);
	//make_table("lc[2]","sect",2);
	//add_s_var("lc[2]->sect[1]","pippo",TYPE_VAL,&xx);
	printf("mach-o\n");
}
void load_elf_hd()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph;
	Elf32_Shdr *sh;
	int x,num_ph,num_sh;
	char path[255];
	elf=(Elf32_Ehdr*)faddr;
	make_table(NULL,"s",-1);
	add_s_var("s","e_ident",TYPE_STRING,&elf->e_ident);
	x=(int)elf->e_type;
	add_s_var("s","e_type",TYPE_VAL,&x);
	x=(int)elf->e_machine;
	add_s_var("s","e_machine",TYPE_VAL,&x);
	add_s_var("s","e_version",TYPE_VAL,&elf->e_version);
	add_s_var("s","e_entry",TYPE_VAL,&elf->e_entry);
	add_s_var("s","e_phoff",TYPE_VAL,&elf->e_phoff);
	add_s_var("s","e_shoff",TYPE_VAL,&elf->e_shoff);
	add_s_var("s","e_flags",TYPE_VAL,&elf->e_flags);
	x=(int)elf->e_ehsize;
	add_s_var("s","e_ehsize",TYPE_VAL,&x);
	x=(int)elf->e_phentsize;
	add_s_var("s","e_phentsize",TYPE_VAL,&x);
	x=(int)elf->e_phnum;
	add_s_var("s","e_phnum",TYPE_VAL,&x);
	x=(int)elf->e_shentsize;
	add_s_var("s","e_shentsize",TYPE_VAL,&x);
	x=(int)elf->e_shnum;
	add_s_var("s","e_shnum",TYPE_VAL,&x);
	x=(int)elf->e_shstrndx;
	add_s_var("s","e_shstrndx",TYPE_VAL,&x);
	if(elf->e_phnum>0)
	{
		make_table(NULL,"ph",elf->e_phnum);
		num_ph=0;
		ph=(Elf32_Phdr*)(faddr+elf->e_phoff);
		do
		{
			sprintf(path,"ph[%d]",num_ph);
			add_s_var(path,"p_type",TYPE_VAL,&ph->p_type);
			add_s_var(path,"p_offset",TYPE_VAL,&ph->p_offset);
			add_s_var(path,"p_vaddr",TYPE_VAL,&ph->p_vaddr);
			add_s_var(path,"p_paddr",TYPE_VAL,&ph->p_paddr);
			add_s_var(path,"p_filesz",TYPE_VAL,&ph->p_filesz);
			add_s_var(path,"p_memsz",TYPE_VAL,&ph->p_memsz);
			add_s_var(path,"p_flags",TYPE_VAL,&ph->p_flags);
			add_s_var(path,"p_align",TYPE_VAL,&ph->p_align);
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
			add_s_var(path,"sh_name",TYPE_VAL,&sh->sh_name);
			add_s_var(path,"sh_type",TYPE_VAL,&sh->sh_type);
			add_s_var(path,"sh_flags",TYPE_VAL,&sh->sh_flags);
			add_s_var(path,"sh_addr",TYPE_VAL,&sh->sh_addr);
			add_s_var(path,"sh_offset",TYPE_VAL,&sh->sh_offset);
			add_s_var(path,"sh_size",TYPE_VAL,&sh->sh_size);
			add_s_var(path,"sh_link",TYPE_VAL,&sh->sh_link);
			add_s_var(path,"sh_info",TYPE_VAL,&sh->sh_info);
			add_s_var(path,"sh_addralign",TYPE_VAL,&sh->sh_addralign);
			add_s_var(path,"sh_entsize",TYPE_VAL,&sh->sh_entsize);
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
void load_pe_hd()
{
	
}
void die(char *s)
{
	printf("%s\n",s);
	exit(1);
}
char* rl_gets (char *prompt)
{
	static char *line_read = (char *)NULL;
  /* If the buffer has already been allocated,
     return the memory to the free pool. */
  if (line_read)
    {
      free (line_read);
      line_read = (char *)NULL;
    }

  /* Get a line from the user. */
  line_read = readline (prompt);

  /* If the line has any text in it,
     save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}
void execute(char *s)
{
	int pipedesc[2],r,len;
	FILE  *fp;
	len=strlen(s);
	r=pipe(pipedesc);
	if(r!=0){die("pipe error\n");}
	r=write(pipedesc[1],s,len);
	if(r!=len){die("pipe write error");}
	close(pipedesc[1]);
	fp=fdopen(pipedesc[0],"r");
	yyin=fp;
	yyparse();
	fclose(fp);
}
void shell()
{
	char *cmd;
	while(forced!=QUITTING)
	{
		cmd=rl_gets("dwarf> ");
		execute(cmd);
	}
	
}
void initialize_readline()
{
	rl_readline_name="dwarf";
	rl_attempted_completion_function=(CPPFunction *)dwarf_completion;
}
char ** dwarf_completion(char *text,int start,int end)
{
	char **matches;
	matches=(char **)NULL;
	matches=completion_matches(text,dwarf_command_generator);
	return (matches);
}
char* dwarf_command_generator(char *text,int state)
{
	static struct comp_list *ptr;
	static int len;
	char *name;
	if(!state)
	{
		ptr=first_comp;
		len=strlen(text);
	}
	while(ptr)
	{
		name=ptr->s;
		if(ptr->next) ptr=ptr->next; else ptr=NULL;
		if(strncmp(name,text,len)==0) return(strdup(name));
	}
	return ((char *)NULL);
}
void add_completion(char *path,char *item,int type)
{
	char str[1024];
	struct comp_list *ptr;
	int ok=0;
	if((path==NULL) && (item==NULL)) return;
	if(path==NULL) {strncpy(str,item,1024);ok=1;}
	if(item==NULL) {strncpy(str,path,1024);ok=1;}
	if(!ok) sprintf(str,"%s->%s",path,item);
	ptr=(struct comp_list*)malloc(sizeof(struct comp_list));
	if(ptr==NULL) die("error allocating for completation item");
	ptr->s=(char*)malloc(strlen(str));
	if(ptr->s==NULL) die("error allocating for completation item name");
	strncpy(ptr->s,str,strlen(str));
	ptr->type=type;
	ptr->prev=NULL;
	ptr->next=NULL;
	if(first_comp==NULL)
	{
		first_comp=ptr;
	}
	else
	{
		ptr->prev=last_comp;
		last_comp->next=ptr;
	}
	last_comp=ptr;
}
void free_completion()
{
	struct comp_list *ptr;
	for(ptr=first_comp;ptr;ptr=ptr->next)
	{
		if(ptr->type==comp_discardable)
		{
			if(ptr->prev)
			{
				ptr->prev->next=ptr->next;
			}
			if(ptr->next)
			{
				ptr->next->prev=ptr->prev;
			}
		}
	}
}
void add_cmds_completions()
{
	add_completion("quit",NULL,comp_fixed);
	add_completion("while",NULL,comp_fixed);
	add_completion("if",NULL,comp_fixed);
	add_completion("else",NULL,comp_fixed);
	add_completion("print",NULL,comp_fixed);
	add_completion("quit",NULL,comp_fixed);
	add_completion("pp",NULL,comp_fixed);
	add_completion("exit",NULL,comp_fixed);
//	add_completion("save",NULL,comp_fixed);
//	add_completion("write",NULL,comp_fixed);
	add_completion("load",NULL,comp_fixed);
	add_completion("read",NULL,comp_fixed);
	add_completion("open",NULL,comp_fixed);
	add_completion("info",NULL,comp_fixed);
	add_completion("type",NULL,comp_fixed);
//	add_completion("force",NULL,comp_fixed);
//	add_completion("sizeof",NULL,comp_fixed);
//	add_completion("call",NULL,comp_fixed);
//	add_completion("my",NULL,comp_fixed);
//	add_completion("local",NULL,comp_fixed);
//	add_completion("alias",NULL,comp_fixed);
//	add_completion("shift",NULL,comp_fixed);
//	add_completion("move",NULL,comp_fixed);
//	add_completion("realloc",NULL,comp_fixed);
	add_completion("help",NULL,comp_fixed);
//	add_completion("insert",NULL,comp_fixed);
//	add_completion("pos",NULL,comp_fixed);
//	add_completion("createh",NULL,comp_fixed);
	add_completion("show",NULL,comp_fixed);
}
