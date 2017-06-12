/*
 sh_switchers.h : dwarf's shell switchers header.

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
void sw_do_info(void);
off_t get_offset(struct _structvar *ptr,int endoffset);
void add_sh_completion(void);
void do_create(char *ss,off_t offs,int grow,int update);
