/*
 dw_macho_utils.h : Mach-O utils definitions.

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

#ifndef DW_MACHO_UTILS_H
#define DW_MACHO_UTILS_H

void load_macho_hd(void);
int get_offset_macho(char *s,char p);
void save_macho_hd(void);
int get_max_lc(void);
int get_max_sect(int lc);
void add_section_lc(int num);
void add_macho_sect(int num);
void load_fat_macho_hd(void);
void save_fat_macho_hd(void);
int get_offset_fat_macho(char *s,char p);
void load_macho_header(int xoffs,char *mainpath);
int get_max_arch(void);

#endif

