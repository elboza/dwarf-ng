/*
 output.h : output functions definitions.

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

/*
 * The 32-bit mach header appears at the very beginning of the object file for
 * 32-bit architectures.
 */

#ifndef DW_OUTPUT_H
#define DW_OUTPUT_H

#define DUMP_SIZE 128
#define DUMP_MAX_LINE 16
#define DUMP_LINES 8

void dump(struct output_format *fmt,struct _var *var);
void print(struct output_format *fmt,struct _var *var);
void extended_dump(off_t ofset,struct output_format *fmt);
void show_help_base(void);
void help_cmd(char *s);
void show_help_quit(void);
void show_help_info(void);
void show_help_create(void);
void show_help_dump(void);
void show_help_extract(void);
void show_help_flush(void);
void show_help_force(void);
void show_help_growth(void);
void show_help_ifthenelse(void);
void show_help_inject(void);
void show_help_len(void);
void show_help_load(void);
void show_help_move(void);
void show_help_print(void);
void show_help_refresh(void);
void show_help_reload(void);
void show_help_remove(void);
void show_help_save(void);
void show_help_show(void);
void show_help_shrink(void);
void show_help_type(void);
void show_help_while(void);

#endif

