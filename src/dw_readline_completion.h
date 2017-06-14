/*
 dw_readline_completion.h : dwarf's readline completion definitions.

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

#ifndef DW_READLINE_H
#define DW_READLINE_H

typedef enum {comp_fixed,comp_var,comp_discardable} rl_comp;
struct comp_list
{
        char *s;
        int type;
        struct comp_list *prev,*next;
}*first_comp,*last_comp;

void initialize_readline(void);
char** dwarf_completion(char *text,int start,int end);
char* dwarf_command_generator(char *text,int state);
void add_completion(char *path,char *item,int type);
void free_completion(void);
void add_cmds_completions(void);

#endif

