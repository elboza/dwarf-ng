/*
utils.c : utils functions implementations.

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

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include"libdwarf.h"

void die(char *s)
{
	fprintf(stderr,"** dwarf-ng critical error !!!\n");
	fprintf(stderr,"------------------------------\n");
	fprintf(stderr,"%s\n",s);
	exit(1);
}
void warn(char *s)
{
	fprintf(stderr,"++ Warning !!! ");
	fprintf(stderr,"%s\n",s);
}
void endian_swap_16(uint16_t *x)
{
	*x = (*x>>8) | 
        (*x<<8);
}

void endian_swap_32(uint32_t *x)
{
	*x = (*x>>24) | 
        ((*x<<8) & 0x00FF0000) |
        ((*x>>8) & 0x0000FF00) |
        (*x<<24);
}

// __int64 for MSVC, "long long" for gcc
void endian_swap_64(uint64_t *x)
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
int probe_cpu_endian()
{
	int x=1,res;
	char *h,*l;
	h=(char*)&x;
	l=h+3;
	res=(int)*l;
	if(res) return big_endian; else return little_endian;
}
uint8_t peek8(off_t offset)
{
	int res;
	char *mem;
	mem=(char*)fc_ptr->faddr;
	mem+=offset;
	//if(cfg.cpu_endian!=cfg.file_endian) endian_swap_16(&data);
	res=(int)*mem;
	return res;
}
uint16_t peek16(off_t offset)
{
	uint16_t res;
	uint16_t *mem;
	mem=(uint16_t*)fc_ptr->faddr;
	mem=(uint16_t*)((char*)mem+offset);
	res=*mem;
	if(fc_ptr->cpu_endian==little_endian) endian_swap_16(&res);
	return res;
}
uint32_t peek32(off_t offset)
{
	uint32_t res;
	uint32_t *mem;
	mem=(uint32_t*)fc_ptr->faddr;
	mem=(uint32_t*)((char*)mem+offset);
	res=*mem;
	if(fc_ptr->cpu_endian==little_endian) endian_swap_32(&res);
	//res=(int)data;
	return res;
}
uint64_t peek64(off_t offset)
{
	uint64_t res;
	uint64_t *mem;
	mem=(uint64_t*)fc_ptr->faddr;
	mem=(uint64_t*)((char*)mem+offset);
	res=*mem;
	if(fc_ptr->cpu_endian==little_endian) endian_swap_64(&res);
	//res=(int)data;
	return res;
}
void poke8(uint8_t data,off_t offset)
{
	char *mem;
	mem=(char*)fc_ptr->faddr;
	mem+=offset;
	*mem=(uint8_t)data;
}
void poke16(uint16_t data,off_t offset)
{
	uint16_t *mem;
	mem=(uint16_t*)fc_ptr->faddr;
	mem=(uint16_t*)((char*)mem+offset);
	if(fc_ptr->cpu_endian==little_endian) endian_swap_16(&data);
	*mem=data;
}
void poke32(uint32_t data,off_t offset)
{
	uint32_t *mem;
	mem=(uint32_t*)fc_ptr->faddr;
	mem=(uint32_t*)((char*)mem+offset);
	if(fc_ptr->cpu_endian==little_endian) endian_swap_32(&data);
	*mem=data;
}
void poke64(uint64_t data,off_t offset)
{
	uint64_t *mem;
	mem=(uint64_t*)fc_ptr->faddr;
	mem=(uint64_t*)((char*)mem+offset);
	if(fc_ptr->cpu_endian==little_endian) endian_swap_64(&data);
	*mem=data;
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
	if(fc_ptr->cpu_endian!=fc_ptr->file_endian) endian_swap_16(&data);
	res=(int)data;
	return res;
}
uint32_t get_data32(uint32_t data)
{
	int res;
	if(fc_ptr->cpu_endian!=fc_ptr->file_endian) endian_swap_32(&data);
	res=(int)data;
	return res;
}
uint64_t get_data64(uint64_t data)
{
	int res;
	if(fc_ptr->cpu_endian!=fc_ptr->file_endian) endian_swap_64(&data);
	res=(int)data;
	return res;
}
void extended_dump(off_t offset)
{
	int n,m,lines;
	void *mem;
	uint8_t *uc;
	off_t temp_offset;
	lines=DUMP_LINES;
	if(!fc_ptr) {printf("no file opened.\n"); return;}
	mem=fc_ptr->faddr+offset;
	temp_offset=offset;
	uc=(uint8_t*)mem;
	for(m=0;m<=lines;m++)
	{
		printf("%.10x  ",(unsigned int)temp_offset);
		uc=(uint8_t*)(fc_ptr->faddr+temp_offset);
		for(n=0;n<DUMP_MAX_LINE;n++)
		{
			printf("%.2x",*(uc++));
			if(n==(DUMP_MAX_LINE/2)-1) printf("-"); else printf(" ");
		}
		uc=(uint8_t*)(fc_ptr->faddr+temp_offset);
		printf(" ");
		for(n=0;n<DUMP_MAX_LINE;n++)
		{
			if(isprint(*uc)) printf("%c",*uc); else printf(".");
			uc++;
		}
		printf("\n");
		temp_offset+=DUMP_MAX_LINE;
	}
}
int decode_file_bit(int bits)
{
	switch(bits) {
		case bit8:
			return 8;
			break;
		case bit16:
			return 16;
			break;
		case bit32:
			return 32;
			break;
		case bit64:
			return 64;
			break;
		case bit128:
			return 128;
			break;
		default:
			return 0;
			break;
	}
}
char* decode_file_endian(int endian)
{
	switch(endian) {
		case little_endian:
					return "little endian";
					break;
				case big_endian:
					return "big endian";
					break;
				default:
					return "unknown endian";
					break;
	}
}
