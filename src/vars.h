/*
 vars.h : variables class definition.

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

#ifndef DW_VARS_H
#define DW_VARS_H

enum {VART_NULL,VART_NUM,VART_WORD,VART_STRUCT};
enum {NULLTYPE_VAR,REGULAR_VAR,TMP_VAR};
#define VARNAME_LEN 255

struct _var {
	char *name;
	int type;
	int var_type;
	union {
		char *s;
		off_t num;
	}val;
};
struct _structvar{
	char name[VARNAME_LEN];
	int num;
	struct _structvar *next;
};
struct _fmt{
	char type;
	int rep;
	int null;
};
struct _gvar{		//global var
	struct _var *var;
	struct _gvar *prev,*next;
}*gv_first,*gv_last;

struct _structvar* makestructvar(char *s,off_t num,struct _structvar *next);
struct _var* lookup(char *name);
int setvar(char *name,int type, void *val);
int setvarval(struct _var *ptr,int type, void *val);
struct _var *newvar(char *name);
int addvar(struct _var *var);
int delvar(struct _var *var);
int delallvar(void);
off_t getvarnum(char *name);
off_t var_tonum(struct _var *ptr);
char* var_tostring(struct _var *ptr);
struct _var* createtmpvar(void);
void destroytmpvar(struct _var *var);
struct _fmt* makefmt(char *s);

#endif
