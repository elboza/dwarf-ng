/*
 lang.y : the parser.

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
%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "lang.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id_var(char *s);
nodeType *id_word(char *s);
nodeType *id_string(char *s);
nodeType *con(int value);
void freeNode(nodeType *p);
struct _var* ex(nodeType *p);
int yylex(void);

void yyerror(char *s);
//int sym[26];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char *sVar;                /* symbol table variable */
    char *sWord;					/* internal variable */
    char *sStruct;		/*struct*/
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <sVar>   VARIABLE
%token <sWord>  WORD
%token <sWord>	FILENAME
%token <sWord>	STRING
%token WHILE IF PRINT QUIT SAVE LOAD INFO TYPE FORCE SIZEOF CALL LOCAL
%token ALIAS SHIFT MOVE REALLOC HELP INSERT POS CREATEH SHOW CLOSE
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS STRUCT STRUCT1 STRUCTW STRUCTE

%type <nPtr> stmt expr stmt_list filename ivar Tivar

%%

program:
        function                { /*exit(-)*/; }
        ;

function:
          function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | PRINT '%' WORD expr ';'				{$$=opr(PRINT,2,id_word($3),$4);}
        | PRINT expr ';'                 { $$ = opr(PRINT, 2,NULL, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id_var($1), $3); }
        | ivar '=' expr ';' 	         { $$ = opr('=', 2, $1, $3); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'              { $$ = $2; }
	| QUIT							{$$= opr(QUIT,2,NULL,NULL);}
	| SAVE							{$$=opr(QUIT,2,NULL,NULL);}
        | LOAD filename						{$$=opr(LOAD,1,$2);}
        | LOAD '(' filename ')'					{$$=opr(LOAD,1,$3);}
        | INFO							{$$=opr(QUIT,2,NULL,NULL);}
        | TYPE							{$$=opr(TYPE,2,NULL,NULL);}
        | FORCE							{$$=opr(QUIT,2,NULL,NULL);}
        | SIZEOF						{$$=opr(QUIT,2,NULL,NULL);}
        | CALL							{$$=opr(QUIT,2,NULL,NULL);}
        | LOCAL							{$$=opr(QUIT,2,NULL,NULL);}
        | ALIAS							{$$=opr(QUIT,2,NULL,NULL);}
        | SHIFT							{$$=opr(QUIT,2,NULL,NULL);}
        | MOVE							{$$=opr(QUIT,2,NULL,NULL);}
        | REALLOC						{$$=opr(QUIT,2,NULL,NULL);}
        | HELP							{$$=opr(QUIT,2,NULL,NULL);}
        | INSERT						{$$=opr(QUIT,2,NULL,NULL);}
        | POS							{$$=opr(QUIT,2,NULL,NULL);}
        | CREATEH						{$$=opr(QUIT,2,NULL,NULL);}
        | SHOW							{$$=opr(QUIT,2,NULL,NULL);}
        | CLOSE							{$$=opr(CLOSE,2,NULL,NULL);}
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | VARIABLE              { $$ = id_var($1); }
        | ivar					{ $$ =$1; }
        | FILENAME					{ $$ =id_word($1); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        | STRING				{$$=id_string($1);}
        ;

filename:
		  FILENAME				{$$=id_word($1);}
		| WORD					{$$=id_word($1);}
		;
		
ivar:
		Tivar					{$$=$1;}
		| Tivar '-' '>' ivar			{$$=opr(STRUCT,2,$1,$4);}
		;

Tivar:
		  WORD '(' WORD ')'			{$$=opr(STRUCTW,2,id_word($1),id_word($3));}
		| WORD '[' expr ']'			{$$=opr(STRUCTE,2,id_word($1),$3);}
		| WORD					{$$=opr(STRUCT1,1,id_word($1));}
		;
%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id_var(char *s) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeVar;
    p->id.s = s;

    return p;
}

nodeType *id_word(char *s) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeWord;
    p->id.s = s;

    return p;
}
nodeType *id_string(char *s) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeString;
    p->id.s = s;

    return p;
}
nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

//int main(void) {
//    yyparse();
//    return 0;
//}
