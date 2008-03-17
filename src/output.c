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
	if(fmt->count==0) fmt->count++;
	if(var->type!=TYPE_VAL) return;
	offset=var->val;
	//offset=lseek(fd,offset,SEEK_SET);
	//if(offset==-1) {printf("error while positioning into the file.\n");return;}
	//buff=(char*)malloc(DUMP_SIZE*sizeof(char));
	if(buff==NULL) {printf("error allocating while reading from file.\n");return;}
	do{
		mem=faddr+offset;
		switch(fmt->out){
		case 'x':
			uc=(uint8_t*)mem;
			printf("%x ",*uc);
			break;
		case 'd':
			uc=(uint8_t*)mem;
			printf("%d ",*uc);
			break;
		case 'c':
			uc=(uint8_t*)mem;
			printf("%c ",*uc);
			break;
		default:
			break;
		};
		offset++;
		m_count++;
	}while(m_count<fmt->count);
	printf("\n");
}
void print(struct output_format *fmt,struct _var *var)
{

}

