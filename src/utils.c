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
#include"dw_elf_utils.h"
#include"dw_macho_utils.h"
#include"dw_pe_utils.h"
#include"dw_readline_completion.h"
#include"lang.h"
#include"vars.h"
#include"utils.h"
#include"../config.h"

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
	int x;
	off_t size;
	delete_tables();
	free_completion();
	file_endian=little_endian;
	if(fd)
	{
		size=lseek(fd,(off_t)0,SEEK_END);
		x=munmap(faddr,(size_t)size);
		close(fd);
		faddr=NULL;
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
		if(mac->magic==MH_MAGIC) file_endian=cpu_endian;
		if(mac->magic==MH_CIGAM) file_endian=(~cpu_endian) & 1;
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
		x=get_data32(mzexe->e_lfanew);
		pe=(_IMAGE_NT_HEADERS*)(faddr+x);
		x=get_data32(pe->Signature);
		if(x==IMAGE_NT_SIGNATURE)
		{
			file_type=FT_PE;
		}
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
	if(fd)
	{
		if(s==NULL) return 0;
		if((strcmp(s,"FILE_BEGIN"))==0) return 0;
		if((strcmp(s,"FILE_END"))==0)
		{
			offset=lseek(fd,(off_t)0,SEEK_END);
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
	default:
		
		break;
	}
}
int remap(len)
{

}
void grouth(int len)
{
	off_t offset;
	int n;
	char *x;
	x=(char*)malloc(len);
	if(x==NULL) die("error allocating mem");
	//printf("grouth:%d\n",len);
	offset=lseek(fd,(off_t)0,SEEK_END);
	n=write(fd,x,(size_t) len);
	free(x);
	#if HAVE_MREMAP
	faddr=mremap(faddr,(size_t) offset,(size_t) (offset+len),MAP_FILE|MAP_SHARED);
	#else
	munmap(faddr,(size_t) offset);
	faddr=(char*)mmap(NULL,(size_t) (offset+len),PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fd,(off_t)0);
	#endif
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void shrink(int len)
{
	off_t offset,new_offset;
	int n;
	offset=lseek(fd,(off_t)0,SEEK_END);
	new_offset=offset-len;
	n=ftruncate(fd,new_offset);
	if(n==-1) die("error shrinking file");
	#if HAVE_MREMAP
	faddr=mremap(faddr,(size_t) offset,(size_t) new_offset,MAP_FILE|MAP_SHARED);
	#else
	munmap(faddr,(size_t) offset);
	faddr=(char*)mmap(NULL,(size_t)new_offset,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fd,(off_t)0);
	#endif
	if(faddr==MAP_FAILED) die("error on mmap(ing) the file");
}
void mod_len(int len)
{
	if(len<0) shrink(-len); else grouth(len);
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
