#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include"stdint.h"
#include"main.h"
#include"macho.h"
#include"elf.h"
#include"utils.h"
#include"vars.h"


void add_var(char *name,int val)
{
	struct _gv *newvar;
	newvar=(struct _gv*)malloc(sizeof(struct _gv));
	if(newvar==NULL) die("error allocating space for a new variable");
	newvar->prev=NULL;
	newvar->next=NULL;
	newvar->name=strdup(name);
	if(newvar->name==NULL) die("error allocating space for a new variable name");
	newvar->val=val;
	if(gv_first==NULL)
	{
		gv_first=newvar;
	}
	else
	{
		newvar->prev=gv_last;
		gv_last->next=newvar;
	}
	gv_last=newvar;
}
void set_normal_var(char *name,int val)
{
	for(gv_ptr=gv_first;gv_ptr;gv_ptr=gv_ptr->next)
	{
		if((strcmp(name,gv_ptr->name))==0)
		{
			gv_ptr->val=val;
			return;
		}
	}
	add_var(name,val);
}
int get_normal_var(char *name)
{
	for(gv_ptr=gv_first;gv_ptr;gv_ptr=gv_ptr->next)
	{
		if((strcmp(name,gv_ptr->name))==0)
		{
			return gv_ptr->val;
		}
	}
	return 0;
}
void add_s_var(int s_type,char *name,int type,int val)
{
	struct _s_var *newvar;
	newvar=(struct _s_var*)malloc(sizeof(struct _s_var));
	if(newvar==NULL) die("error allocating space for inVar");
	newvar->name=strdup(name);
	if(newvar->name==NULL) die("error allocating space for inVar name");
	newvar->prev=NULL;
	newvar->next=NULL;
	newvar->type=type;
	switch(type)
	{
		case TYPE_VAL:
			newvar->val=val;
			break;
		case TYPE_STRING:
			
			break;
		default:
			break;
	}
	switch(s_type)
	{
		case S_MAIN:
			if(s_first==NULL)
			{
				s_first=newvar;
			}
			else
			{
				newvar->prev=s_last;
				s_last->next=newvar;
			}
			s_last=newvar;
			break;
		case S_PH:
			break;
		case S_SH:
			break;
		case S_LC:
			break;
		default:
			break;
	}
}
void set_s_var(int s_type,char *name,int type,int val)
{

}
int get_s_var(int s_type,char *name)
{
	switch(s_type)
	{
		case S_MAIN:
			
			break;
		case S_PH:
			break;
		case S_SH:
			break;
		case S_LC:
			break;
		default:
			break;
	}
}
void set_var(char *name,int val)
{
	if(name[0]=='$') set_normal_var(name,val);
}
int get_var(char *name)
{
	if(name[0]=='$') return (get_normal_var(name));
}
