/*
 output.c : output functions.

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

/*
 * The 32-bit mach header appears at the very beginning of the object file for
 * 32-bit architectures.
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
#include"output.h"

void dump(struct output_format *fmt,struct _var *var)
{
	int m_count=0,*val;
	char *buff;
	uint8_t *uc;
	void *mem;
	off_t offset;
	if(faddr==NULL) return;
	if(fmt->count==0) fmt->count++;
	if(var->type!=TYPE_VAL) return;
	offset=(off_t)var->val;
	//offset=lseek(fd,offset,SEEK_SET);
	//if(offset==-1) {printf("error while positioning into the file.\n");return;}
	//buff=(char*)malloc(DUMP_SIZE*sizeof(char));
	//if(buff==NULL) {printf("error allocating while reading from file.\n");return;}
	do{
		mem=faddr+offset;
		switch(fmt->out){
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
			fmt->count=-1;
			extended_dump(offset,fmt);
			return;
			break;
		default:
			fmt->count=-1;
			extended_dump(offset,fmt);
			return;
			break;
		};
		offset++;
		m_count++;
	}while(m_count<fmt->count);
	printf("\n");
}
void extended_dump(off_t offset,struct output_format *fmt)
{
	int n,m,lines;
	void *mem;
	uint8_t *uc;
	off_t temp_offset;
	lines=fmt->count;
	if(lines==-1) lines=DUMP_LINES;
	mem=faddr+offset;
	temp_offset=offset;
	uc=(uint8_t*)mem;
	for(m=0;m<=lines;m++)
	{
		printf("%.10x ",(unsigned int)temp_offset);
		uc=(uint8_t*)(faddr+temp_offset);
		for(n=0;n<=DUMP_MAX_LINE;n++)
		{
			printf("%.2x",*(uc++));
			if(n==DUMP_MAX_LINE/2) printf("-"); else printf(" ");
		}
		uc=(uint8_t*)(faddr+temp_offset);
		for(n=0;n<=DUMP_MAX_LINE;n++)
		{
			if(isprint(*uc)) printf("%c",*uc); else printf(".");
			uc++;
		}
		printf("\n");
		temp_offset+=DUMP_MAX_LINE;
	}
}
void print(struct output_format *fmt,struct _var *var)
{

}
void show_help_base()
{
	printf("inline help\n");
	printf("commands:\n");
	printf("help 	.::. show this help\n");
	printf("quit 	.::. exit dwarf-ng interpreter\n");
	printf("type	.::. display the file type of the opened file.\n");
	printf("create 	.::. creates a new section header.\n");
	printf("dump 	.::. show raw file data\n");
	printf("extract .::. exports portion of opened file into a new raw file\n");
	printf("flush 	.::. syncronize to disc the file structured variables\n");
	printf("growth 	.::. increase the size of the opened file.\n");
// 	printf("if-then-else .::. a tipical if-then-else construct.\n");
	printf("info 	.::. shows the main structures of the opened file.\n");
	printf("inject 	.::. inject code into the opened file.\n");
	printf("len 	.::. increase or decrease the sze of the opened file.\n");
	printf("load 	.::. open a new file.\n");
	printf("move 	.::. moves code around the opened file.\n");
	printf("print 	.::. prints variables.\n");
	printf("save 	.::. save and close the file.\n");
	printf("refresh .::. reload the file structure into memory.\n");
	printf("reload 	.::. close and reopen the file.\n");
	printf("shrink 	.::. decrease the size of the opened file.\n");
	printf("remove	.::. removes a section header.\n");
	printf("\ntype 'help \"command\"' for specific information or see dwarf's man page.\n");
}
void help_cmd(char *s)
{
	printf("help command:%s\n",s);
	if((strcmp(s,"help"))==0) show_help_base();
 	if((strcmp(s,"quit"))==0) show_help_quit();
 	if((strcmp(s,"create"))==0) show_help_create();
 	if((strcmp(s,"dump"))==0) show_help_dump();
 	if((strcmp(s,"else"))==0) show_help_ifthenelse();
 	if((strcmp(s,"extract"))==0) show_help_extract();
 	if((strcmp(s,"flush"))==0) show_help_flush();
 	if((strcmp(s,"force"))==0) show_help_force();
 	if((strcmp(s,"growth"))==0) show_help_growth();
 	if((strcmp(s,"if"))==0) show_help_ifthenelse();
 	if((strcmp(s,"info"))==0) show_help_info();
 	if((strcmp(s,"inject"))==0) show_help_inject();
 	if((strcmp(s,"len"))==0) show_help_len();
 	if((strcmp(s,"load"))==0) show_help_load();
 	if((strcmp(s,"move"))==0) show_help_move();
 	if((strcmp(s,"open"))==0) show_help_load();
 	if((strcmp(s,"pp"))==0) show_help_print();
 	if((strcmp(s,"print"))==0) show_help_print();
 	if((strcmp(s,"read"))==0) show_help_load();
 	if((strcmp(s,"refresh"))==0) show_help_refresh();
 	if((strcmp(s,"reload"))==0) show_help_reload();
 	if((strcmp(s,"remove"))==0) show_help_remove();
 	if((strcmp(s,"save"))==0) show_help_save();
 	if((strcmp(s,"show"))==0) show_help_show();
 	if((strcmp(s,"shrink"))==0) show_help_shrink();
 	if((strcmp(s,"type"))==0) show_help_type();
 	if((strcmp(s,"while"))==0) show_help_while();
 	if((strcmp(s,"write"))==0) show_help_save();
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
	printf("create(type,offs[,update][,shift])\n");
	printf("creates a new header. type is the header type, offs tells the offset where to create the header. offs is an integer an it refers to the position where to create the new header.  update tells dwarf to update the other data structures of the new header presence (relocates offsets and section numbers).shift (\">>\") tells dwarf to eventually shift the opened file so that the new header does not overwrite portions of the file.\n");
}
void show_help_dump()
{
	printf("dump [%fmt] expr\n");
	printf("dumps portion of opened file from expr offset in various formats depending on %fmt. The %fmt force a different output mode. fmt is in the form: xnn (<letter><number>) where the letter x indicates the output mode desired, and the number nn indicates the number of bytes to output. Valid option for the output modes are: x for hex output, d for decimal putput s for string output c for char output, b for binary output,e (default) for a nice dump output. examples: dump @< (dumps the beginning of the file), dump %x3 @<+10 (displays 3 bytes in hex format from 10 positions from the beginning of the file).\n");
}
void show_help_extract()
{
	printf("extract(from,len,file)\n");
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
	printf("growth expr | growth (expr)\n");
	printf("increase the size of the opened file.\n");
}
void show_help_ifthenelse()
{
	
}
void show_help_inject()
{
	printf("inject(expr1,expr2[,expr3][,expr4])\n");
	printf("injects data inside the opened file. ( expr1=file|byte, expr2=offs-from, expr3=len, expr4=\">>\"(shift) )if expr1 is a string it refers to a file to be injected into the opened file. if expr1 is a numerical value it represent the byte to inject (eventually repeated) inside the opened file. expr2 indicates the offset from where to begin to inject. expr4 can be \">>\" or omitted. if omitted the file is not growth by injecting data. expr3 can be a numerical value representing the len or the end-offset of the injecting data or cen be omitted. if expr3 is omitted dwarf uses a default len thas is 1 for the byte-injection or the file length for the file-injection.if expr4 is omitted expr3 can refer to the shift value (\">>\") or to the len-offset...dwarf is able to disambiguate. examples: inject(0,ph[3]->offset,12) writes 12 bytes of zeros at the absolute addres pointed from the variable ph[3]->offset.\n");
}
void show_help_len()
{
	printf("len expr -- increase or decrease the sze of the opened file.\n");
}
void show_help_load()
{
	printf("open filename | open(filename)\n");
	printf("(...guess?!?!) open a new file.\n");
}
void show_help_move()
{
	printf("move(expr1,expr2,expr3) | move(expr1,+expr2,expr3) | move(expr1,-expr2,expr3)\n");
	printf("move blocks of file around. (expr1=from, expr2=[len|end], expr3=to). moves from expr1 to expr3. expr2 can be a len if precede by plus(+) or minus(-), otherwise it refers to an absolute address indicating the end of the from-end block to move. examples: move(@<,+10,@>ph[0]) moves 10 bytes from the beginning of the file to the end of the first(0) program's header.\n");
}
void show_help_print()
{
	printf("print expr | print %fmt expr -- alias pp\n");
	printf("outputs the result of the expression (numerical or string). The %fmt force a different output mode. fmt indicates the output mode desired. Valid option for the output modes are: x for hex output, d for decimal, b for binary output. examples: print %x 16 (outputs 0xa), print %d 0xa (outputs 16).\n");
}
void show_help_refresh()
{
	printf("refresh -- reload the file structure into memory.\n");
}
void show_help_reload()
{
	printf("reload -- closes the opened file and reopens it reloading all the data structures.\n");
}
void show_help_remove()
{
	printf("remove(type,offs[,update[,shift]])\n");
	printf("removes a section header. see create for parameters explanation.\n");
}
void show_help_save()
{
	printf("save | save(filename)\n");
	printf("saves all the changes into the opened file. This action once performed is unreversible: changes will be permanently written on the disk.\n");
}
void show_help_show()
{
	
}
void show_help_shrink()
{
	printf("shrink expr | shrink(expr)\n");
	printf("decrease the size of the opened file.\n");
}
void show_help_type()
{
	printf("type\n");
	printf("display the file type of the opened file.\n");
}
void show_help_while()
{
	
}
