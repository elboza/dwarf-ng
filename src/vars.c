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
void remove_item(struct _gv *p)
{
	if(p->prev)
	{
		p->prev->next=p->next;
	}
	if(p->next)
	{
		p->next->prev=p->prev;
	}
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
struct _p* get_struct_pointer(char *name,int num)
{
	struct _p *p;
	struct _gv *ptr,*in_ptr;
	int count;
	p=(struct _p*)malloc(sizeof(struct _p));
	if(p==NULL) die("error allocating _p struct");
	if(name==NULL)
	{
		p->first=gv_first;
		p->last=gv_last;
	}
	else
	{
		for(ptr=gv_first;ptr;ptr=ptr->next)
		{
			if(ptr->v.type>=TYPE_STRUCT)
			{
				if((strcmp(name,ptr->v.name))==0)
				{
					if(ptr->v.type==TYPE_NODE_STRUCT)
					{
						count=0;
						for(in_ptr=ptr->v.p.first;in_ptr;in_ptr=in_ptr->next)
						{
							if(count==num)
							{
								p->first=in_ptr->v.p.first;
								p->last=in_ptr->v.p.last;
								return p;
							}
							count++;
						}
						if(p) free(p);p=NULL;
					}
					else
					{
						p->first=ptr->v.p.first;
						p->last=ptr->v.p.last;
						return p;
					}
				}
			}
		}
	}
	return p;
}
void add_s_var(char *struct_name,int struct_num,char *name,int type,void *val)
{
	struct _gv *newvar;
	struct _p *pst;
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
	pst=get_struct_pointer(struct_name,struct_num);
	if(pst)
	{
		if(pst->first==NULL)
		{
			pst->first=newvar;
		}
		else
		{
			newvar->prev=pst->last;
			pst->last->next=newvar;
		}
		pst->last=newvar;
		//if(((strcmp(struct_name,"s"))==0) || ((strcmp(struct_name,"main"))==0))
		//{
		//	gv_first=pst->first;
		//	gv_last=pst->last;
		//}
		//free(pst);
	}
}
void set_s_var(struct _p *p,char *name,int type,void *val)
{
	struct _var *var;
	int *x;
	x=val;
	var=get_s_var(p,name);
	if(var)
	{
		if(var->type==type)
		{
			switch(type){
			case TYPE_STRING:
				free(var->s);
				var->s=strdup((char*)val);
				if(var->s==NULL) {die("error allocating space for var val");}
				break;
			case TYPE_VAL:
				var->val=*x;
				break;
			case TYPE_STRUCT:
				break;
			default:
				break;
			}
		}
	}
	
}
struct _var* get_s_var(struct _p *p,char *name)
{
	struct _gv *ptr;
	if(!p)
	{
		p->first=gv_first;
		p->last=gv_last;
	}
	for(ptr=p->first;ptr;ptr=ptr->next)
	{
		if(strcmp(ptr->v.name,name)==0)
		{
			return &ptr->v;
		}
	}
	return NULL;
}
void set_var(char *name,int type,void *val)
{
	if(name[0]=='$') set_normal_var(name,type,val);
}
struct _var* get_var(char *name)
{
	if(name[0]=='$') return (get_normal_var(name));
}
void del_table(struct _gv *p)
{
	struct _gv *ptr;
	for(ptr=p->v.p.first;ptr;ptr=ptr->next)
	{
		if(ptr->v.type>=TYPE_STRUCT)
		{
			del_table(ptr);
		}
			remove_item(ptr);
	}
}
void delete_tables()
{
	struct _gv *ptr;
	for(ptr=gv_first;ptr;ptr=ptr->next)
	{
		if(ptr->v.type>=TYPE_STRUCT)
		{
			del_table(ptr);
			remove_item(ptr);
		}
	}
}
void add_table(struct _gv *p,struct _gv *st)
{
	struct _gv **f,**l;
	if(p==NULL)
	{
		f=&gv_first;
		l=&gv_last;
	}
	else
	{
		f=&st->v.p.first;
		l=&st->v.p.last;
	}
	if(*f==NULL)
	{
		*f=st;
	}
	else
	{
		st->prev=*l;
		(*l)->next=st;
	}
	*l=st;
}
void make_table(char *nome,int num)
{
	struct _gv *p,*x;
	int n;
	p=(struct _gv*)malloc(sizeof(struct _gv));
	if(p==NULL) die("error allocating table memory");
	p->v.name=strdup(nome);
	if(p->v.name==NULL) die("error allocating table memory");
	p->prev=NULL;
	p->next=NULL;
	p->v.p.first=NULL;
	p->v.p.last=NULL;
	if(num<0) p->v.type=TYPE_STRUCT;
	else p->v.type=TYPE_NODE_STRUCT;
	add_table(NULL,p);
	if(num>=0)
	{
		for(n=0;n<=num;n++)
		{
			x=(struct _gv*)malloc(sizeof(struct _gv));
			if(x=NULL) die("error allocating child table memory");
			x->v.name=strdup(nome);
			if(x->v.name==NULL) die("error allocating child table memory");
			x->v.type=TYPE_STRUCT;
			x->prev=NULL;
			x->next=NULL;
			x->v.p.first=NULL;
			x->v.p.last=NULL;
			add_table(p,x);
			x=NULL;
		}
	}
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

