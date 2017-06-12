/*
 sh_elf_utils.h : dwarf's shell elf utils header file.

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
#ifndef SH_ELF_UTILS_H
#define SH_ELF_UTILS_H

void print_elf_hdr(void);
void print_sh(int num);
void print_ph(int num);
void print_shlist(void);
void print_phlist(void);
char* print_sh_strtable(int num);
char* print_ph_type(int num);
void print_elf_s_var(struct _fmt *fmt,struct _structvar *ptr);
void add_elf_completion(void);

#endif
