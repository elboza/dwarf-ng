/*
 sh_macho_utils.h : dwarf's shell Mach-O utils header file.

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
#ifndef SH_MACHO_UTILS_H
#define SH_MACHO_UTILS_H

void print_fatmacho_s_var(struct _fmt *fmt,struct _structvar *ptr);
void print_fatmacho_hdr(void);
void print_fatarch(int num);
void print_fatarchlist(void);
void print_mac_hdr(int arch);
void print_lclist(int arch);
void print_macho_s_var(struct _fmt *fmt,struct _structvar *ptr);
void print_lc(int arch,int lcnum);
void print_sect(int arch,int lc,int sect);
void print_sectlist(int arch,int lc);
void add_fatmacho_completion(void);
void add_macho_completion(void);

#endif
