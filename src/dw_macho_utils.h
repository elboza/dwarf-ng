/*
 dw_macho_utils.h : Mach-O utils definitions.

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
#ifndef DW_MACHO_UTILS_H
#define DW_MACHO_UTILS_H

void dw_macho_decode_file_cpu(char *s,int cpu);
int max_fat_arch(void);
struct _var* fatmacho_get_s_val(struct _structvar *ptr);
struct _var* get_fatmacho_var(int sect,int num,char *varname);
void fatmacho_set_s_val(struct _structvar *ptr,struct _var *val);
void set_fatmacho_var(int sect,int num,char *varname,struct _var *val);
int max_lc(int arch);
int max_sect(int arch,int lc);
struct _var* macho_get_s_val(struct _structvar *ptr);
struct _var* get_macho_var(int sect,int arch,int lc,int lc_sect,char *varname);
void macho_set_s_val(struct _structvar *ptr,struct _var *val);
void set_macho_var(int sect,int arch,int lc,int lc_sect,char *varname,struct _var *val);
off_t get_arch_off(int arch);
char* get_lc_type(int type);
off_t get_macho_offset(struct _structvar *ptr,int endoffset);
off_t get_macho_offset2(int sect,int arch,int lc_num,int sec_num,int endoffset);
off_t get_fatmacho_offset(struct _structvar *ptr,int endoffset);
off_t get_fatmacho_offset2(int sect,int num,int endoffset);
void create_fatmacho_hdr(int sect,off_t offs,int grow,int update);
void create_macho_hdr(int sect,off_t offs,int grow,int update);

#endif
