/*
 dw_readline_completion.h : dwarf's readline completion functions.

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
#include"stdint.h"
#include"main.h"
#include"macho.h"
#include"elf.h"
#include"pe.h"
#include"dw_elf_utils.h"
#include"dw_macho_utils.h"
#include"dw_pe_utils.h"
#include"dw_readline_completion.h"
#include"lang.h"
#include"vars.h"
#include"utils.h"

void initialize_readline()
{
	rl_readline_name="dwarf";
	rl_attempted_completion_function=(CPPFunction *)dwarf_completion;
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
	if(!ok) sprintf(str,"%s->%s",path,item);
	ptr=(struct comp_list*)malloc(sizeof(struct comp_list));
	if(ptr==NULL) die("error allocating for completation item");
	ptr->s=(char*)malloc(strlen(str));
	if(ptr->s==NULL) die("error allocating for completation item name");
	strncpy(ptr->s,str,strlen(str));
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
	add_completion("while ",NULL,comp_fixed);
	add_completion("if ",NULL,comp_fixed);
	add_completion("else",NULL,comp_fixed);
	add_completion("print ",NULL,comp_fixed);
	add_completion("quit",NULL,comp_fixed);
	add_completion("pp ",NULL,comp_fixed);
	add_completion("exit",NULL,comp_fixed);
//	add_completion("save",NULL,comp_fixed);
//	add_completion("write",NULL,comp_fixed);
	add_completion("load ",NULL,comp_fixed);
	add_completion("read",NULL,comp_fixed);
	add_completion("open ",NULL,comp_fixed);
	add_completion("info",NULL,comp_fixed);
	add_completion("type",NULL,comp_fixed);
//	add_completion("force",NULL,comp_fixed);
//	add_completion("sizeof",NULL,comp_fixed);
//	add_completion("call",NULL,comp_fixed);
	add_completion("grouth ",NULL,comp_fixed);
	add_completion("shrink ",NULL,comp_fixed);
//	add_completion("local",NULL,comp_fixed);
//	add_completion("alias",NULL,comp_fixed);
//	add_completion("shift",NULL,comp_fixed);
	add_completion("move ",NULL,comp_fixed);
//	add_completion("realloc",NULL,comp_fixed);
	add_completion("help ",NULL,comp_fixed);
	add_completion("inject ",NULL,comp_fixed);
	add_completion("fill ",NULL,comp_fixed);
	add_completion("eject ",NULL,comp_fixed);
	add_completion("addhd ",NULL,comp_fixed);
	add_completion("rmhd ",NULL,comp_fixed);
//	add_completion("pos",NULL,comp_fixed);
//	add_completion("createh",NULL,comp_fixed);
	add_completion("show ",NULL,comp_fixed);
	add_completion("dump ",NULL,comp_fixed);
}
