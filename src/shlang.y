/*
 shlang.y : dwarf's shell language parser.

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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include"libdwarf.h"
#include"repl.h"
#include"output.h"
#include "sh_elf_utils.h"
#include "dw_readline_completion.h"
#include "sh_switchers.h"
#include "utils.h"

/* prototypes */
int yylex(void);
extern char *yytext;
extern int yylineno;
void yyerror(char *s);
//int sym[26];                    /* symbol table */
struct _var *var;
%}

%union {
	off_t iValue;                 /* integer value */
	char *sWord;					/* internal variable */
	struct _structvar *sVar;
	struct _fmt *sFmt;
};

%token <iValue> INTEGER
%token <sWord>  WORD FMT
%token <sWord>	FILENAME
%token <sWord>	STRING
%token QUIT HELP LOAD FILESIZE DUMP CLOSE PRINT HUMAN GROW SHRINK LEN
%token EXTRACT MOVE INJECT FILEBEGIN FILEEND CFG VAR_IN MAINCFG
%token FILELIST FILEUSE INFO SAVE CREATE GROWSYMBOL NOGROWSYMBOL
%token UPDATESYMBOL 
%type <iValue> maybehelpcommand
%type <sVar>	svar maybenext
%type <iValue> expr maybenum offset maybeendoffset
%type <sWord> filename cfgparam maybesavename grow maybeupdate
%type <sFmt> fmt
%left EQ
%left '+' '-'
%left '*' '/'

%start inputdata
%%

inputdata:	commands

commands:	command
			|commands ';' command
			|/*empty*/
			/*|commands comment commands*/

command: /*empty*/
		|QUIT							{dw_quit();quit_shell=true;YYACCEPT;}
		|HELP maybehelpcommand			{/*printf("help!\n");*/}
		|LOAD filename					{file_open($2);free_completion();add_sh_completion();}
		|FILESIZE						{do_filesize(fc_ptr,false);}
		|FILESIZE HUMAN					{do_filesize(fc_ptr,true);}
		|DUMP fmt expr					{do_dump($2,$3);}
		|CLOSE							{file_close();free_completion();}
		|PRINT STRING					{printf("%s\n",$2);}
		|PRINT fmt svar					{do_print_s_var($2,$3);}
		|PRINT fmt expr					{do_print_expr($2,$3);}
		|PRINT fmt WORD					{do_printvar($2,$3);}
		|PRINT CFG cfgparam				{do_printcfg(fc_ptr,$3);}
		|PRINT MAINCFG cfgparam			{do_printcfg(&cfg,$3);}
		|'{' commands '}'				{}
		|GROW expr						{growth($2);}
		|SHRINK expr					{shrink($2);}
		|LEN '+' expr					{mod_len($3);}
		|LEN '-' expr					{mod_len(-$3);}
		|EXTRACT expr expr filename		{extract($2,$3,$4);}
		|MOVE expr expr expr			{move($2,$3,$4);}
		|MOVE expr '+' expr expr		{move_r_pos($2,$4,$5);}
		|MOVE expr '-' expr expr		{move_r_pos($2,$4,$5);}
		|INJECT expr expr expr			{inject_byte($2,$3,$4,false);}
		|INJECT expr expr expr STRING	{do_inject_byte($2,$3,$4,$5);}
		|INJECT filename expr expr		{inject_file($2,$3,$4,false);}
		|INJECT filename expr expr STRING {do_inject_file($2,$3,$4,$5);}
		|FILELIST						{do_filelist();}
		|FILEUSE expr					{do_fileuse($2);}
		|WORD '=' expr					{setvar($1,VART_NUM,(void*)$3);}
		|WORD '=' STRING				{setvar($1,VART_WORD,$3);}
  |svar '=' expr					{var=createtmpvar();if(var){setvarval(var,VART_NUM,(void*)$3);set_s_val($1,var);}}
  |svar '=' STRING				{var=createtmpvar();if(var){setvarval(var,VART_WORD,$3);set_s_val($1,var);}}
		|INFO							{sw_do_info();}
		|'!' STRING						{system($2);}
		|SAVE maybesavename				{file_save($2);}
		|CREATE STRING expr grow maybeupdate		{do_create($2,$3,$4,$5);}

expr:	INTEGER							{$$=$1;}
		|expr '+' expr					{$$=$1+$3;}
		|expr '-' expr					{$$=$1-$3;}
		|expr '*' expr					{$$=$1*$3;}
		|expr '/' expr					{$$=$1/$3;}
		|'(' expr ')'					{$$=$2;}
		|FILEBEGIN						{$$=0;}
		|FILEEND						{$$=do_getfilesize(fc_ptr);}
		|WORD							{$$=getvarnum($1);}
		|svar							{$$=(long long)var_tonum(get_s_val($1));}
		|offset							{$$=$1;}

filename:	WORD						{$$=$1;}
			|FILENAME					{$$=$1;}
			|STRING						{$$=$1;}

cfgparam:	/*empty*/					{$$="\0";}
			|VAR_IN WORD				{$$=$2;}
			
svar:	'$' WORD maybenum maybenext		{$$=makestructvar($2,$3,$4);}

maybenum:	/*empty*/					{$$=-1;}
			| '[' expr ']'				{$$=$2;}

maybenext:	/*empty*/					{$$=NULL;}
			|VAR_IN WORD maybenum maybenext	{$$=makestructvar($2,$3,$4);}

offset:		'@' WORD maybenum maybenext maybeendoffset		{$$=get_offset(makestructvar($2,$3,$4),$5);}
maybeendoffset:	/*empty*/				{$$=0;}
			|'<'						{$$=0;}
			|'>'						{$$=1;}

fmt:	FMT								{$$=makefmt($1);}
			|/*empty*/					{$$=makefmt(NULL);}

maybehelpcommand: /*empty*/				{show_help_base();}
			|STRING						{help_cmd($1);}

maybesavename:	/*empty*/				{$$=NULL;}
			|filename					{$$=$1;}

grow:		GROWSYMBOL						{$$=1;}
			|NOGROWSYMBOL					{$$=0;}

maybeupdate:	/*empty*/				{$$=0;}
			|UPDATESYMBOL				{$$=1;}

%%

void yyerror(char *s) {
    //fprintf(stdout, "%s\n", s);
    fprintf(stdout,"%s on line %d - %s\n", s, yylineno, yytext);
}
