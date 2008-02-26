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
	Elf32_Ehdr *elf;int xx=23;
	elf=(Elf32_Ehdr*)faddr;
	make_table(NULL,"s",-1);
	add_s_var("s","prova",TYPE_VAL,&xx);
	make_table(NULL,"ph",2);
	add_s_var("ph[1]","pippo",TYPE_VAL,&xx);
	make_table("ph[2]","ptable",4);
	add_s_var("ph[2]->ptable[1]","pippo",TYPE_VAL,&xx);
	printf("elf\n");
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
