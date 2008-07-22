/*
 dw_elf_utils.h : elf utils definitions.

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

#ifndef DW_ELF_UTILS_H
#define DW_ELF_UTILS_H

void load_elf_hd(void);
int get_offset_elf(char *s,char p);
void save_elf_hd(void);
int get_max_ph(void);
void add_section_ph(int num);
int get_max_sh(void);

#endif

