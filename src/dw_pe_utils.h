/*
 dw_pe_utils.h : PE utils definitions.

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

#ifndef DW_PE_UTILS_H
#define DW_PE_UTILS_H

void load_pe_hd(void);
void load_mz_hd(void);
off_t get_offset_pe(char *s,char p);
off_t get_offset_mz(char *s,char p);
void save_mz_hd(void);
void save_pe_hd(void);
int get_max_pe_sect(void);

#endif

