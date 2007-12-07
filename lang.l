%{
#include <stdlib.h>
#include "lang.h"
#include "y.tab.h"
void yyerror(char *);
%}

%%

[0-9]+      {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

0x[0-9a-fA-F]+	{
				sscanf(yytext+2, "%X", &lang_mx);
				yylval.iValue=lang_mx;
				return INTEGER;
			}
			
\$[a-zA-Z_]+	{
					yylval.sVar=strdup(yytext);
					printf("var=%s %s\n",yytext,yylval);
					return VARIABLE;
				}

[-()<>=+*/;{}.!] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
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
"info"			return INFO;
"type"			return TYPE;
"force"			return FORCE;
"sizeof"		return SIZEOF;
"call"			return CALL;
"my"			return LOCAL;
"local"			return LOCAL;
"alias"			return ALIAS;
"shift"			return SHIFT;
"move"			return MOVE;
"realloc"		return REALLOC;
"help"			return HELP;

[a-zA-Z_]+       { 
                	//yylval.inVar = *yytext - 'a';
                	printf("Word %s\n",yytext);
                	return WORD;
            	}

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
