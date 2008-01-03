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
	struct mach_header *mac;
	mac=(struct mach_header*)faddr;
	add_s_var(S_MAIN,"magic",TYPE_VAL,mac->magic);
	add_s_var(S_MAIN,"cputype",TYPE_VAL,mac->cputype);
	add_s_var(S_MAIN,"cpusubtype",TYPE_VAL,mac->cpusubtype);
	add_s_var(S_MAIN,"filetype",TYPE_VAL,mac->filetype);
	add_s_var(S_MAIN,"ncmds",TYPE_VAL,mac->ncmds);
	add_s_var(S_MAIN,"sizeofcmds",TYPE_VAL,mac->sizeofcmds);
	add_s_var(S_MAIN,"flags",TYPE_VAL,mac->flags);
	printf("mach-o\n");
}
void load_elf_hd()
{
	
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
		cmd=rl_gets("dwarf>");
		execute(cmd);
	}
	
}
