/*
 dw_elf_utils.h : elf utils definitions.

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

#ifndef DW_ELF_UTILS_H
#define DW_ELF_UTILS_H

void dw_elf_decode_file_cpu(char *s,int cpu);
int max_sh(void);
int max_ph(void);
struct _var* elf_get_s_val(struct _structvar *ptr);
struct _var* get_elf_var(int sect,int num,char *varname);
void elf_set_s_val(struct _structvar *ptr,struct _var *val);
void set_elf_var(int sect,int num,char *varname,struct _var *val);
off_t elf_get_ph_offs(void);
off_t elf_get_sh_offs(void);
int elf_get_ph_entsize(void);
int elf_get_sh_entsize(void);
off_t get_elf_offset(struct _structvar *ptr,int endoffset);
off_t get_elf_offset2(int sect,int num,int endoffset);
void create_elf_hdr(int sect,off_t offs,int grow,int update);

#endif

