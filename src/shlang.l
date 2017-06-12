/*
 shlang.l : dwarf's shell language lexer.

 (c) 2007-2011 Fernando Iazeolla

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
qstring \"[^"\n]*["\n]
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

{cfmt}{digit}*{letter}		{
	yylval.sWord=strdup(yytext+1);
	return FMT;
}

"-h"		return HUMAN;
"quit"		return QUIT;
"help"		return HELP;
"open"		return LOAD;
"load"		return LOAD;
"fs"		return FILESIZE;
"filesize"	return FILESIZE;
"du"		return DUMP;
"dump"		return DUMP;
"close"		return CLOSE;
"print"		return PRINT;
"pp"		return PRINT;
"grow"		return GROW;
"shrink"	return SHRINK;
"len"		return LEN;
"extract"	return EXTRACT;
"move"		return MOVE;
"inject"	return INJECT;
"@<"		return FILEBEGIN;
"@>"		return FILEEND;
"@@cfg"		return CFG;
"@@maincfg"	return MAINCFG;
"::"		return VAR_IN;
"filelist"	return FILELIST;
"fileuse"	return FILEUSE;
"fl"		return FILELIST;
"fu"		return FILEUSE;
"info"		return INFO;
"save"		return SAVE;
"create"	return CREATE;
">>"		return GROWSYMBOL;
"!>>"		return NOGROWSYMBOL;
"++"		return UPDATESYMBOL;

[-()<>%+@*/;,{}.$!"\[\]:\\=] {
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
