/*
 dw_readline_completion.h : dwarf's readline completion functions.

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
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<stdio.h>
#include<readline/readline.h>
#include<readline/history.h>
#include"../libdwarf/stdint.h"
#include"dw_readline_completion.h"

void initialize_readline()
{
	rl_readline_name="dwarf";
	rl_attempted_completion_function=(CPPFunction *)dwarf_completion;
	rl_completer_word_break_characters = " \t\n\"\\'<=;+*/%^~#{}(),.->$@";
	rl_completion_append_character = ' ';
}
char ** dwarf_completion(char *text,int start,int end)
{
	char **matches;
	matches=(char **)NULL;
	matches=completion_matches(text,dwarf_command_generator);
	return (matches);
}
char* dwarf_command_generator(char *text,int state)
{
	static struct comp_list *ptr;
	static int len;
	char *name;
	if(!state)
	{
		ptr=first_comp;
		len=strlen(text);
	}
	while(ptr)
	{
		name=ptr->s;
		if(ptr->next) ptr=ptr->next; else ptr=NULL;
		if(strncmp(name,text,len)==0) return(strdup(name));
	}
	return ((char *)NULL);
}
void add_completion(char *path,char *item,int type)
{
	char str[1024];
	struct comp_list *ptr;
	int ok=0;
	if((path==NULL) && (item==NULL)) return;
	if(path==NULL) {strncpy(str,item,1024);ok=1;}
	if(item==NULL) {strncpy(str,path,1024);ok=1;}
	if(!ok) sprintf(str,"%s.%s",path,item);
	ptr=(struct comp_list*)malloc(sizeof(struct comp_list));
	if(ptr==NULL) {warn("error allocating for completation item");return;}
	ptr->s=(char*)malloc(strlen(str)+1);
	if(ptr->s==NULL) {warn("error allocating for completation item name");free(ptr);return;}
	strncpy(ptr->s,str,strlen(str)+1);
	ptr->type=type;
	ptr->prev=NULL;
	ptr->next=NULL;
	if(first_comp==NULL)
	{
		first_comp=ptr;
	}
	else
	{
		ptr->prev=last_comp;
		last_comp->next=ptr;
	}
	last_comp=ptr;
}
void free_completion()
{
	struct comp_list *ptr;
	for(ptr=first_comp;ptr;ptr=ptr->next)
	{
		if(ptr->type==comp_discardable)
		{
			if(ptr->prev)
			{
				ptr->prev->next=ptr->next;
			}
			if(ptr->next)
			{
				ptr->next->prev=ptr->prev;
			}
		}
	}
}
void add_cmds_completions()
{
	add_completion("quit",NULL,comp_fixed);
	add_completion("while",NULL,comp_fixed);
	add_completion("if",NULL,comp_fixed);
	add_completion("else",NULL,comp_fixed);
	add_completion("print",NULL,comp_fixed);
	add_completion("quit",NULL,comp_fixed);
	add_completion("pp",NULL,comp_fixed);
	add_completion("exit",NULL,comp_fixed);
	add_completion("save",NULL,comp_fixed);
	add_completion("write",NULL,comp_fixed);
	add_completion("load",NULL,comp_fixed);
	add_completion("read",NULL,comp_fixed);
	add_completion("open",NULL,comp_fixed);
	add_completion("info",NULL,comp_fixed);
	add_completion("type",NULL,comp_fixed);
	add_completion("force",NULL,comp_fixed);
//	add_completion("",NULL,comp_fixed);
	add_completion("flush",NULL,comp_fixed);
	add_completion("grow",NULL,comp_fixed);
	add_completion("shrink",NULL,comp_fixed);
	add_completion("refresh",NULL,comp_fixed);
	add_completion("reload",NULL,comp_fixed);
	add_completion("filelist",NULL,comp_fixed);
	add_completion("fileuse",NULL,comp_fixed);
	add_completion("move",NULL,comp_fixed);
	add_completion("fs",NULL,comp_fixed);
	add_completion("help",NULL,comp_fixed);
	add_completion("inject",NULL,comp_fixed);
	add_completion("filesize",NULL,comp_fixed);
	add_completion("extract",NULL,comp_fixed);
	add_completion("create",NULL,comp_fixed);
	add_completion("remove",NULL,comp_fixed);
	add_completion("len",NULL,comp_fixed);
	add_completion("du",NULL,comp_fixed);
	add_completion("close",NULL,comp_fixed);
	add_completion("echo",NULL,comp_fixed);
	add_completion("dump",NULL,comp_fixed);
	add_completion("fu",NULL,comp_fixed);
	add_completion("fl",NULL,comp_fixed);
	
	add_completion("@@cfg",NULL,comp_fixed);
	add_completion("@@maincfg",NULL,comp_fixed);
	add_completion("@@cfg::work_on_copy",NULL,comp_fixed);
	add_completion("@@cfg::writable",NULL,comp_fixed);
	add_completion("@@cfg::can_grow",NULL,comp_fixed);
	add_completion("@@cfg::changed_altered",NULL,comp_fixed);
	add_completion("@@cfg::verbose",NULL,comp_fixed);
	add_completion("@@cfg::fd",NULL,comp_fixed);
	add_completion("@@cfg::name",NULL,comp_fixed);
	add_completion("@@cfg::cpu_endian",NULL,comp_fixed);
	add_completion("@@cfg::file_endian",NULL,comp_fixed);
	add_completion("@@cfg::bitclass",NULL,comp_fixed);
	add_completion("@@cfg::faddr",NULL,comp_fixed);
	add_completion("@@cfg::copydir",NULL,comp_fixed);
	add_completion("@@cfg::copyname",NULL,comp_fixed);
	add_completion("@@cfg::xxx",NULL,comp_fixed);
	add_completion("@@maincfg::work_on_copy",NULL,comp_fixed);
	add_completion("@@maincfg::writable",NULL,comp_fixed);
	add_completion("@@maincfg::can_grow",NULL,comp_fixed);
	add_completion("@@maincfg::changed_altered",NULL,comp_fixed);
	add_completion("@@maincfg::verbose",NULL,comp_fixed);
	add_completion("@@maincfg::fd",NULL,comp_fixed);
	add_completion("@@maincfg::name",NULL,comp_fixed);
	add_completion("@@maincfg::cpu_endian",NULL,comp_fixed);
	add_completion("@@maincfg::file_endian",NULL,comp_fixed);
	add_completion("@@maincfg::file_bit_class",NULL,comp_fixed);
	add_completion("@@maincfg::faddr",NULL,comp_fixed);
	add_completion("@@maincfg::copydir",NULL,comp_fixed);
	add_completion("@@maincfg::copyname",NULL,comp_fixed);
	add_completion("@@maincfg::xxx",NULL,comp_fixed);
}
