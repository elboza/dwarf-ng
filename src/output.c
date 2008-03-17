/*
 output.c : output functions.

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
#include"utils.h"
#include"lang.h"
#include"vars.h"
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
	offset=var->val;
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
		printf("%.10x ",temp_offset);
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

