/*
 utils.c : miscellaneous definitions.

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
typedef enum {comp_fixed,comp_var,comp_discardable} rl_comp;
struct comp_list
{
	char *s;
	int type;
	struct comp_list *prev,*next;
}*first_comp,*last_comp;

void file_probe(void);
void file_open(char *s);
void die(char *s);
char* rl_gets(char *prompt);
void execute(char *s);
void file_close(void);
void shell(void);
void load_headers(void);
void load_macho_hd(void);
void load_elf_hd(void);
void initialize_readline(void);
char ** dwarf_completion();
char* dwarf_command_generator();
void add_completion(char *path,char *item,int type);
void free_completion(void);
void add_cmds_completions(void);
