/*
 utils.h : miscellaneous definitions.

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

#ifndef DW_UTILS_H
#define DW_UTILS_H

typedef enum {comp_fixed,comp_var,comp_discardable} rl_comp;
typedef enum {SEC_SH,SEC_PH,SEC_PE_S,SEC_LC,SEC_SECT} files_sections;
struct comp_list
{
	char *s;
	int type;
	struct comp_list *prev,*next;
}*first_comp,*last_comp;
struct output_format
{
	int count;
	char out;
};
void file_probe(int verbose);
void file_open(char *s);
void die(char *s);
char* rl_gets(char *prompt);
void execute(char *s);
void execute_script(char *file);
void file_close(void);
void shell(void);
void load_headers(void);
uint16_t get_data16(uint16_t data);
uint32_t get_data32(uint32_t data);
uint64_t get_data64(uint64_t data);
int get_offset(char *s,char p);
int get_offset_var(struct _var *var,char p);
void get_format(char *s,struct output_format *fmt);
void save_hd(void);
void growth(int len);
void shrink(int len);
void mod_len(int len);
void move(int from,int end,int to);
void move_r_pos(int from,int len,int to);
void move_r_neg(int from,int len,int to);
void inject_file(char *file,int from,int len,int shift);
void inject_byte(int data,int from,int len,int shift);
void create_hd(char *type,int offs,char *update,char *shift);
void remove_hd(char *type,int offs,char *update,char *shift);
int type_look_up(char *type);
void refresh(void);
void reload(void);
void flush(void);
void force(char *type);
void new_file(char *filename);
void save_file(void);
int section_name(char *name);
void info(void);
void extract(int from,int len,char *file);

#endif

