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
void remove_item(struct _gv *from,struct _gv *p)
{
	if(p->prev)
	{
		p->prev->next=p->next;
	}
	else
	{

	}
	if(p->next)
	{
		p->next->prev=p->prev;
	}
	if(p->prev==NULL)
	{
		if(from==NULL)
		{
			gv_first=p->next;
		}
		else
		{
			from->v.p.first=p->next;
		}
	}
	if(p->next==NULL)
	{
		if(from==NULL)
		{
			gv_last=p->prev;
		}
		else
		{
			from->v.p.last=p->prev;
		}
	}
	if(p)
	{
		if(p->v.name) free(p->v.name);
		if(p->v.type==TYPE_STRING)
		{
			if(p->v.s) free(p->v.s);
		}
		free(p);
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
char* get_token(struct token *tok,char *s)
{
	char temp[255];
	char *c,*tp;
	c=&(tok->name[0]);
	tok->num=-1;
	tp=&temp[0];
	while(isalnum(*s))
	{
		*c=*s;
		c++;s++;
	}
	*c='\0';
	c++;
	*c='\0';
	if(*s=='[')
	{
		s++;
		while(isdigit(*s))
		{
			*tp=*s;
			tp++;s++;
		}
		*tp='\0';
		s++;
		tok->num=atoi(temp);
	}
	if(*s=='-') s+=2;
	return s;
}
struct _p* get_struct_pointer(struct _p *fromtable,char *name,int num)
{
	struct _p *p;
	struct _gv *ptr,*in_ptr,*initialpointer;
	int count;
	//p=(struct _p*)malloc(sizeof(struct _p));
	//if(p==NULL) die("error allocating _p struct");
	if(fromtable==NULL) initialpointer=gv_first; else initialpointer=fromtable->first;
	if(name==NULL)
	{
		//p->first=gv_first;
		//p->last=gv_last;
		return NULL;
	}
	else
	{
		for(ptr=initialpointer;ptr;ptr=ptr->next)
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
								//p->first=in_ptr->v.p.first;
								//p->last=in_ptr->v.p.last;
								//*(pp->f)=in_ptr->v.p.first;
								//*(pp->l)=in_ptr->v.p.last;
								p=(struct _p*)&(in_ptr->v.p);
								return p;
							}
							count++;
						}
						if(p) free(p);p=NULL;
					}
					else
					{
						//p->first=ptr->v.p.first;
						//p->last=ptr->v.p.last;
						//*(pp->f)=ptr->v.p.first;
						//*(pp->l)=ptr->v.p.last;
						p=(struct _p*)&(ptr->v.p);
						return p;
					}
				}
			}
		}
	}
	return NULL;
}
struct _p* quickparse(char *str)
{
	struct token tok;
	struct _p *p;
	if(str==NULL) return NULL;
	str=get_token(&tok,str);
	p=NULL;
	p=get_struct_pointer(p,tok.name,tok.num);
	while((strlen(str))!=0)
	{
		p=get_struct_pointer(p,tok.name,tok.num);
		str=get_token(&tok,str);
	}
	return p;
}
void add_s_var(char *path,char *name,int type,void *val)
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
	pst=quickparse(path);
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
//void set_s_var_bis(struct _p *p,char *name,int type,void *val)
//{
//	struct _var *var;
//	int *x;
//	x=val;
//	var=get_s_var(p,name);
//	if(var)
//	{
//		{
//			switch(type){
//			case TYPE_STRING:
//				free(var->s);
//				var->s=strdup((char*)val);
//				if(var->s==NULL) {die("error allocating space for var val");}
//				break;
//			case TYPE_VAL:
//				var->val=*x;
//				break;
//			case TYPE_STRUCT:
//				break;
//			default:
//			}
//		}
//	}
	
//}
void set_s_var(struct _var *var,int type,void *val)
{
	int *x;
	x=val;
	if(var)
	{
		if(var->type!=type) {printf("*** type not matching! ignoring.\n");return;}
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
struct _var* get_s_var(char *name)
{
	struct _gv *ptr;
	struct _p tp;
	struct _p *p;
	p=get_bookmark();
	if(!p)
	{
		tp.first=gv_first;
		tp.last=gv_last;
	}
	else
	{
		tp.first=p->first;
		tp.last=p->last;
	}
	for(ptr=tp.first;ptr;ptr=ptr->next)
	{
		if(strcmp(ptr->v.name,name)==0)
		{
			return &ptr->v;
		}
	}
	return NULL;
}
struct _var* get_s_num_var(char *name,int num)
{
	struct _var *var;
	struct _gv *ptr;
	int n=0;
	var=get_s_var(name);
	if(var)
	{
		ptr=var->p.first;
		while(n++<num)
		{
			ptr=ptr->next;
		}
		return &ptr->v;
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
void del_table(struct _gv *p)
{
	struct _gv *ptr;
	for(ptr=p->v.p.first;ptr;ptr=ptr->next)
	{
		if(ptr->v.type>=TYPE_STRUCT)
		{
			del_table(ptr);
		}
			remove_item(p,ptr);
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
			remove_item(NULL,ptr);
		}
	}
}
void add_table(struct _p *p,struct _gv *st)
{
	struct _gv **f,**l;
	if(p==NULL)
	{
		f=&gv_first;
		l=&gv_last;
	}
	else
	{
		f=&p->first;
		l=&p->last;
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
void make_table(char *path,char *nome,int num)
{
	struct _gv *p,*x;
	struct _p *pointerpath;
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
	pointerpath=quickparse(path);
	add_table(pointerpath,p);
	if(num>=0)
	{
		for(n=0;n<=num;n++)
		{
			x=(struct _gv*)malloc(sizeof(struct _gv));
			if(x==NULL) die("error allocating child table memory");
			x->v.name=strdup(nome);
			if(x->v.name==NULL) die("error allocating child table memory");
			x->v.type=TYPE_STRUCT;
			x->prev=NULL;
			x->next=NULL;
			x->v.p.first=NULL;
			x->v.p.last=NULL;
			add_table((struct _p*)&(p->v.p),x);
			x=NULL;
		}
	}
}
void print_s(struct _var *p)
{
	struct _gv *var;
	int count=0;
	for(var=p->p.first;var;var=var->next)
	{
		if(var->v.type==TYPE_VAL)
		{
			printf("%s : 0x%x (%d)\n",var->v.name,var->v.val,var->v.val);
		}
		if(var->v.type==TYPE_STRING)
		{
			printf("%s : %s\n",var->v.name,var->v.s);
		}
		if(var->v.type>=TYPE_STRUCT)
		{
			if(p->type==TYPE_NODE_STRUCT) printf("*%s[%d] : structure.\n",var->v.name,count++); else
			printf("*%s : structure.\n",var->v.name);
		}
	}
}
void free_bookmark()
{
	bookmark=NULL;
}
void set_bookmark(struct _p *p)
{
	bookmark=p;
}
struct _p* get_bookmark()
{
	return bookmark;
}

