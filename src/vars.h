/*
 vars.h : variables header definitions.

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

#ifndef DW_VARS_H
#define DW_VARS_H

enum {TYPE_NULL,TYPE_STRING,TYPE_VAL,TYPE_STRUCT,TYPE_NODE_STRUCT};
enum {S_MAIN,S_PH,S_SH,S_LC};
struct _var
{
	char *name;
	int type;
	int len;
	union{
		int val;
		char *s;
		struct {
			struct _gv *first,*last;
		}p;
	};
};
struct _p
{
	struct _gv *first,*last;
}*bookmark;
struct _pp
{
	struct _gv **f,**l;
};
struct token
{
	char name[255];
	int num;
};
struct _gv
{
	struct _var v;
	struct _gv *prev,*next;
} *gv_first,*gv_last,*gv_ptr,*s_first,*s_last,*s_ptr;
void set_var(char *name,int type,void *val);
struct _var* get_s_var(char *name);
struct _var* get_var(char *name);
struct _var* get_s_var_byname(char *path,char *varname);
struct _var* get_s_var_bypointer(struct _p *p,char *varname);
struct _stack_s
{
	char *name;
	struct _stack_s *prev;
}*last_stack;
void free_bookmark(void);
void set_bookmark(struct _p *p);
struct _p* get_bookmark(void);
//void push(char *s);
//struct _var* peek(void);
//struct _var* pop(void);
//void s_remove(void);

#endif

