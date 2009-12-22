/*
 utils.c : miscellaneous functions.

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
#include"../config.h"

extern FILE *yyin;
extern int errno;
extern struct m_files mfiles;
extern struct m_cfg mcfg;

char * get_tmp_filename()
{
	char *s="dw_temp_file";
	strncat(mcfg.tmpworkdir,s,MAX_FILENAME);
	return &mcfg.tmpworkdir;
}
void file_open(char *s)
{
	char *tmpfilename,makecopystr[MAX_CMD],actualfilename[MAX_FILENAME];
	int ret;
	off_t len;
	if(mcfg.tmpworkcopy)
	{
		tmpfilename=get_tmp_filename();
		strncpy(actualfilename,tmpfilename,MAX_FILENAME);
		strncpy(mfiles.tmpcopy,tmpfilename,MAX_FILENAME);
		sprintf(makecopystr,"cp %s %s",s,tmpfilename);
		//printf("DEBUG: %s\n",makecopystr);
		ret=system(makecopystr);
		if(ret==-1) {printf("error on creating tmp file\n");return;}
		mfiles.fd=open(tmpfilename,O_RDWR);
	}
	else
	{
		strncpy(actualfilename,s,MAX_FILENAME);
		mfiles.fd=open(s,O_RDWR);
	}
	
	if(mfiles.fd==-1)
	{
		printf("error opening %s file.",actualfilename);
		if(errno==EACCES) printf("permission denied or check file existance.");
		printf("\nexiting...\n");
		exit(1);
	}
	strncpy(mfiles.originalfilename,s,MAX_FILENAME);
	len=lseek(mfiles.fd,(off_t)0,SEEK_END);
	faddr=(char*)mmap(NULL,(size_t)len,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,mfiles.fd,(off_t)0);
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void file_close()
{
	int x,ret;
	char cmd[MAX_CMD];
	off_t size;
	delete_tables();
	free_completion();
	file_endian=little_endian;
	if(mfiles.fd)
	{
		size=lseek(mfiles.fd,(off_t)0,SEEK_END);
		x=munmap(faddr,(size_t)size);
		close(mfiles.fd);
		mfiles.fd=0;
		faddr=NULL;
		if(mcfg.tmpworkcopy)
		{
			sprintf(cmd,"rm %s",mfiles.tmpcopy);
			ret=system(cmd);
			if(ret==-1) printf("could not remove the temp file !\n");
		}
	}
	file_type=FT_NULL;
	sh_shstrtab=0;
}
void file_probe(int verbose)
{
	struct mach_header *mac;
	struct fat_header *fat;
	Elf32_Ehdr *elf;
	_IMAGE_DOS_HEADER *mzexe;
	_IMAGE_NT_HEADERS *pe;
	int x,count;
	//file_open(filename);
	//probe if is mach-o
	if(faddr==NULL) {printf("no file loaded!\n"); return;}
	mac=(struct mach_header*)faddr;
	if((mac->magic==MH_MAGIC)||(mac->magic==MH_CIGAM))
	{
		file_type=FT_MACHO;
		if(mac->magic==MH_MAGIC) file_endian=cpu_endian;
		if(mac->magic==MH_CIGAM) file_endian=(~cpu_endian) & 1;
		if(verbose) printf("File type: Mach-O\n");
		return;
	}
	//probe if is FAT mach-o
	fat=(struct fat_header*)faddr;
	if((fat->magic==FAT_MAGIC)||(fat->magic==FAT_CIGAM))
	{
		file_type=FT_FAT_MACHO;
		file_endian=big_endian;
		if(verbose) printf("File type; FAT Mach-O\n");
		return;
	}
	//probe if it is an ELF
	elf=(Elf32_Ehdr*)faddr;
	if((strncmp(ELFMAG,elf->e_ident,4))==0)
	{
		file_type=FT_ELF;
		if(elf->e_ident[EI_DATA]==1) file_endian=little_endian;
		if(elf->e_ident[EI_DATA]==2) file_endian=big_endian;
		if(elf->e_ident[EI_CLASS]==ELFCLASS32) file_bit_class=bit32; //load 32 bit arch
		if(elf->e_ident[EI_CLASS]==ELFCLASS64) file_bit_class=bit64; // load 64 bit arch
		//probe_sh_shstrtab();
		if(verbose) printf("File type: Elf\n");
		return;
	}
	//probe if it is a PE
	//probe if it is a MZ
	mzexe=(_IMAGE_DOS_HEADER*)faddr;
	x=(int)mzexe->e_magic;
	//printf("sig: %x (%x)\n",x,mzexe->e_lfanew);
	if((x==IMAGE_DOS_SIGNATURE)||(x==0x4d5a))
	{
		file_type=FT_MZ;
		file_endian=little_endian;
		count=0;
		x=get_data32(mzexe->e_lfanew);
		pe=(_IMAGE_NT_HEADERS*)(faddr+x);
		x=get_data32(pe->Signature);
		if(x==IMAGE_NT_SIGNATURE)
		{
			file_type=FT_PE;
			count=1;
		}
		if(verbose) {printf("File type: ");if(count) printf("PE\n"); else printf("MZ-exe");}
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
	case FT_MZ:
		load_mz_hd();
		break;
	case FT_FAT_MACHO:
		load_fat_macho_hd();
		break;
	default:
		
		break;
	}
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
	if(!s) s="quit";
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
void execute_script(char *file)
{
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL) {printf("error opening %s file.\n",file); return;}
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
int probe_endian()
{
	int x=1,res;
	char *h,*l;
	h=(char*)&x;
	l=h+3;
	res=(int)*l;
	if(res) return big_endian; else return little_endian;
}
inline void endian_swap_16(uint16_t *x)
{
    *x = (*x>>8) | 
        (*x<<8);
}

inline void endian_swap_32(uint32_t *x)
{
    *x = (*x>>24) | 
        ((*x<<8) & 0x00FF0000) |
        ((*x>>8) & 0x0000FF00) |
        (*x<<24);
}

// __int64 for MSVC, "long long" for gcc
inline void endian_swap_64(uint64_t *x)
{
    *x = (*x>>56) | 
        ((*x<<40) & 0x00FF000000000000) |
        ((*x<<24) & 0x0000FF0000000000) |
        ((*x<<8)  & 0x000000FF00000000) |
        ((*x>>8)  & 0x00000000FF000000) |
        ((*x>>24) & 0x0000000000FF0000) |
        ((*x>>40) & 0x000000000000FF00) |
        (*x<<56);
}
uint8_t get_data8(uint8_t data)
{
	int res;
	//if(cpu_endian!=file_endian) endian_swap_16(&data);
	res=(int)data;
	return res;
}
uint16_t get_data16(uint16_t data)
{
	int res;
	if(cpu_endian!=file_endian) endian_swap_16(&data);
	res=(int)data;
	return res;
}
uint32_t get_data32(uint32_t data)
{
	int res;
	if(cpu_endian!=file_endian) endian_swap_32(&data);
	res=(int)data;
	return res;
}
uint64_t get_data64(uint64_t data)
{
	int res;
	if(cpu_endian!=file_endian) endian_swap_64(&data);
	res=(int)data;
	return res;
}
int get_offset(char *s,char p)
{
	off_t offset;
	offset=0;
	if(mfiles.fd)
	{
		if(s==NULL) return 0;
		if((strcmp(s,"FILE_BEGIN"))==0) return 0;
		if((strcmp(s,"FILE_END"))==0)
		{
			offset=lseek(mfiles.fd,(off_t)0,SEEK_END);
			return offset;
		}
		//printf("pos=%s %c\n",s,p);
		switch(file_type) {
		case FT_MACHO:
			offset=get_offset_macho(s,p);
			break;
		case FT_PE:
			offset=get_offset_pe(s,p);
			break;
		case FT_ELF:
			return get_offset_elf(s,p);
			break;
		case FT_MZ:
			offset=get_offset_mz(s,p);
			break;
		case FT_FAT_MACHO:
			offset=get_offset_fat_macho(s,p);
			break;
		default:
			break;
		};
	}
	return (int)offset;
}
int get_offset_var(struct _var *var,char p)
{
	if(!var) return 0;
	return get_offset(var->s,p);
}
void get_format(char *s,struct output_format *fmt)
{
	char *c;
	c=s;
	fmt->count=0;
	if(s==NULL) {fmt->out='Z';return;} // a different letter from all others :)
	fmt->out=*(c++);
	while(isdigit(*c))
	{
		fmt->count=fmt->count*10+((*(c++))-'0');
	}
	//fmt->out=*c;
}
void save_hd()
{
	switch(file_type)
	{
	case FT_MACHO:
		save_macho_hd();
		break;
	case FT_ELF:
		save_elf_hd();
		break;
	case FT_PE:
		save_pe_hd();
		break;
	case FT_MZ:
		save_mz_hd();
		break;
	case FT_FAT_MACHO:
		save_fat_macho_hd();
		break;
	default:
		
		break;
	}
}
void save_file()
{
	char cmd[MAX_CMD];
	int ret;
	save_hd();
	if(mcfg.tmpworkcopy)
	{
		printf("saving from tmp copy...");
		sprintf(cmd,"cp %s %s",mfiles.tmpcopy,mfiles.originalfilename);
		ret=system(cmd);
		if(ret==-1) {printf("error on saving original file !!\n");}
	}
}
int remap(len)
{

}
void growth(int len)
{
	off_t offset;
	int n;
	char *x;
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	x=(char*)malloc(len);
	if(x==NULL) die("error allocating mem");
	//printf("grouth:%d\n",len);
	offset=lseek(mfiles.fd,(off_t)0,SEEK_END);
	n=write(mfiles.fd,x,(size_t) len);
	free(x);
	#if HAVE_MREMAP
	faddr=mremap(faddr,(size_t) offset,(size_t) (offset+len),MAP_FILE|MAP_SHARED);
	#else
	munmap(faddr,(size_t) offset);
	faddr=(char*)mmap(NULL,(size_t) (offset+len),PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,mfiles.fd,(off_t)0);
	#endif
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void shrink(int len)
{
	off_t offset,new_offset;
	int n;
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	offset=lseek(mfiles.fd,(off_t)0,SEEK_END);
	new_offset=offset-len;
	n=ftruncate(mfiles.fd,new_offset);
	if(n==-1) die("error shrinking file");
	#if HAVE_MREMAP
	faddr=mremap(faddr,(size_t) offset,(size_t) new_offset,MAP_FILE|MAP_SHARED);
	#else
	munmap(faddr,(size_t) offset);
	faddr=(char*)mmap(NULL,(size_t)new_offset,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,mfiles.fd,(off_t)0);
	#endif
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void mod_len(int len)
{
	if(len<0) shrink(-len); else growth(len);
}
void move(int from,int end,int to)
{
	#define reversed 1
	#define normal 0
	int to_end,tmp,direction,i;
	char *mem;
	printf("move from:%d end:%d to%d\n",from,end,to);
	if(!faddr) {printf("no files is opened!\n");return;}
	if(end<from)
	{
		tmp=from;
		from=end;
		end=tmp;
	}
	to_end=end-from+to;
	printf("* from:%d end:%d to:%d to_end:%d\n",from,end,to,to_end);
	if((from<to) && (to<end)) direction=reversed; else direction=normal;
	mem=(char*)faddr;
	if(direction==reversed)
	{
		printf("reversed\n");
		for(i=end;i>=from;i--)
		{
			mem[to_end]=mem[i];
			to_end--;
		}
	}
	else
	{
		printf("normal\n");
		for(i=from;i<=end;i++)
		{
			mem[to]=mem[i];
			to++;
		}
	}
}
void move_r_pos(int from,int len,int to)
{
	move(from,from+len,to);
}
void move_r_neg(int from,int len,int to)
{
	move(from,from+len,to);
}
void inject_file(char *file,int from,int len,char *shift)
{
	int to_shift,i;
	char *mem,cdata,tmp_cmd[MAX_CMD];
	FILE *fp;
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	if(shift) { if((strcmp(">>",shift))==0) to_shift=1; else to_shift=0;} else to_shift=0;
	printf("inject from file...%s %d %d %d\n",file,from,len,to_shift);
	mem=(char*)faddr;
	fp=fopen(file,"r");
	if(fp==NULL) {printf("error opening file to inject.\n"); return;}
	if(len==-1) if((fseek(fp,0L,SEEK_END))==0) len=(int)ftell(fp);
	rewind(fp);
	if(to_shift) { sprintf(tmp_cmd,"len %d;move(%d,@>,%d);",len,from,from+len);execute(tmp_cmd);}
	mem+=from;
	for(i=0;i<len;i++,mem++)
	{
		cdata=(char)fgetc(fp);
		*mem=cdata;
		//to code: check if mem goes out of bound !!!
	}
	fclose(fp);
}
void inject_byte(int data,int from,int len,char *shift)
{
	int to_shift,i;
	char *mem,cdata,tmp_cmd[MAX_CMD];
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	if(shift) { if((strcmp(">>",shift))==0) to_shift=1; else to_shift=0;} else to_shift=0;
	printf("inject byte...%d %d %d %d\n",data,from,len,to_shift);
	cdata=(char)data;
	mem=(char*)faddr;
	if(to_shift) { sprintf(tmp_cmd,"len %d;move(%d,@>,%d);",len,from,from+len);execute(tmp_cmd);}
	mem+=from;
	for(i=0;i<len;i++,mem++) *mem=cdata;
}
int type_look_up(char *type)
{
	if(file_type==FT_NULL) {printf("file type not defined.\n"); return -1;}
	if((strcmp("ph",type))==0)
	{
		if(file_type!=FT_ELF) {printf("wrong file type.\n");return -1;}
		return(sizeof(Elf32_Phdr));
	}
	if((strcmp("sh",type))==0)
	{
		if(file_type!=FT_ELF) {printf("wrong file type.\n");return -1;}
		return(sizeof(Elf32_Shdr));
	}
	if((strncmp("lc",type,2))==0)
	{
		if(file_type!=FT_MACHO) {printf("wrong file type.\n");return -1;}
		return(sizeof(struct segment_command));
		//if((strcasecmp(type,"LC_SEGMENT"))==0) return(sizeof(struct segment_command));
		//if((strcasecmp(type,"LC_SYMSEG"))==0) return -1;
		//if((strcasecmp(type,"LC_THREAD"))==0) return -1;
		//if((strcasecmp(type,"LC_UNIXTHREAD"))==0) return -1;
		//if((strcasecmp(type,"LC_LOADFVMLIB"))==0) return -1;
		//if((strcasecmp(type,"LC_IDENT"))==0) return -1;
		//if((strcasecmp(type,"LC_PREPAGE"))==0) return -1;
		//if((strcasecmp(type,"LC_DYSYMTAB"))==0) return -1;
		//if((strcasecmp(type,"LC_LOAD_DYLIB"))==0) return -1;
		//if((strcasecmp(type,"LC_ID_DYLIB"))==0) return -1;
		//if((strcasecmp(type,"LC_LOAD_DYLINKER"))==0) return -1;
		//if((strcasecmp(type,"LC_ID_DYLINKER"))==0) return -1;
		//return -1;
	}
	if((strcmp("sect",type))==0)
	{
		if(file_type!=FT_MACHO) {printf("wrong file type.\n");return -1;}
		return(sizeof(struct section));
	}
	if((strcmp("pe",type))==0)
	{
		if(file_type!=FT_PE) {printf("wrong file type.\n");return -1;}
		return(sizeof(_IMAGE_NT_HEADERS));
	}
	if((strcmp("mz",type))==0)
	{
		if((file_type!=FT_PE)||(file_type!=FT_MZ)) {printf("wrong file type.\n");return -1;}
		return(sizeof(_IMAGE_DOS_HEADER));
	}
	if((strcmp("s",type))==0)
	{
		switch(file_type){
		case FT_ELF:
			return(sizeof(Elf32_Ehdr));
			break;
		case FT_PE:
			return(sizeof(_IMAGE_SECTION_HEADER));
			break;
		case FT_MACHO:
			return(sizeof(struct mach_header));
			break;
		default:
			printf("wrong file type.\n");
			break;
		};
	}
	return -1;
}
void create_hd(char *type,int offs,char *update,char *shift)
{
	int to_shift,to_update,len,from,sec_num,max_secs,max_macho_sects,n_type;
	char tmp_cmd[MAX_CMD],str[MAX_STR];
	sec_num=offs;
	if(update==NULL) to_update=0; else to_update=1;
	if(shift==NULL) to_shift=0; else to_shift=1;
	printf("create: %s %d %d %d....still coding it...sorry :)\n",type,offs,to_update,to_shift);
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	//from=offs;
	len=type_look_up(type);
	if(len==-1) return;
	printf("len=%d\n",len);
	n_type=section_name(type);
	switch(n_type){
	case SEC_SH:
		max_secs=get_max_sh();
		if(sec_num==-1)
		{
			sprintf(str,"sh[%d]",max_secs);
			from=get_offset_elf(str,'e');
		}
		else
		{
			sprintf(str,"sh[%d]",sec_num);
			from=get_offset_elf(str,' ');
		}
		//from-=(int)faddr;
		printf("from===%d\n",from);
		add_section_sh(sec_num);
		break;
	case SEC_PH:
		max_secs=get_max_ph();
		if(sec_num==-1)
		{
			sprintf(str,"ph[%d]",max_secs);
			from=get_offset_elf(str,'e');
		}
		else
		{
			sprintf(str,"ph[%d]",sec_num);
			from=get_offset_elf(str,' ');
		}
		//from-=(int)faddr;
		printf("from===%d\n",from);
		add_section_ph(sec_num);
		break;
	case SEC_PE_S:
		max_secs=get_max_pe_sect();
		if(sec_num==-1)
		{
			sprintf(str,"s[%d]",max_secs);
			from=(int)get_offset_pe(str,'e');
		}
		else
		{
			sprintf(str,"s[%d]",sec_num);
			from=(int)get_offset_pe(str,' ');
		}
		//from-=(int)faddr;
		printf("from===%d\n",from);
		add_section_pe(sec_num);
		break;
	case SEC_LC:
		max_secs=get_max_lc();
		if(sec_num==-1)
		{
			sprintf(str,"lc[%d]",max_secs);
			from=(int)get_offset_macho(str,'e');
		}
		else
		{
			sprintf(str,"lc[%d]",sec_num);
			from=(int)get_offset_macho(str,' ');
		}
		//from-=(int)faddr;
		printf("from===%d\n",from);
		add_section_lc(sec_num);
		break;
	case SEC_SECT:
		max_secs=get_max_lc();
		if(sec_num==-1)
		{
			max_macho_sects=get_max_sect(max_secs);
			if(max_macho_sects==0) {printf("invalid load command!\n");return;}
			sprintf(str,"lc[%d]->sect[%d]",max_secs,max_macho_sects);
			from=get_offset_macho(str,'e');
		}
		else
		{
			max_macho_sects=get_max_sect(sec_num);
			if(max_macho_sects==0) {printf("invalid load command!\n");return;}
			sprintf(str,"lc[%d]->sect[%d]",sec_num,max_macho_sects);
			from=get_offset_macho(str,'e');
		}
		//from-=(int)faddr;
		printf("from===%d\n",from);
		add_macho_sect(sec_num);
		break;
	default:
		printf("wrong section name!\n");
		return;
	}
	if(to_shift) {sprintf(tmp_cmd,"inject(0,%d,%d,\">>\");",from,len);execute(tmp_cmd);} else { sprintf(tmp_cmd,"inject(0,%d,%d);",from,len);}
	printf("%s\n",tmp_cmd);
	execute(tmp_cmd);
	if(to_update) printf("update cascade is not implemented yet.\n");
	//refresh();
}
int section_name(char *name)
{
	if((strcmp(name,"sh"))==0) return SEC_SH;
	if((strcmp(name,"ph"))==0) return SEC_PH;
	if((strcmp(name,"s"))==0) return SEC_PE_S;
	if((strncmp(name,"lc",2))==0) return SEC_LC;
	if((strcmp(name,"sect"))==0) return SEC_SECT;
	return -1;
}
void remove_hd(char *type,int offs,char *update,char *shift)
{
	int to_shift,to_update,len,from;
	char tmp_cmd[MAX_CMD];
	if(update==NULL) to_update=0; else to_update=1;
	if(shift==NULL) to_shift=0; else to_shift=1;
	printf("remove: %s %d %d %d......still coding it...sorry :)\n",type,offs,to_update,to_shift);
	if(!mfiles.fd) {printf("no file opened!\n");return;}
	from=offs;
	len=type_look_up(type);
	if(len==-1) return;
	printf("len=%d\n",len);
	if(to_shift) {sprintf(tmp_cmd,"move(%d,%d,%d);len %d;",from,len,-len,-len);execute(tmp_cmd);} else { sprintf(tmp_cmd,"inject(0,%d,%d);",from,len);}
	//execute(tmp_cmd);
	//remove hd in var table !!!
	if(to_update) printf("update cascade is not implemented yet.\n");
	//refresh();
}
void refresh()
{
	int x;
	if(mcfg.flush_before_refresh)
	{
		if(mcfg.ask_flush)
		{
			puts("flush data before refresh? (Yes/No) ");
			x=getchar();
			if((x=='y') || (x=='Y')) flush();
		}
		else
		{
			flush();
		}
	}
	delete_tables();
	file_probe(NOVERBOSE);
	load_headers();
	printf("refresh...\n");
}
void reload()
{
	printf("reload...not yet implemented\n");
}
void flush()
{
	save_hd();
	printf("data flushed\n");
}
void force(char *type)
{
	if((strcmp("elf",type))==0) {file_type=FT_ELF;return;}
	if((strcmp("pe",type))==0) {file_type=FT_PE;return;}
	if((strcmp("macho",type))==0) {file_type=FT_MACHO;return;}
	if((strcmp("mz",type))==0) {file_type=FT_MZ;return;}
	printf("wrong file type.\n");
}
void new_file(char *filename)
{
	printf("new file...");
	if(filename!=NULL) printf("%s",filename);
	printf("\n");
	
}
void info()
{
	switch(file_type){
	case FT_ELF:
		printf("elf header       : s\n");
		printf("section headersh : sh[0 .. %d]\n",get_max_sh());
		printf("program headersh : ph[0 .. %d]\n",get_max_ph());
		break;
	case FT_PE:
		printf("dos header : mz\n");
		printf("win header : pe\n");
		printf("sections   : s[0 .. %d]\n",get_max_pe_sect());
		break;
	case FT_MZ:
		printf("dos header    : mz\n");
		break;
	case FT_MACHO:
		printf("mac header    : s\n");
		printf("load commands : lc[0 .. %d]\n",get_max_lc());
		break;
	case FT_FAT_MACHO:
		printf("fat header    : fat\n");
		printf("fat_arch      : fat->arch[0 .. %d]\n",get_max_arch());
		break;
	default:
		break;
	}
	
}
void extract(int from,int len,char *file)
{
	char *mem;
	int i;
	FILE *fp;
	printf("extract: %d %d %s\n",from,len,file);
	mem=(char*)faddr;
	mem+=from;
	fp=fopen(file,"a");
	if(!fp) {printf("error opening %s file.\n",file); return;}
	for(i=0;i<len;i++)
	{
		fputc((int)(*(mem++)),fp);
	}
	fclose(fp);
}
