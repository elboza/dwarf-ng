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
%token <sWord>  WORD FMT HEX_WORD
%token <sWord>	FILENAME
%token <sWord>	STRING
%token QUIT HELP LOAD FILESIZE CLOSE PRINT HUMAN DW_GROW DW_SHRINK DW_RESIZE
%token EXTRACT MOVE INJECT FILEBEGIN FILEEND CFG VAR_IN MAINCFG
%token FILELIST FILEUSE INFO SAVE CREATE GROWSYMBOL NOGROWSYMBOL
%token UPDATESYMBOL DWP_PRINT_CFG DW_PRINT_MAINCFG 
%token SHOW_HELP_MOVE SHOW_HELP_OPEN SHOW_HELP_CONFIG SHOW_HELP_PRINT SHOW_HELP_PPRINT DW_SHOW_HELP_WRITE DW_SHOW_HELP_WRITEOVER
%token DW_DUMP_HEX DW_DUMP_HEXX DW_DUMP_HEX_LINES DW_DUMP_STRING
%token DW_BLOCK_CMD BLOCK_HELP BLOCK_INC BLOCK_DEC
%token DW_SEEK_HELP DW_SEEK_CMD DW_SEEK_BACK DW_SEEK_FWD DW_SEEK_BLOCK_BACK DW_SEEK_BLOCK_FWD DW_SEEK_HISTORY DW_SEEK_DATA DW_SEEK_HEX
%token DW_WRITE_HEX DW_WRITE_HEX_INC DW_WRITE_STRING DW_WRITE_STRING_INC DW_WRITE_STRINGZ DW_WRITE_STRINGZ_INC DW_WRITE_PATTERN_STRING DW_WRITE_PATTERN_STRING_INC DW_WRITE_PATTERN_HEX DW_WRITE_PATTERN_HEX_INC DW_WRITE_FILE DW_WRITE_FILE_INC DW_WRITE_RANDOM DW_WRITE_RANDOM_INC DW_WRITE_LE DW_WRITE_LE_INC DW_WRITE_BE DW_WRITE_BE_INC DW_WRITE_NUMBER DW_WRITE_NUMBER_INC
%token DW_WO_ADD DW_WO_AND DW_WO_SUB DW_WO_RSHIFT DW_WO_MUL DW_WO_LSHIFT DW_WO_OR DW_WO_XOR DW_WO_2SWAP DW_WO_4SWAP DW_WO_8SWAP
%token DW_SECT_HELP DW_SECT_CREATE_INC 
%token DW_OPEN_NEW DW_OPEN_TYPE DW_OPEN_LIGHT DW_OPEN_PROBE 
%type <iValue> maybehelpcommand
%type <sVar>	svar maybenext
%type <iValue> expr maybenum offset maybeendoffset grow maybeupdate
%type <sWord> filename cfgparam maybesavename  cfgparam2
%type <sFmt> fmt
%left EQ
%left '+' '-'
%left '*' '/'

%start inputdata
%%

inputdata:	commands

commands:	command
			|commands ';' command


