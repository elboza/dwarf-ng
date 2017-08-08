/*
 shlang.l : dwarf's shell language lexer.

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
#include <stdlib.h>
#include "shlang.tab.h"
#include "utils.h"
void yyerror(char *s);
int yywrap(void);
unsigned int lang_mx;
%}

digit [0-9]
letter [a-zA-Z_]
hexletter [a-fA-F]
comment #
cfmt %
identifier {letter}({letter}|{digit})*
filename [0-9a-zA-Z_./]+
qstring \'[^'\n]*['\n]
qqstring \"[^"\n]*["\n]
ws [ \t\r]+
newline \n

%%

{digit}+	{
				yylval.iValue = atoi(yytext);
				//printf("%d\n",yylval.iValue);
				return INTEGER;
			}

0x({digit}|{hexletter})+	{
				sscanf(yytext+2, "%X", &lang_mx);
				yylval.iValue=lang_mx;
				return INTEGER;
			}

{cfmt}{digit}*{letter}?		{
	yylval.sWord=strdup(yytext+1);
	return FMT;
}


"-h"		return HUMAN;
"quit"		return QUIT;
"help"		return HELP;
"close"		return CLOSE;
"pp"		return PRINT;
"move"		return MOVE;
"inject"	return INJECT;
"@<"		return FILEBEGIN;
"@>"		return FILEEND;
"$cfg"		return CFG;
"$maincfg"	return MAINCFG;
"::"		return VAR_IN;
"info"		return INFO;
"save"		return SAVE;
"create"	return CREATE;
">>"		return GROWSYMBOL;
"!>>"		return NOGROWSYMBOL;
"++"		return UPDATESYMBOL;

"q"			return QUIT;
"?"			return HELP;
"i"			return INFO;
"o"			return LOAD;
"o?"		return SHOW_HELP_OPEN;
"oc"		return CLOSE;
"ol"		return FILELIST;
"os"		return FILESIZE;
"ou"		return FILEUSE;
"e"			return DWP_PRINT_CFG;
"em"		return DW_PRINT_MAINCFG;
"e?"		return SHOW_HELP_CONFIG;
"oR+"		return DW_GROW;
"oR-"		return DW_SHRINK;
"oR"		return DW_RESIZE;
"oS"		return SAVE;
"b"			return DW_BLOCK_CMD;
"b?"		return BLOCK_HELP;
"b+"		return BLOCK_INC;
"b-"		return BLOCK_DEC;
"s?"		return DW_SEEK_HELP;
"s"			return DW_SEEK_CMD;
"s-"		return DW_SEEK_BACK;
"s+"		return DW_SEEK_FWD;
"s--"		return DW_SEEK_BLOCK_BACK;
"s++"		return DW_SEEK_BLOCK_FWD;
"s*"		return DW_SEEK_HISTORY;
"s/"		return DW_SEEK_DATA;
"s/x"		return DW_SEEK_HEX;
"M"			return MOVE;
"M?"		return SHOW_HELP_MOVE;
"p?"		return SHOW_HELP_PRINT;
"pp?"		return SHOW_HELP_PPRINT;
"x?"		return SHOW_HELP_PRINT;
"px"		return DW_DUMP_HEX;
"x"			return DW_DUMP_HEX;
"xx"		return DW_DUMP_HEXX;
"pxx"		return DW_DUMP_HEXX;
"xl"		return DW_DUMP_HEX_LINES;
"ps"		return DW_DUMP_STRING;
"w?"		return DW_SHOW_HELP_WRITE;
"wo?"		return DW_SHOW_HELP_WRITEOVER;
"wx"		return DW_WRITE_HEX;
"wx+"		return DW_WRITE_HEX_INC;
"ws"		return DW_WRITE_STRING;
"ws+"		return DW_WRITE_STRING_INC;
"wz"		return DW_WRITE_STRINGZ;
"wz+"		return DW_WRITE_STRINGZ_INC;
"wps"		return DW_WRITE_PATTERN_STRING;
"wps+"		return DW_WRITE_PATTERN_STRING_INC;
"wpx"		return DW_WRITE_PATTERN_HEX;
"wpx+"		return DW_WRITE_PATTERN_HEX_INC;
"wf"		return DW_WRITE_FILE;
"wf+"		return DW_WRITE_FILE_INC;
"wr"		return DW_WRITE_RANDOM;
"wr+"		return DW_WRITE_RANDOM_INC;
"wle"		return DW_WRITE_LE;
"wle+"		return DW_WRITE_LE_INC;
"wbe"		return DW_WRITE_BE;
"wbe+"		return DW_WRITE_BE_INC;
"wn"		return DW_WRITE_NUMBER;
"wn+"		return DW_WRITE_NUMBER_INC;
"woa"		return DW_WO_ADD;
"woA"		return DW_WO_AND;
"wos"		return DW_WO_SUB;
"wol"		return DW_WO_LSHIFT;
"wom"		return DW_WO_MUL;
"wor"		return DW_WO_RSHIFT;
"woo"		return DW_WO_OR;
"wox"		return DW_WO_XOR;
"wo2"		return DW_WO_2SWAP;
"wo4"		return DW_WO_4SWAP;
"wo8"		return DW_WO_8SWAP;
"S?"		return DW_SECT_HELP;
"Sc+"		return DW_SECT_CREATE_INC;
"oN"		return DW_OPEN_NEW;
"oT"		return DW_OPEN_TYPE;
"oO"		return DW_OPEN_LIGHT;
"oP"		return DW_OPEN_PROBE;
"oL"		return DW_OPEN_PROBE;



[-()<>%+@*/;?,{}.$!"\[\]:\\=] {
				return *yytext;
			}


{identifier}	{ 
				//yylval.inVar = *yytext - 'a';
				//printf("Word %s\n",yytext);
				yylval.sWord=strdup(yytext);
				//printf("internal word=%s %s\n",yytext,yylval);
				return WORD;
			}
{filename}	{
				yylval.sWord=strdup(yytext);
				//printf("file type is=%s %s\n",yytext,yylval);
                return FILENAME;
			}
{qstring}	{ 
				yylval.sWord = strdup(yytext+1); 
				if (yylval.sWord[yyleng-2] != '\'') 
				die("improperly terminated string"); 
				else 
				yylval.sWord[yyleng-2] = 0;
				return STRING;
			}
{qqstring}	{ 
				yylval.sWord = strdup(yytext+1); 
				if (yylval.sWord[yyleng-2] != '"') 
				die("improperly terminated string"); 
				else 
				yylval.sWord[yyleng-2] = 0;
				return STRING;
			}
{comment}	{
			//printf("comment:");
			register int c;
			while((c=input())!='\n')
			{if(c==EOF) {/*printf("EOF!\n");*/yyterminate();}}
			//printf("::%x\n",ungetc(' ',yyin));
			yylineno++;
		}
<<EOF>>		yyterminate();
{ws}		;	/* ignore whitespace */
{newline}	yylineno++;

.			{printf("%s ",yytext);yyerror("Unknown character");}
%%
int yywrap(void) {
    return 1;
}
