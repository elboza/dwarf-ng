/*
 output.c : output functions.

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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include"../libdwarf/libdwarf.h"
#include"repl.h"
#include"output.h"
#include"dw_readline_completion.h"
#include"sh_switchers.h"
#include"../../config.h"

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
	else printf("%ld\n",len);
}
void do_inject_byte(int data,off_t offset,off_t len,char *shift_str)
{
	int shift;
	if((strcmp(shift_str,">"))!=0) {printf("syntax error\n"); return;}
	shift=true;
	inject_byte(data,offset,len,shift);
}
void do_inject_file(char *filename,off_t offset, off_t len,char *shift_str)
{
	int shift;
	if((strcmp(shift_str,">"))!=0) {printf("syntax error\n"); return;}
	shift=true;
	inject_file(filename,offset,len,shift);
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
		printf("writable: %d\n",ptr->writable);
		printf("can_grow: %d\n",ptr->can_grow);
		printf("name :%s\n",ptr->name);
		printf("work_on_copy: %d\n",ptr->work_on_copy);
		printf("changed_altered: %d\n",ptr->changed_altered);
		printf("verbose: %d\n",ptr->verbose);
		printf("cpu_endian: %d\n",ptr->cpu_endian);
		printf("file_endian: %d\n",ptr->file_endian);
		printf("file_type: %d\n",ptr->file_type);
		printf("file_bit_class: %d\n",ptr->file_bit_class);
		printf("faddr: 0x%x\n",(unsigned int)ptr->faddr);
		printf("copydir: %s\n",ptr->copydir);
		printf("copyname: %s\n",ptr->copyname);
		printf("fd: %d\n",ptr->fd);
		printf("prev: 0x%x next: 0x%x first: 0x%x last: 0x%x\n",(unsigned int)ptr->prev,(unsigned int)ptr->next,(unsigned int)filecfg_first,(unsigned int)filecfg_last);
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
		printf("0x%x\n",(unsigned int)ptr->faddr);
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
		printf("0x%x\n",(unsigned int)ptr->prev);
		return;
	}
	if((strcmp(s,"next"))==0)
	{
		printf("0x%x\n",(unsigned int)ptr->next);
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
			if(fc_ptr==ptr) printf("* %d: %s\n",count,ptr->name); else printf("  %d: %s\n",count,ptr->name);
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
			printf("** ");
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
				printf("0x%x ",x);
				break;
			case 'd':
			case 'i':
				printf("%lld ",(long long)x);
				break;
			case 'c':
				printf("%c ",x);
				break;
			case 'o':
				printf("%o ",x);
				break;
			default:
				printf("%lld ",(long long)x);
				break;
		}
		fmt->rep--;
	}while(fmt->rep>0);
	printf("\n");
}
void do_dump(struct _fmt *fmt,off_t offset)
{
	int m_count=0;
	char *buff;
	uint8_t *uc;
	void *mem;
	if(!fc_ptr) return;
	if(fmt->rep==0) fmt->rep++;
	if(fmt->null) {extended_dump(offset); return;}
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
				fmt->rep=-1;
				extended_dump(offset);
				return;
				break;
			default:
				fmt->rep=-1;
				extended_dump(offset);
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
	printf("inline help\n");
	printf("commands:\n");
	printf("help 	.::. show this help\n");
	printf("quit 	.::. exit dwarf-ng interpreter\n");
	printf("filelist.::. display the list of all opened file.\n");
	printf("create 	.::. creates a new section header.\n");
	printf("dump 	.::. show raw file data\n");
	printf("extract .::. exports portion of opened file into a new raw file\n");
//	printf("flush 	.::. syncronize to disc the file structured variables\n");
	printf("grow 	.::. increase the size of the opened file.\n");
// 	printf("if-then-else .::. a tipical if-then-else construct.\n");
	printf("info 	.::. shows the main structures of the opened file.\n");
	printf("inject 	.::. inject code into the opened file.\n");
	printf("len 	.::. increase or decrease the sze of the opened file.\n");
	printf("load 	.::. open a new file.\n");
	printf("move 	.::. moves code around the opened file.\n");
	printf("print 	.::. prints variables, strings and expressions.\n");
	printf("save 	.::. save and close the file.\n");
	printf("fileuse .::. set current working file.\n");
	printf("!		.::. executes shell commands.\n");
	printf("shrink 	.::. decrease the size of the opened file.\n");
	printf("remove	.::. removes a section header.\n");
	printf("filesize.::. shows the size of the current working file.\n");
	printf("\ntype 'help \"command\"' for specific information or see dwarf's man page (man dwarf).\n");
}
void help_cmd(char *s)
{
	printf("help command:%s\n",s);
	if((strcmp(s,"help"))==0) show_help_base();
	if((strcmp(s,"quit"))==0) show_help_quit();
	if((strcmp(s,"create"))==0) show_help_create();
	if((strcmp(s,"dump"))==0) show_help_dump();
//	if((strcmp(s,"else"))==0) show_help_ifthenelse();
	if((strcmp(s,"extract"))==0) show_help_extract();
//	if((strcmp(s,"flush"))==0) show_help_flush();
//	if((strcmp(s,"force"))==0) show_help_force();
	if((strcmp(s,"growth"))==0) show_help_growth();
//	if((strcmp(s,"if"))==0) show_help_ifthenelse();
	if((strcmp(s,"info"))==0) show_help_info();
	if((strcmp(s,"inject"))==0) show_help_inject();
	if((strcmp(s,"len"))==0) show_help_len();
	if((strcmp(s,"load"))==0) show_help_load();
	if((strcmp(s,"move"))==0) show_help_move();
	if((strcmp(s,"open"))==0) show_help_load();
	if((strcmp(s,"pp"))==0) show_help_print();
	if((strcmp(s,"print"))==0) show_help_print();
//	if((strcmp(s,"read"))==0) show_help_load();
	if((strcmp(s,"fileuse"))==0) show_help_fileuse();
	if((strcmp(s,"!"))==0) show_help_executeshell();
	if((strcmp(s,"remove"))==0) show_help_remove();
	if((strcmp(s,"save"))==0) show_help_save();
	if((strcmp(s,"filesize"))==0) show_help_filesize();
	if((strcmp(s,"shrink"))==0) show_help_shrink();
	if((strcmp(s,"filelist"))==0) show_help_filelist();
//	if((strcmp(s,"while"))==0) show_help_while();
//	if((strcmp(s,"write"))==0) show_help_save();
// 	if((strcmp(s,""))==0) show_help_ ;
	
}
void show_help_quit()
{
  printf("quit	-- exit dwarf interpreter\n");
}
void show_help_info()
{
	printf("info -- shows the main structures of the opened file.\n");
}
void show_help_create()
{
	printf("create \"type\" offs grow [update]\n");
	printf("creates a new header. type is the header type (\"pe\",\"ph\",\"elf\",\"sect\"...), offs tells the offset where to create the header. offs is an integer an it refers to the position where to create the new header.  update tells dwarf to update the other data structures of the new header presence (relocates offsets and section numbers), the default value is not-update. grow (>> grow, !>> don't grow) tells dwarf to eventually shift/grow the opened file so that the new header does not overwrite portions of the file.\n");
}
void show_help_dump()
{
	printf("dump [%%fmt] expr\n");
	printf("dumps portion of opened file from expr offset in various formats depending on %%fmt. The %%fmt force a different output mode. fmt is in the form: nnx (<number><letter>) where the letter x indicates the output mode desired, and the number nn indicates the number of bytes to output. Valid option for the output modes are: x for hex output, d or ifor decimal output s for string output c for char output, b for binary output,e (default) for a nice dump output. examples: dump @< (dumps the beginning of the file), dump %3x @<+10 (displays 3 bytes in hex format from 10 positions from the beginning of the file).\n");
}
void show_help_extract()
{
	printf("extract from len \"file\" \n");
	printf("extract 'len' bytes from opened file from 'from' position and save it into a new file called 'file'.\n");
}
void show_help_flush()
{
	printf("flush -- writes data structured on disc.\n");
}
void show_help_force()
{
	
}
void show_help_growth()
{
	printf("grow expr\n");
	printf("increase the size of the opened file.\n");
}
void show_help_ifthenelse()
{
	
}
void show_help_inject()
{
	printf("inject expr1 expr2 [expr3] [expr4])\n");
	printf("injects data inside the opened file. ( expr1=file|byte, expr2=offs-from, expr3=len, expr4=\">>\"(shift) )if expr1 is a string it refers to a file to be injected into the opened file. if expr1 is a numerical value it represent the byte to inject (eventually repeated) inside the opened file. expr2 indicates the offset from where to begin to inject. expr4 can be \">>\" or omitted. if omitted the file is not growth by injecting data. expr3 can be a numerical value representing the len or the end-offset of the injecting data or cen be omitted. if expr3 is omitted dwarf uses a default len thas is 1 for the byte-injection or the file length for the file-injection.if expr4 is omitted expr3 can refer to the shift value (\">>\") or to the len-offset...dwarf is able to disambiguate. examples: inject 0 $ph[3]::offset 12 , writes 12 bytes of zeros at the absolute addres pointed from the variable $ph[3]::offset without growing file.\n");
}
void show_help_len()
{
	printf("len expr -- increase or decrease the sze of the opened file.\n");
}
void show_help_load()
{
	printf("open filename\n");
	printf("(...guess?!?!) open a new file.\n");
}
void show_help_move()
{
	printf("move expr1 expr2 expr3 | move expr1 +expr2 expr3 | move expr1 -expr2 expr3\n");
	printf("move blocks of file around. (expr1=from, expr2=[len|end], expr3=to). moves from expr1 to expr3. expr2 can be a len if precede by plus(+) or minus(-), otherwise it refers to an absolute address indicating the end of the from-end block to move. examples: move @< +10 @ph[0]>) moves 10 bytes from the beginning of the file to the end of the first(0) program's header.\n");
}
void show_help_print()
{
	printf("print expr | print %%fmt expr -- (alias pp)\n");
	printf("outputs the result of the expression (numerical or string). The %%fmt force a different output mode. fmt indicates the output mode desired. Valid option for the output modes are: x for hex output, d or ifor decimal, o for octal output. examples: print %x 16 (outputs 0xa), print %d 0xa (outputs 16).\n");
}
void show_help_fileuse()
{
	printf("fileuse expr -- (alias fu)\n");
	printf("set expr number (taken from filelist command) file as current working file.\n");
}
void show_help_executeshell()
{
	printf("!\"commands\"\n");
	printf("executes (bash) shell commands.\n");
}
void show_help_remove()
{
	printf("remove ..........(still coding)........\n");
//	printf("remove(type,offs[,update[,shift]])\n");
	printf("removes a section header. see create for parameters explanation.\n");
}
void show_help_save()
{
	printf("save | save \"filename\" \n");
	printf("saves all the changes into the opened file. This action once performed is unreversible: changes will be permanently written on the disk.\n");
}
void show_help_filesize()
{
	printf("filesize [-h] -- (alias fs)\n");
	printf("shows the size in bytes of the current working file. The -h option gives the result in human readable format.\n");
}
void show_help_shrink()
{
	printf("shrink expr\n");
	printf("decrease the size of the opened file.\n");
}
void show_help_filelist()
{
	printf("filelist -- (alias fl)\n");
	printf("display a list of all current opened files.\n");
}
void show_help_while()
{
	
}
