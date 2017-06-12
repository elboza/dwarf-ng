/*
 sh_pe_utils.h : dwarf's shell elf utils header file.

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
#ifndef SH_PE_UTILS_H
#define SH_PE_UTILS_H

void print_mz_hdr(void);
void print_pe_s_var(struct _fmt *fmt,struct _structvar *ptr);
void print_pe_hdr(void);
void print_fileheader_hdr(void);
void print_optionalheader_hdr(void);
void print_pe_sectlist(void);
void print_pe_sect(int num);
void add_pe_completion(void);

#endif
