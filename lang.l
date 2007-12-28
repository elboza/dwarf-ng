digit [0-9]
letter [a-zA-Z_]
hexletter [a-fA-F]
%{
#include <stdlib.h>
#include "lang.h"
#include "y.tab.h"
void yyerror(char *);
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

[-()<>=%+*/;{}.!"\[\]] {
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
"open"			return LOAD;
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
"insert"		return INSERT;
"pos"			return POS;
"createh"		return CREATEH;
"show"			return SHOW;

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

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
