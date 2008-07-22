/*
 lang.l : the lexical analyzer.

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
digit [0-9]
letter [a-zA-Z_]
hexletter [a-fA-F]
%{
#include <stdlib.h>
#include"stdint.h"
#include "utils.h"
#include "lang.h"
#include "y.tab.h"
void yyerror(char *);
int yywrap(void);
%}

%%

{digit}+      {
                yylval.iValue = atoi(yytext);
                //printf("%d\n",yylval.iValue);
                return INTEGER;
            }

0x({digit}|{hexletter})+	{
				sscanf(yytext+2, "%X", &lang_mx);
				yylval.iValue=lang_mx;
				return INTEGER;
			}
			
\${letter}({letter}|{digit})*	{
					yylval.sVar=strdup(yytext);
					//printf("var=%s %s\n",yytext,yylval);
					return VARIABLE;
				}

[-()<>=%+@*/;,{}.!"\[\]] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"@<"		return FILE_BEGIN;
"@>"		return FILE_END;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"       	return PRINT;
"quit"			return QUIT;
"pp"			return PRINT;
"exit"			return QUIT;
"save"			return SAVE;
"write"			return SAVE;
"load"			return LOAD;
"read"			return LOAD;
"open"			return LOAD;
"info"			return INFO;
"type"			return TYPE;
"force"			return FORCE;
"sizeof"		return SIZEOF;
"flush"			return FLUSH;
"grouth"		return GROUTH;
"shrink"		return SHRINK;
"refresh"		return REFRESH;
"reload"		return RELOAD;
"shift"			return SHIFT;
"move"			return MOVE;
"help"			return HELP;
"inject"		return INJECT;
"create"		return ADDHD;
"remove"		return RMHD;
"len"			return LEN;
"new"			return NEW;
"show"			return SHOW;
"close"			return CLOSE;
"dump"			return DUMP;

{letter}({letter}|{digit})*       { 
                	//yylval.inVar = *yytext - 'a';
                	//printf("Word %s\n",yytext);
                	yylval.sWord=strdup(yytext);
			//printf("internal word=%s %s\n",yytext,yylval);
                	return WORD;
            	}
[0-9a-zA-Z_./]+	{
			yylval.sWord=strdup(yytext);
			//printf("file type is=%s %s\n",yytext,yylval);
                	return FILENAME;
            	}
\"[^"\n]*["\n] { 
               yylval.sWord = strdup(yytext+1); 
               if (yylval.sWord[yyleng-2] != '"') 
                   die("improperly terminated string"); 
               else 
                   yylval.sWord[yyleng-2] = 0;
                   return STRING;
                       }


[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
