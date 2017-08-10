/*
 cfglang.l : cfg lexer.

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
#include <stdlib.h>
#include "utils.h"
#include "libdwarf.h"
#include "cfglang.tab.h"
void cfgyyerror(char *s);
int cfgyywrap(void);
unsigned int lang_mx;
extern int cfgyylineno;
//extern void cfgyyterminate(void);
%}

digit [0-9]
letter [a-zA-Z_]
hexletter [a-fA-F]
comment #
eq =
identifier {letter}({letter}|{digit})*
filename [0-9a-zA-Z_./]+
qstring \'[^'\n]*['\n]
qqstring \"[^"\n]*["\n]
ws [ \t\r]+
newline \n
%x INCOMMENT

%%

{digit}+	{
				cfgyylval.iValue = atoi(cfgyytext);
				//printf("%d\n",yylval.iValue);
				return INTEGER;
			}

0x({digit}|{hexletter})+	{
				sscanf(cfgyytext+2, "%X", &lang_mx);
				cfgyylval.iValue=lang_mx;
				return INTEGER;
			}

"work_on_tmpcopy"	return CFG_WORKONTMPCOPY;
"verbose"			return CFG_VERBOSE;
"tmpdir"			return CFG_TMPDIR;
"tmpname"			return CFG_TMPNAME;
"colors"			return CFG_COLORS;
"theme"				return CFG_THEME;
{eq}				return EQ;
"yes"				{cfgyylval.iValue=true; return BYES;}
"no"				{cfgyylval.iValue=false;return BNO;}

[-()<>%+@*/;,{}.!"\[\]] {
				return *cfgyytext;
			}


{identifier}	{ 
				//yylval.inVar = *yytext - 'a';
				//printf("Word %s\n",yytext);
				cfgyylval.sWord=strdup(cfgyytext);
				//printf("internal word=%s %s\n",yytext,yylval);
				return WORD;
			}
{filename}	{
				cfgyylval.sWord=strdup(cfgyytext);
				//printf("file type is=%s %s\n",yytext,yylval);
                return FILENAME;
			}
{qstring}	{ 
				cfgyylval.sWord = strdup(yytext+1); 
				if (cfgyylval.sWord[yyleng-2] != '\'') 
				die("improperly terminated string"); 
				else 
				cfgyylval.sWord[yyleng-2] = 0;
				return STRING;
			}
{qqstring}	{ 
				cfgyylval.sWord = strdup(cfgyytext+1); 
				if (cfgyylval.sWord[yyleng-2] != '"') 
				die("improperly terminated string"); 
				else 
				cfgyylval.sWord[cfgyyleng-2] = 0;
				return STRING;
			}
{comment}	{
			//printf("comment:");
			register int c;
			while((c=input())!='\n')
			{if(c==EOF) {/*printf("EOF!\n");*/yyterminate();}}
			//ungetc('\n',cfgyyin);
			unput('\n');
			cfgyylineno++;
			//BEGIN(INCOMMENT);
		}
<INCOMMENT>\n BEGIN(INITIAL);
<INCOMMENT>. ;
<<EOF>>		yyterminate();
{ws}		;	/* ignore whitespace */
{newline}	{cfgyylineno++;return NEWLINE;}

.               cfgyyerror("Unknown character");
%%
int cfgyywrap(void) {
    return 1;
}
