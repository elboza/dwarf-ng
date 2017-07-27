/*
 output.h : output header.

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
#ifndef DW_OUTPUT_H
#define DW_OUTPUT_H

void do_filesize(struct _cfg *ptr,int human);
off_t do_getfilesize(struct _cfg *ptr);
void do_printcfg(struct _cfg *ptr,char *s);
void do_filelist(void);
void do_fileuse(int num);
void do_printvar(struct _fmt *fmt,char *name);
void do_print_s_var(struct _fmt *fmt,struct _structvar *ptr);
void do_print_expr(struct _fmt *fmt,off_t x);
void do_dump(struct _fmt *fmt,off_t offset);
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
void show_help_pprint(void);
void show_help_fileuse(void);
void show_help_executeshell(void);
void show_help_remove(void);
void show_help_save(void);
void show_help_filesize(void);
void show_help_shrink(void);
void show_help_filelist(void);
void show_help_while(void);
void block_help_func(void);
void seek_help_func(void);
void show_help_open(void);
void show_help_config(void);
void show_help_print(void);

#endif
