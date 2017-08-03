/*
 vars.c : vars functions implementation.

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
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "libdwarf.h"

struct _structvar* makestructvar(char *s,off_t num,struct _structvar *next)
{
	struct _structvar *ptr;
	ptr=(struct _structvar*)malloc(sizeof(struct _structvar));
	if(!ptr) die("cannot allocate to create structvar");
	strncpy(ptr->name,s,VARNAME_LEN);
	ptr->num=num;
	ptr->next=next;
	return ptr;
}
struct _var *lookup(char *name)
{
	struct _gvar *ptr;
	for(ptr=gv_first;ptr;ptr=ptr->next)
	{
		if((strcmp(name,ptr->var->name))==0) return ptr->var;
	}
	return NULL;
}
int setvar(char *name,int type, void *val)
{
	struct _var *ptr;
	int x;
	ptr=lookup(name);
	if(!ptr) 
	{
		ptr=newvar(name);
		setvarval(ptr,type,val);
		x=addvar(ptr);
	}
	else
	{
		x=setvarval(ptr,type,val);
	}
	return x;
}
struct _var *newvar(char *name)
{
	struct _var *ptr;
	ptr=malloc(sizeof(struct _var));
	if(!ptr) {printf("error allocating new var!\n"); return NULL;}
	ptr->name=strdup(name);
	if(!ptr->name) {printf("error allocating new var !!\n"); free(ptr); return NULL;}
	ptr->type=VART_NULL;
	ptr->var_type=NULLTYPE_VAR;
	return ptr;
}
int setvarval(struct _var *ptr,int type, void *val)
{
	off_t *x;
	x=(off_t*)val;
	//printf("** %d %d\n",val,x);
	if(!ptr) return -1;
	if(ptr->type==VART_WORD) free(ptr->val.s);
	ptr->type=type;
	switch(type) {
		case VART_WORD:
			ptr->val.s=strdup((char*)val);
			if(!ptr->val.s) {printf("error allocating new var strval!\n"); ptr->type=VART_NULL; return -1;}
			break;
		case VART_NUM:
			ptr->val.num=(off_t)x;
			break;
		default:
			printf("undefined var type, aborting new var setval.\n");
			ptr->type=VART_NULL;
			return -1;
			break;
	}
	return 0;
}
int addvar(struct _var *var)
{
	struct _gvar *ptr;
	if(!var) return -1;
	ptr=malloc(sizeof(struct _gvar));
	if(!ptr) {printf("error adding var to main table.\n"); return -1;}
	ptr->var=var;
	ptr->prev=NULL;
	ptr->next=NULL;
	if(!gv_first)
	{
		gv_first=ptr;
	}
	else
	{
		gv_last->next=ptr;
		ptr->prev=gv_last;
	}
	gv_last=ptr;
	var->var_type=REGULAR_VAR;
	return true;
}
int delvar(struct _var *var)
{
	return true;	
}
int delallvar(void)
{
	return true;
}
off_t getvarnum(char *name)
{
	struct _var *ptr;
	ptr=lookup(name);
	if(!ptr) {printf("variable %s not found. (assign zero to var %s)\n",name,name); return 0;}
	if(ptr->type==VART_NUM)
	{
		return ptr->val.num;
	}
	else
	{
		printf("wrong variable type. (assign zero to var %s)\n",name); return 0;
	}
}
off_t var_tonum(struct _var *ptr)
{
	off_t x;
	if(!ptr) {warn("null var"); destroytmpvar(ptr); return 0;}
	if(ptr->type==VART_NUM)
	{
		x=ptr->val.num;
		destroytmpvar(ptr);
		return x;
	}
	destroytmpvar(ptr);
	return 0;
}
char* var_tostring(struct _var *ptr)
{
	char *str;
	if(!ptr) {warn("null var"); destroytmpvar(ptr); return NULL;}
	if(ptr->type==VART_WORD)
	{
		str=strdup(ptr->val.s);
		destroytmpvar(ptr);
		return str;
	}
	destroytmpvar(ptr);
	return NULL;
}
struct _var* createtmpvar()
{
	struct _var *ptr;
	ptr=newvar("tmpvar");
	ptr->var_type=TMP_VAR;
	return ptr;
}
void destroytmpvar(struct _var *var)
{
	if(!var) return;
	if(var->var_type==TMP_VAR)
	{
		if(var->type==VART_WORD)
		{
			if(var->val.s) free(var->val.s);
		}
		free(var);
	}
}
struct _fmt* makefmt(char *s)
{
	char *s1,cc[20];
	int x;
	struct _fmt *fmt;
	if(!s)
	{
		fmt=malloc(sizeof(struct _fmt));
		if(!fmt) return NULL;
		fmt->rep=1;
		fmt->type='d';
		fmt->null=1;
	}
	else
	{
		fmt=malloc(sizeof(struct _fmt));
		if(!fmt) return NULL;
		fmt->rep=0;
		fmt->type='x';
		fmt->null=0;
		s1=s;
		while(*s1>='0'&&*s1<='9') fmt->rep=fmt->rep*10+(*s1++)-'0';
		x=0;
		while(((*s1>='a'&&*s1<='z') ||(*s1>='A'&&*s1<='Z')) && x<18){
			cc[x++]=*s1++;
		}
		cc[x]='\0';
		//strncpy(fmt->type,&cc[0],20);
		if(x>0) fmt->type=(int)cc[0];

		//s1=strdup(s);
		//if(!s1) return NULL;
		//x=strlen(s1);
		//if(!x) return NULL;
		//fmt=malloc(sizeof(struct _fmt));
		//if(!fmt) return NULL;
		//fmt->type='x';
		//if(s1[x-1]>='a'&& s1[x-1]<='z'){
		//	fmt->type=s1[x-1];
		//	s1[x-1]='\0';
		//}
		//if(x>1) fmt->rep=atoi(s1); else fmt->rep=1;
		//fmt->null=0;
		//if(s1) free(s1);
	}
	return(fmt);
}
struct _fmt* makefmt2(int n,char c){
	struct _fmt *fmt;
	fmt=malloc(sizeof(struct _fmt));
	if(!fmt) return NULL;
	fmt->rep=n;
	fmt->type=c;
	fmt->null=0;
	return fmt;
}