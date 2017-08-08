/*
 cfglang.y : cfg parser.

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
%{
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "libdwarf.h"

/* prototypes */
int cfgyylex(void);
extern char *cfgyytext;
extern int cfgyylineno;
void cfgyyerror(char *s);
//int sym[26];                    /* symbol table */
//The precise list of symbols renamed is yyparse, yylex, yyerror, yynerrs, yylval, yychar and yydebug.
%}

%union {
	int iValue;                 /* integer value */
	char *sWord;					/* internal variable */
};

%token <iValue> INTEGER BYES BNO
%token <sWord>  WORD
%token <sWord>	FILENAME
%token <sWord>	STRING
%token NEWLINE CFG_WORKONTMPCOPY CFG_VERBOSE CFG_TMPDIR CFG_COLORS CFG_THEME CFG_TMPNAME
%type <iValue> bool

%left EQ

%start cfgfile
%%

cfgfile:	commands

commands:	command
		|commands NEWLINE command

		
command: /*empty*/
		|assign
assign:	CFG_WORKONTMPCOPY EQ bool		{cfg.work_on_copy=$3;}
		|CFG_VERBOSE EQ INTEGER			{cfg.verbose=$3;}
		|CFG_TMPDIR EQ STRING			{strncpy(cfg.copydir,$3,VARNAME_LEN);}
		|CFG_TMPNAME EQ STRING			{strncpy(cfg.copyname,$3,VARNAME_LEN);}
        |CFG_COLORS EQ bool             {cfg.theme=1;cfg.colors=$3;}
        |CFG_THEME EQ INTEGER           {cfg.theme=$3;cfg.colors=true;}

bool:	BYES							{$$=true;}
		|BNO							{$$=false;}


%%

void cfgyyerror(char *s) {
    //fprintf(stdout, "%s\n", s);
    fprintf(stdout,"%s on line %d in config file - %s\n", s, cfgyylineno, cfgyytext);
}