command: /*empty*/
		|QUIT							{dw_quit();quit_shell=true;YYACCEPT;}
		|HELP maybehelpcommand			{/*printf("help!\n");*/}
		|LOAD filename					{file_open($2,true);free_completion();add_sh_completion();}
		|FILESIZE						{do_filesize(fc_ptr,false);}
		|FILESIZE HUMAN					{do_filesize(fc_ptr,true);}
		|CLOSE							{file_close();free_completion();}
		|PRINT STRING					{printf("%s\n",$2);}
		|PRINT fmt svar					{do_print_s_var($2,$3);}
		|PRINT fmt expr					{do_print_expr($2,$3);}
		|PRINT fmt WORD					{do_printvar($2,$3);}
		|PRINT fmt '@' expr {do_print_offset($2,$4);}
		|PRINT CFG cfgparam				{do_printcfg(fc_ptr,$3);}
		|PRINT MAINCFG cfgparam			{do_printcfg(&cfg,$3);}
		|DWP_PRINT_CFG cfgparam2			{do_printcfg(fc_ptr,$2);}
		|DW_PRINT_MAINCFG cfgparam2		{do_printcfg(&cfg,$2);}
		|'{' commands '}'				{}
		|DW_GROW expr						{growth($2);}
		|DW_SHRINK expr					{shrink($2);}
		|DW_RESIZE '+' expr					{mod_len($3);}
		|DW_RESIZE '-' expr					{mod_len(-$3);}
		|EXTRACT expr expr filename		{extract($2,$3,$4);}
		|MOVE expr expr expr			{move($2,$3,$4);}
		|MOVE expr '+' expr expr		{move_r_pos($2,$4,$5);}
		|MOVE expr '-' expr expr		{move_r_pos($2,$4,$5);}
		|SHOW_HELP_MOVE {show_help_move();}
		|INJECT expr expr expr			{inject_byte($2,$3,$4,false);}
		|INJECT expr expr expr grow	{inject_byte($2,$3,$4,$5);}
		|INJECT filename expr expr		{inject_file($2,$3,$4,false);}
		|INJECT filename expr expr grow {inject_file($2,$3,$4,$5);}
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
		|BLOCK_HELP {block_help_func();}
		|DW_BLOCK_CMD expr {block_func(true,$2);}
		|DW_BLOCK_CMD {block_func(false,0);}
		|BLOCK_INC expr {block_inc_func($2);}
		|BLOCK_DEC expr {block_dec_func($2);}
		|DW_SEEK_HELP {seek_help_func();}
		|DW_SEEK_CMD {show_current_seek();}
		|DW_SEEK_CMD expr {set_current_seek($2);}
		|DW_SEEK_BACK {printf("still to code...\n");}
		|DW_SEEK_FWD {printf("still to code...\n");}
		|DW_SEEK_BACK expr {seek_dec($2);}
		|DW_SEEK_FWD expr {seek_inc($2);}
		|DW_SEEK_BLOCK_BACK {seek_block_dec();}
		|DW_SEEK_BLOCK_FWD {seek_block_inc();}
		|DW_SEEK_HISTORY {printf("still to code...\n");}
		|DW_SEEK_DATA WORD {seek_data($2);}
		|DW_SEEK_HEX STRING {seek_hex_data($2);}
		|SHOW_HELP_OPEN {show_help_open();}
		|SHOW_HELP_CONFIG {show_help_config();}
		|SHOW_HELP_PRINT {show_help_print();}
		|SHOW_HELP_PPRINT {show_help_pprint();}
		|DW_DUMP_HEX fmt {do_dump_hex($2,0,false,false);}
		|DW_DUMP_HEX fmt expr {do_dump_hex($2,$3,true,false);}
		|DW_DUMP_HEXX fmt {do_dump_hex($2,0,false,true);}
		|DW_DUMP_HEXX fmt expr {do_dump_hex($2,$3,true,true);}
		|DW_DUMP_HEX_LINES expr {do_dump_hex_lines($2,0,false);}
		|DW_DUMP_HEX_LINES expr '@' expr {do_dump_hex_lines($2,$4,true);}
		|DW_DUMP_STRING fmt {do_dump_string($2,0,false);}
		|DW_DUMP_STRING fmt expr {do_dump_string($2,$3,true);}
		|DW_SHOW_HELP_WRITE {show_help_write();}
		|DW_SHOW_HELP_WRITEOVER {show_help_writeover();}
		|DW_WRITE_HEX fmt STRING {dw_write_hex($2,$3,0,false,false);}
		|DW_WRITE_HEX fmt STRING expr {dw_write_hex($2,$3,$4,true,false);}
		|DW_WRITE_HEX_INC fmt STRING {dw_write_hex($2,$3,0,false,true);}
		|DW_WRITE_HEX_INC fmt STRING expr {dw_write_hex($2,$3,$4,true,true);}
		|DW_WRITE_STRING fmt STRING {dw_write_string($2,$3,0,false,false,false);}
		|DW_WRITE_STRING fmt STRING expr {dw_write_string($2,$3,$4,true,false,false);}
		|DW_WRITE_STRING_INC fmt STRING {dw_write_string($2,$3,0,false,true,false);}
		|DW_WRITE_STRING_INC fmt STRING expr {dw_write_string($2,$3,$4,true,true,false);}
		|DW_WRITE_STRINGZ fmt STRING {dw_write_string($2,$3,0,false,false,true);}
		|DW_WRITE_STRINGZ fmt STRING expr {dw_write_string($2,$3,$4,true,false,true);}
		|DW_WRITE_STRINGZ_INC fmt STRING {dw_write_string($2,$3,0,false,true,true);}
		|DW_WRITE_STRINGZ_INC fmt STRING expr {dw_write_string($2,$3,$4,true,true,true);}
		|DW_WRITE_PATTERN_STRING fmt STRING {dw_write_pattern($2,$3,0,false,false,DW_PATTERN_STRING);}
		|DW_WRITE_PATTERN_STRING fmt STRING expr {dw_write_pattern($2,$3,$4,true,false,DW_PATTERN_STRING);}
		|DW_WRITE_PATTERN_STRING_INC fmt STRING {dw_write_pattern($2,$3,0,false,true,DW_PATTERN_STRING);}
		|DW_WRITE_PATTERN_STRING_INC fmt STRING expr{dw_write_pattern($2,$3,$4,true,true,DW_PATTERN_STRING);}
		|DW_WRITE_PATTERN_HEX fmt STRING {dw_write_pattern($2,$3,0,false,false,DW_PATTERN_HEX);}
		|DW_WRITE_PATTERN_HEX fmt STRING expr {dw_write_pattern($2,$3,$4,true,false,DW_PATTERN_HEX);}
		|DW_WRITE_PATTERN_HEX_INC fmt STRING {dw_write_pattern($2,$3,0,false,true,DW_PATTERN_HEX);}
		|DW_WRITE_PATTERN_HEX_INC fmt STRING offset {dw_write_pattern($2,$3,$4,true,true,DW_PATTERN_HEX);}
		|DW_WRITE_FILE fmt filename {dw_write_file($2,$3,0,false,false);}
		|DW_WRITE_FILE fmt filename expr {dw_write_file($2,$3,$4,true,false);}
		|DW_WRITE_FILE_INC fmt filename {dw_write_file($2,$3,0,false,true);}
		|DW_WRITE_FILE_INC fmt filename expr {dw_write_file($2,$3,$4,true,true);}
		|DW_WRITE_RANDOM fmt {dw_write_random($2,0,false,false);}
		|DW_WRITE_RANDOM fmt expr {dw_write_random($2,$3,true,false);}
		|DW_WRITE_RANDOM_INC fmt {dw_write_random($2,0,false,true);}
		|DW_WRITE_RANDOM_INC fmt expr {dw_write_random($2,$3,true,true);}
		|DW_WRITE_LE fmt expr {dw_write_le($2,$3,0,false,false);}
		|DW_WRITE_LE fmt expr expr {dw_write_le($2,$3,$4,true,false);}
		|DW_WRITE_LE_INC fmt expr {dw_write_le($2,$3,0,false,true);}
		|DW_WRITE_LE_INC fmt expr expr {dw_write_le($2,$3,$4,true,true);}
		|DW_WRITE_BE fmt expr {dw_write_be($2,$3,0,false,false);}
		|DW_WRITE_BE fmt expr expr {dw_write_be($2,$3,$4,true,false);}
		|DW_WRITE_BE_INC fmt expr {dw_write_be($2,$3,0,false,true);}
		|DW_WRITE_BE_INC fmt expr expr {dw_write_be($2,$3,$4,true,true);}
		|DW_WRITE_NUMBER fmt expr {dw_write_number($2,$3,0,false,false);}
		|DW_WRITE_NUMBER fmt expr expr {dw_write_number($2,$3,$4,true,false);}
		|DW_WRITE_NUMBER_INC fmt expr {dw_write_number($2,$3,0,false,true);}
		|DW_WRITE_NUMBER_INC fmt expr expr {dw_write_number($2,$3,$4,true,true);}
		|DW_WO_ADD fmt expr {dw_write_over($2,$3,0,false,E_WO_ADD);}
		|DW_WO_ADD fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_ADD);}
		|DW_WO_AND fmt expr {dw_write_over($2,$3,0,false,E_WO_AND);}
		|DW_WO_AND fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_AND);}
		|DW_WO_SUB fmt expr {dw_write_over($2,$3,0,false,E_WO_SUB);}
		|DW_WO_SUB fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_SUB);}
		|DW_WO_RSHIFT fmt expr {dw_write_over($2,$3,0,false,E_WO_RSHIFT);}
		|DW_WO_RSHIFT fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_RSHIFT);}
		|DW_WO_MUL fmt expr {dw_write_over($2,$3,0,false,E_WO_MUL);}
		|DW_WO_MUL fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_MUL);}
		|DW_WO_LSHIFT fmt expr {dw_write_over($2,$3,0,false,E_WO_LSHIFT);}
		|DW_WO_LSHIFT fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_LSHIFT);}
		|DW_WO_OR fmt expr {dw_write_over($2,$3,0,false,E_WO_OR);}
		|DW_WO_OR fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_OR);}
		|DW_WO_XOR fmt expr {dw_write_over($2,$3,0,false,E_WO_XOR);}
		|DW_WO_XOR fmt expr expr {dw_write_over($2,$3,$4,true,E_WO_XOR);}
		|DW_WO_2SWAP {dw_write_over_2swap(0,false);}
		|DW_WO_2SWAP expr {dw_write_over_2swap($2,true);}
		|DW_WO_4SWAP {dw_write_over_4swap(0,false);}
		|DW_WO_4SWAP expr {dw_write_over_4swap($2,true);}
		|DW_WO_8SWAP {dw_write_over_8swap(0,false);}
		|DW_WO_8SWAP expr {dw_write_over_8swap($2,true);}
		|DW_SECT_HELP {show_help_section();}
		|DW_SECT_CREATE_INC filename {dw_create_section($2,0,false);}
		|DW_SECT_CREATE_INC filename expr {dw_create_section($2,$3,true);}
		|DW_OPEN_NEW fmt {dw_open_create($2,NULL,NULL);}
		|DW_OPEN_NEW fmt filename {dw_open_create($2,$3,NULL);}
		|DW_OPEN_NEW fmt filename '!' WORD {dw_open_create($2,$3,$5);}
		|DW_OPEN_NEW fmt '!' WORD {dw_open_create($2,NULL,$4);}
		|DW_OPEN_TYPE filename {dw_open_type($2);}
		|DW_OPEN_LIGHT filename {file_open($2,false);}
		|DW_OPEN_PROBE {free_completion();file_probe();add_sh_completion();}


