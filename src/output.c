/*
 output.c : output functions.

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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include<inttypes.h>
#include"libdwarf.h"
#include"repl.h"
#include"output.h"
#include"dw_readline_completion.h"
#include"sh_switchers.h"
#include"sh_elf_utils.h"
#include"sh_macho_utils.h"
#include"sh_pe_utils.h"
#include"utils.h"
//#include"../../config.h"

void do_filesize(struct _cfg *ptr,int human)
{
	off_t len;
	char str[FILENAME_LEN];
	if(!ptr) {printf("no file opened.\n"); return;}
	len=filesize(ptr->fd);
	if(human)
	{
		dottedbyte(str,len);
		printf("%s\n",str);
	}
	else printf("%lld\n",(long long)len);
}
off_t do_getfilesize(struct _cfg *ptr)
{
	off_t len;
	if(!ptr) {printf("no file opened.\n"); return 0;}
	len=filesize(ptr->fd);
	return len;
}
void do_printcfg(struct _cfg *ptr,char *s)
{
	if(!ptr) {printf("no file opened.\n"); return;}
	if((strcmp(s,""))==0)
	{
		//printf("struct:\n");
		printf("writable: %d %s\n",ptr->writable,decode_yes_no(ptr->writable));
		printf("can_grow: %d %s\n",ptr->can_grow,decode_yes_no(ptr->can_grow));
		printf("name :%s\n",ptr->name);
		printf("work_on_copy: %d %s\n",ptr->work_on_copy,decode_yes_no(ptr->work_on_copy));
		printf("changed_altered: %d %s\n",ptr->changed_altered,decode_yes_no(ptr->changed_altered));
		printf("verbose: %d %s\n",ptr->verbose,decode_yes_no(ptr->verbose));
		printf("cpu_endian: %d %s\n",ptr->cpu_endian,decode_file_endian(ptr->cpu_endian));
		printf("file_endian: %d %s\n",ptr->file_endian,decode_file_endian(ptr->file_endian));
		printf("file_type: %d %s\n",ptr->file_type,decode_file_type(ptr->file_type));
		printf("file_bit_class: %d %dbit\n",ptr->file_bit_class,decode_file_bit(ptr->file_bit_class));
		printf("filesize: %lld\n",(long long)ptr->filesize);
		printf("faddr: %p\n",ptr->faddr);
		printf("copydir: %s\n",ptr->copydir);
		printf("copyname: %s\n",ptr->copyname);
		printf("fd_type: %d %s\n",ptr->fd_type,decode_fd_type(ptr->fd_type));
		printf("fd: %d\n",ptr->fd);
		printf("seek: 0x%llx\n",(long long unsigned) ptr->seek);
		printf("block: 0x%llx\n",(long long unsigned) ptr->block);
		printf("colors: %d %s\n",ptr->colors,decode_yes_no(ptr->colors));
		printf("theme: %d %s\n",ptr->theme,decode_yes_no(ptr->theme));
		printf("prev: %p next: %p first: %p last: %p\n",ptr->prev,ptr->next,filecfg_first,filecfg_last);
		return;
	}
	if((strcmp(s,"writable"))==0)
	{
		printf("%d\n",ptr->writable);
		return;
	}
	if((strcmp(s,"can_grow"))==0)
	{
		printf("%d\n",ptr->can_grow);
		return;
	}
	if((strcmp(s,"name"))==0)
	{
		printf("%s\n",ptr->name);
		return;
	}
	if((strcmp(s,"work_on_copy"))==0)
	{
		printf("%d\n",ptr->work_on_copy);
		return;
	}
	if((strcmp(s,"verbose"))==0)
	{
		printf("%d\n",ptr->verbose);
		return;
	}
	if((strcmp(s,"changed_altered"))==0)
	{
		printf("%d\n",ptr->changed_altered);
		return;
	}
	if((strcmp(s,"cpu_endian"))==0)
	{
		printf("%d\n",ptr->cpu_endian);
		return;
	}
	if((strcmp(s,"file_endian"))==0)
	{
		printf("%d\n",ptr->file_endian);
		return;
	}
	if((strcmp(s,"file_type"))==0)
	{
		printf("%d\n",ptr->file_type);
		return;
	}
	if((strcmp(s,"file_bit_class"))==0)
	{
		printf("%d\n",ptr->file_bit_class);
		return;
	}
	if((strcmp(s,"faddr"))==0)
	{
		printf("%p\n",ptr->faddr);
		return;
	}
	if((strcmp(s,"copydir"))==0)
	{
		printf("%s\n",ptr->copydir);
		return;
	}
	if((strcmp(s,"copyname"))==0)
	{
		printf("%s\n",ptr->copyname);
		return;
	}
	if((strcmp(s,"fd"))==0)
	{
		printf("%d\n",ptr->fd);
		return;
	}
	if((strcmp(s,"xxx"))==0)
	{
printf("   _________________________________       \n");
printf("  / you're curious eh? what did you \\      \n");
printf("  \\ expected ?                      /      \n");
printf("   ---------------------------------       \n");
printf("      \\      \n");
printf("       \\      \n");
printf("      ^__^         /      \n");
printf("      (oo)\\_______/  _________      \n");
printf("      (__)\\       )=(  ____|_ \\_____     \n");
printf("          ||----w |  \\ \\     \\_____ |     \n");
printf("          ||     ||   ||           ||     \n");

		return;
	}
	if((strcmp(s,"prev"))==0)
	{
		printf("%p\n",ptr->prev);
		return;
	}
	if((strcmp(s,"next"))==0)
	{
		printf("%p\n",ptr->next);
		return;
	}
	
	//default unknown varable.
	printf("unknown variable\n");
}
void do_filelist()
{
	struct _cfg *ptr;
	int count;
	for(ptr=filecfg_first,count=0;ptr;ptr=ptr->next,count++)
	{
		if(fc_ptr)
		{
			if(fc_ptr==ptr) printf("%s*%s %d: %s\n",ptr_colors[C_BGREEN],ptr_colors[C_RESET],count,ptr->name); else printf("  %d: %s\n",count,ptr->name);
		}
		else printf("  %d: %s\n",count,ptr->name);
	}
}
void do_fileuse(int num)
{
	struct _cfg *ptr;
	int count;
	for(ptr=filecfg_first,count=0;ptr;ptr=ptr->next,count++)
	{
		if(count==num)
		{
			fc_ptr=ptr;
			printf("Ok. Now working on %s file.\n",ptr->name);
#ifdef HAVE_LIBREADLINE
			free_completion();
			add_sh_completion();
			printf("%s**%s ",ptr_colors[C_BOLD],ptr_colors[C_RESET]);
#endif
			return;
		}
	}
	printf("invalid file selection.\n");
}
void do_printvar(struct _fmt *fmt,char *name)
{
	struct _var *ptr;
	ptr=lookup(name);
	if(!ptr) {printf("variable not found.\n"); return;}
	switch(ptr->type) {
		case VART_NUM:
			do_print_expr(fmt,ptr->val.num);
			break;
		case VART_WORD:
			printf("%s\n",ptr->val.s);
			break;
		default:
			printf("unknown variable type.\n");
			break;
	}
}
void do_print_s_var(struct _fmt *fmt,struct _structvar *ptr)
{
	if(!fc_ptr) {printf("unknown file type.\n"); return;}
	switch(fc_ptr->file_type) {
		case FT_ELF:
			print_elf_s_var(fmt,ptr);
			break;
		case FT_FATMACHO:
			print_fatmacho_s_var(fmt,ptr);
			break;
		case FT_MACHO:
			print_macho_s_var(fmt,ptr);
			break;
		case FT_PE:
			print_pe_s_var(fmt,ptr);
			break;
		default:
			printf("unknown file type.\n");
			break;
	}
}
void do_print_expr(struct _fmt *fmt,off_t x)
{
	if(!fmt) {printf("error no fmt.\n"); return;}
	do{
		switch(fmt->type){
			case 'x':
				printf("0x%llx ",(long long)x);
				break;
			case 'd':
			case 'i':
				printf("%lld ",(long long)x);
				break;
			case 'u':
				printf("%llu",(long long unsigned)x);
				break;
			case 'c':
				printf("%c ",(int)x);
				break;
			case 'o':
				printf("%llo ",(long long)x);
				break;
			default:
				printf("%lld ",(long long)x);
				break;
		}
		fmt->rep--;
	}while(fmt->rep>0);
	printf("\n");
}
void do_print_offset(struct _fmt *fmt,off_t x){
	char *c;
	if(!fmt) {fprintf(stderr,"error no fmt.\n"); return;}
	if(!fc_ptr){fprintf(stderr,"no file is open.\n"); return;}
	do{
		c=fc_ptr->faddr+x;
		switch(fmt->type){
			case 'x':
				printf("0x%x ",(unsigned int)*c);
				break;
			case 'd':
			case 'i':
				printf("%d ",(int)*c);
				break;
			case 'u':
				printf("%u",(unsigned int)*c);
				break;
			case 'c':
				printf("%c ",*c);
				break;
			case 'o':
				printf("%o ",*c);
				break;
			default:
				printf("%d ",(int)*c);
				break;
		}
		fmt->rep--;
		x++;
	}while(fmt->rep>0);
	printf("\n");
}
void do_dump(struct _fmt *fmt,off_t offset)
{
	int m_count=0;
	//char *buff;
	uint8_t *uc;
	void *mem;
	if(!fc_ptr) return;
	if(fmt->rep==0) fmt->rep++;
	if(fmt->null) {extended_dump(offset,0); return;}
	do{
		mem=fc_ptr->faddr+offset;
		switch(fmt->type){
			case 'x':
				uc=(uint8_t*)mem;
				printf("%.2x ",*uc);
				break;
			case 'd':
				uc=(uint8_t*)mem;
				printf("%d ",*uc);
				break;
			case 'c':
				uc=(uint8_t*)mem;
				if(isprint(*uc)) printf("%c",*uc); else printf(".");
				break;
			case 'e':
				fmt->rep=0;
				extended_dump(offset,(off_t)fmt->rep);
				return;
				break;
			default:
				fmt->rep=0;
				extended_dump(offset,(off_t)fmt->rep);
				return;
				break;
		};
		offset++;
		m_count++;
	}while(m_count<fmt->rep);
	printf("\n");
}
void show_help_base()
{
	printf("Append '?' to any char command to get detailed help.\n");
	printf("%sgeneral command syntax:%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("command%s[?][+] [%%nx] [data] [offset]%s    %sn=times,x=format%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ?                %sthis help%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| q                %sexit dwarf-ng interpreter%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| i                %sshow file info and headers info.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| o%s[?]%s             %sopen/opened file commands.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b%s[?]%s             %sSee / change block size.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s%s[?] [addr]%s      %sseek to address.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| e%s[?]%s             %sshow config vars.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| M%s[?]%s             %smove block of file around.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| p%s[?]%s             %sprint commands.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| x%s[?]%s             %sjust an alias for px (print hex).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| w%s[?]%s             %swrite operations.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| S%s[?]%s             %ssection headers operations.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| !%s'commands'%s      %sexecute commands into shell (bash).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_move()
{
	printf("%sM      #move blocks of file around.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| M %sexpr1%s [%s + - %s]%sexpr2 expr3%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_YELLOW],ptr_colors[C_RESET]);
	printf("%smove blocks of file around. (%sexpr1=from%s, %sexpr2=[len|end]%s, %sexpr3=to%s). moves from expr1 to expr3. expr2 can be a len if precede by plus(+) or minus(-), otherwise it refers to an absolute address indicating the end of the from-end block to move. examples: M @< +10 @ph[0]> moves 10 bytes from the beginning of the file to the end of the first(0) program's header.%s\n",ptr_colors[C_GREEN],ptr_colors[C_BBLUE],ptr_colors[C_GREEN],ptr_colors[C_BBLUE],ptr_colors[C_GREEN],ptr_colors[C_BBLUE],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_pprint()
{
	printf("%sgeneric print function.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| pp %s[%%nx] [@]expr%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET]);
	printf("%soutputs the result of the expression (numerical or string or @offset). The %%nx force a different output mode. x indicates the output mode desired n times. Valid option for the output modes are: x for hex output, d or ifor decimal, o for octal output, u for unsigned int. examples: pp %%x 16 (outputs 0xa), pp %%d 0xa (outputs 16), pp %%3x 23 (output 0x17 0x17 0x17), pp %%3x @0x100 (output 3 hex bytes at offset 0x100).%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void block_help_func(void){
	printf("%sGet/Set block size%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b             %sdisplay current block size%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b  %s33%s         %sset block size to 33%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b+ %s3%s          %sincrease blocksize by 3%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b- %s16%s         %sdecrease blocksize by 16%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| b  %s0xfa+4%s     %snumeric argument can be an expression%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void seek_help_func(void){
	printf("%sUsage: s   #Seek commands%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s             %sPrint current address%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s  %saddr       %sSeek to address%s\n",ptr_colors[C_YELLOW],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s-            %sUndo seek%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s+            %sRedo seek%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s- %sn          %sSeek n bytes backward%s\n",ptr_colors[C_YELLOW],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s+ %sn          %sSeek n bytes forward%s\n",ptr_colors[C_YELLOW],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s--           %sSeek blocksize bytes backward%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s++           %sSeek blocksize bytes forward%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s*            %sList undo seek history%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s/ %sDATA       %sSearch for next occurrence of 'DATA'%s\n",ptr_colors[C_YELLOW],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s/x %s9091      %sSearch for next occurrence of \\x90\\x91%s\n",ptr_colors[C_YELLOW],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ss            %sSeek silently (without adding an entry to the seek history)%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| s             %sPrint current address%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_open(void){
	printf("%sopen/opened files commands:%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| o %s[%%x] file [!type]%s       %sopen file (o foo !elf ;open foo as elf file)(o foo !! ;open foo as generic no type)(x=w write-on-copy, x=f no write-on-copy).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oc                        %sclose opened file.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ol                        %sshow opened files list.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| os                        %sshow opened file size.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ou  %sn%s                     %sswitch to opened file number n%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oR  %s[+ -]n%s                %sresize opened file of [+ -]n bytes.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oR+ %sn%s                     %sincreaes size of opened file of n bytes.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oR- %sn%s                     %sdecreasesize of opened file of n bytes.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oS %s[filename]%s             %ssave file (to filename).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oT %stype%s                   %schange file type to 'type' (oT elf).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oN %s[%%x] [filename] [!type]%s %sopen new empty file [named filename] x=w (work-on-copy) x=f (no-work-on-copy).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oP %s%s                       %sprobe opened file type and load data struct.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oL %s%s                       %s(probe (oP) alias).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| oO  %s[%%x] filename%s         %sopen filename without probing filetype and without loading data structures (x=w write-on-copy, x=f no-write-on-copy).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_config(void){
	printf("%sopen/opened files%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| e        %sshow opened file config vars.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| em       %sshow main config vars.%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_print(void){
	printf("%sprint commands%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| pp%s[?] [%%nx] [@]expr%s     %sgeneric print function.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| px %s[%%n][x]%s              %shex print of n bytes from offset x.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| pxl %sn%s                   %shex print n lines.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| pxx %s[%%n][x]%s             %shex pretty print of n bytes from offset x.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ps %s[%%n][x]%s              %sprint a string at offset x of n len.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| x  %s[%%n][x]%s              %ssame as px.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| xl  %sn%s                   %ssame as pxl.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| xx %s[%%n][x]%s              %ssame as pxx.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_write(void){
	printf("%swrite operations commands.\nappend '+' to the commands to perform the operation increasing the file size (not overwriting operations).%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wx%s[+] [%%n] '9190' [offset]%s          %swrite hex 0x91 0x90 at offset max n bytes.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| ws%s[+] [%%n] 'foo' [offset]%s           %swrite string foo at offset max n chars.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wz%s[+] [%%n] 'foo' [offset]%s           %swrite string zero terminated at offset max n chars.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wps%s[+] [%%np|b] 'pattern' [offset]%s   %swrite string pattern n times at offset (p repeats n times the string, b repeats n times the bytes[can trunc string]).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wpx%s[+] [%%np|b] 'pattern' [offset]%s   %swrite hex pattern n times at offset (p repeats n times the string, b repeats n times the bytes[can trunc string]).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wf%s[+] [%%n] ./myfile [offset]%s        %swrite myfile at offset max n bytes.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wr%s[+] [%%n][offset]%s                  %swrite random bytes at offset max n times.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wle%s[+] [%%1|2|4|8] number [offset]%s   %swrite little endian number of 1,2,4,8 bytes at offset.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wbe%s[+] [%%1|2|4|8] number [offset]%s   %swrite big endian number of 1,2,4,8 bytes at offset.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wn%s[+] [%%cCsSwWqQ] number [offset]%s   %swrite number at offset (c C=1byte, s=le short(little endian 2 bytes), S=be short, w=le word, W=be word(big endian 4 bytes), q=le quad(little endian 8 bytes), Q=be quad).%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wo%s[?]%s                               %swrite over operations commands.%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_writeover(void){
	printf("%swrite over operations commands (n=times [bytes repeated], offs=offset)%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| woa %s[%%n] expr [offs]%s        %s+=  addition (f.ex: woa 0x0102)%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| woA %s[%%n] expr [offs]%s        %s&=  and%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wos %s[%%n] expr [offs]%s        %s-=  substraction%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wol %s[%%n] expr [offs]%s        %s<<= shift left%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wom %s[%%n] expr [offs]%s        %s*=  multiply%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wor %s[%%n] expr [offs]%s        %s>>= shift right%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| woo %s[%%n] expr [offs]%s        %s|=  or%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wox %s[%%n] expr [offs]%s        %s^=  xor  (f.ex: wox %%4 0x90 0x100)%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wo2 %s[offs]%s                  %s2=  2 byte endian swap%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wo4 %s[offs]%s                  %s4=  4 byte endian swap%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| wo8 %s[offs]%s                  %s8=  8 byte endian swap%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void show_help_section(void){
	printf("%ssection commands%s\n",ptr_colors[C_GREEN],ptr_colors[C_RESET]);
	printf("| Sc+ %ssectname [offs]%s        %screate section 'sectname' at [offset] and resize file%s\n",ptr_colors[C_YELLOW],ptr_colors[C_RESET],ptr_colors[C_GREEN],ptr_colors[C_RESET]);
}
void do_dump_hex(struct _fmt *fmt,off_t x,int b,int xx){
	if(!fmt){fprintf(stderr,"error, no fmt!\n");return;}
	if(!fc_ptr) return;
	if(!b) x=fc_ptr->seek;
	if(xx){extended_dump(x,fmt->rep);return;}
	do_dump(fmt,x);
}
void do_dump_hex_lines(off_t l,off_t x,int b){
	if(!fc_ptr) return;
	if(!b){
		extended_dump(fc_ptr->seek,l*16);
		return;
	}
	extended_dump(x,l*16);
}
void do_dump_string(struct _fmt *fmt,off_t x,int b){
	if(!fmt){fprintf(stderr,"error, no fmt!\n");return;}
	if(!fc_ptr) return;
	if(!b) x=fc_ptr->seek;
	if(fmt->rep==1 && fmt->type=='d') fmt->rep=50;
	int m_count=0;
	//char *buff;
	uint8_t *uc;
	do{
		uc=(uint8_t*)fc_ptr->faddr+x;
		if(isprint(*uc)) printf("%c",*uc); else break;	
		x++;
		m_count++;
	}while(m_count<fmt->rep);
	printf("\n");
}