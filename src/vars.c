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


void add_var(char *name,int type,void *val)
{
	struct _gv *newvar;
	int *x;
	x=val;
	newvar=(struct _gv*)malloc(sizeof(struct _gv));
	if(newvar==NULL) die("error allocating space for a new variable");
	newvar->prev=NULL;
	newvar->next=NULL;
	newvar->v.type=type;
	newvar->v.name=strdup(name);
	if(newvar->v.name==NULL) die("error allocating space for a new variable name");
	if(newvar->v.type==TYPE_VAL)
	{
		newvar->v.val=*x;
	}
	if(newvar->v.type==TYPE_STRING)
	{
		newvar->v.s=strdup((char*)val);
		if(newvar->v.s==NULL) {die("error allocating space for var val");}
	}
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
void set_normal_var(char *name,int type,void *val)
{
	int *x;
	x=val;
	for(gv_ptr=gv_first;gv_ptr;gv_ptr=gv_ptr->next)
	{
		if((strcmp(name,gv_ptr->v.name))==0)
		{
			
			if(type==TYPE_VAL)
			{
				gv_ptr->v.val=*x;
			}
			if(type==TYPE_STRING)
			{
				if(gv_ptr->v.type==TYPE_STRING) if(gv_ptr->v.s) free (gv_ptr->v.s);
				gv_ptr->v.s=strdup((char*)val);
				if(gv_ptr->v.s==NULL){die("error allocating space for setting var str");}
			}
			gv_ptr->v.type=type;
			return;
		}
	}
	add_var(name,type,val);
}
struct _var* get_normal_var(char *name)
{
	for(gv_ptr=gv_first;gv_ptr;gv_ptr=gv_ptr->next)
	{
		if((strcmp(name,gv_ptr->v.name))==0)
		{
			return &gv_ptr->v;
		}
	}
	return 0;
}
void add_s_var(int s_type,char *name,int type,void *val)
{
	struct _gv *newvar;
	int *x;
	x=val;
	newvar=(struct _gv*)malloc(sizeof(struct _gv));
	if(newvar==NULL) die("error allocating space for inVar");
	newvar->v.name=strdup(name);
	if(newvar->v.name==NULL) die("error allocating space for inVar name");
	newvar->prev=NULL;
	newvar->next=NULL;
	newvar->v.type=type;
	switch(type)
	{
		case TYPE_VAL:
			newvar->v.val=*x;
			break;
		case TYPE_STRING:
			newvar->v.s=strdup((char*)val);
			if(newvar->v.s==NULL) {die("error allocating space for var val");}
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
struct _var* get_s_var(int s_type,char *name)
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
void set_var(char *name,int type,void *val)
{
	if(name[0]=='$') set_normal_var(name,type,val);
}
struct _var* get_var(char *name)
{
	if(name[0]=='$') return (get_normal_var(name));
}
void push(char *s)
{
	struct _stack_s *ptr;
	ptr=(struct _stack_s *)malloc(sizeof(struct _stack_s));
	if(ptr==NULL) die("error allocating new stack  item");
	ptr->name=NULL;
	ptr->name=strdup(s);
	if(ptr->name==NULL) die("error allocating new stack item");
	ptr->prev=last_stack;
	last_stack=ptr;
}
struct _var* peek(void)
{
	return last_stack;
}
struct _var* pop(void)
{
	struct _stack_s *ptr;
	ptr=last_stack;
	if(ptr)
	{
		last_stack=ptr->prev;
		//free(ptr);
	}
	return ptr;
}
void s_remove(void)
{
	if(last_stack)
	{
		
	}
}

