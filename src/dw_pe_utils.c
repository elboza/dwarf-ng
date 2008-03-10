/*
 dw_pe_utils.c : PE file format utils.

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
#include"utils.h"
#include"lang.h"
#include"vars.h"


void load_pe_hd()
{
	load_mz_hd();
	
}
void load_mz_hd()
{
    _IMAGE_DOS_HEADER *mz;
    int x;
    make_table(NULL,"mz",-1);
    mz=(_IMAGE_DOS_HEADER*)faddr;
	x=get_data16(mz->e_magic);
	add_s_var("mz","e_magic",TYPE_VAL,&x);
    x=get_data16(mz->e_cblp);
    add_s_var("mz","e_cblp",TYPE_VAL,&x);
    x=get_data16(mz->e_cp);
    add_s_var("mz","e_cp",TYPE_VAL,&x);
    x=get_data16(mz->e_crlc);
	add_s_var("mz","e_crlc",TYPE_VAL,&x);
    x=get_data16(mz->e_cparhdr);
	add_s_var("mz","e_cparhdr",TYPE_VAL,&x);
    x=get_data16(mz->e_minalloc);
    add_s_var("mz","e_minalloc",TYPE_VAL,&x);
    x=get_data16(mz->e_maxalloc);
	add_s_var("mz","e_maxalloc",TYPE_VAL,&x);
    x=get_data16(mz->e_ss);
	add_s_var("mz","e_ss",TYPE_VAL,&x);
    x=get_data16(mz->e_sp);
	add_s_var("mz","e_sp",TYPE_VAL,&x);
    x=get_data16(mz->e_csum);
	add_s_var("mz","e_csum",TYPE_VAL,&x);
    x=get_data16(mz->e_ip);
	add_s_var("mz","e_ip",TYPE_VAL,&x);
    x=get_data16(mz->e_cs);
	add_s_var("mz","e_cs",TYPE_VAL,&x);
    x=get_data16(mz->e_lfarlc);
	add_s_var("mz","e_lfarlc",TYPE_VAL,&x);
    x=get_data16(mz->e_ovno);
	add_s_var("mz","e_ovno",TYPE_VAL,&x);
//    x=get_data16(mz->e_res[4]);                    // Reserved words
//	add_s_var("mz","",TYPE_VAL,&x);
    x=get_data16(mz->e_oemid);
    add_s_var("mz","e_oemid",TYPE_VAL,&x);
    x=get_data16(mz->e_oeminfo);
	add_s_var("mz","e_oeminfo",TYPE_VAL,&x);
//    x=get_data16(mz->e_res2[10]);                  // Reserved words
//	add_s_var("mz","",TYPE_VAL,&x);
    x=get_data32(mz->e_lfanew);
    add_s_var("mz","e_lfanew",TYPE_VAL,&x);
}