expr:	INTEGER							{$$=$1;}
		|expr '+' expr					{$$=$1+$3;}
		|expr '-' expr					{$$=$1-$3;}
		|expr '*' expr					{$$=$1*$3;}
		|expr '/' expr					{$$=$1/$3;}
		|'(' expr ')'					{$$=$2;}
		|WORD							{$$=getvarnum($1);}
		|svar							{$$=(long long)var_tonum(get_s_val($1));}
		|offset							{$$=$1;}

filename:	WORD						{$$=$1;}
			|FILENAME					{$$=$1;}
			|STRING						{$$=$1;}

cfgparam:	/*empty*/					{$$="\0";}
			|VAR_IN WORD				{$$=$2;}

cfgparam2:	/*empty*/					{$$="\0";}
			|WORD						{$$=$1;}
			
svar:	'$' WORD maybenum maybenext		{$$=makestructvar($2,$3,$4);}

maybenum:	/*empty*/					{$$=-1;}
			| '[' expr ']'				{$$=$2;}

maybenext:	/*empty*/					{$$=NULL;}
			|VAR_IN WORD maybenum maybenext	{$$=makestructvar($2,$3,$4);}

offset:		'@' WORD maybenum maybenext maybeendoffset		{$$=get_offset(makestructvar($2,$3,$4),$5);}
			|FILEBEGIN						{$$=0;}
			|FILEEND						{$$=do_getfilesize(fc_ptr);}
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
