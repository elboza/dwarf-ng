/*
 repl.c : read evaluate print and loop, shell functions.

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
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<stdio.h>
#include<time.h>
#include<readline/readline.h>
#include<readline/history.h>
#include"libdwarf.h"
#include"main.h"
#include"dw_readline_completion.h"
//#include"../../config.h"
#include"repl.h"
#include"utils.h"

extern FILE *yyin;
extern int errno;
extern int yy_scan_string(const char *str);
extern int yyparse (void);
extern void yylex_destroy(void);

char* rl_gets (char *prompt)
{
	static char *line_read = (char *)NULL;
  /* If the buffer has already been allocated,
     return the memory to the free pool. */
  if (line_read)
    {
      free (line_read);
      line_read = (char *)NULL;
    }

  /* Get a line from the user. */
  line_read = readline (prompt);

  /* If the line has any text in it,
     save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}
void execute(char *s)
{
	if(!s) s="quit";
	yy_scan_string(s);
	yyparse();
	yylex_destroy();
}
void run_script(char *file)
{
	FILE *fp;
	fp=fopen(file,"r");
	if(fp==NULL) {printf("error opening %s file.\n",file); return;}
	yyin=fp;
	yyparse();
	fclose(fp);
}
void shell(void)
{
	char *cmd,*prompt;
	reset_stdin();
	quit_shell=false;
	prompt=(char*)malloc(128*sizeof(char));
	if(check_funny()) funny_shell_disclaimer(); else normal_shell_disclaimer();
	printf("entering shell-interactive mode...\n");
	while(!quit_shell)
	{
		#ifdef HAVE_LIBREADLINE
		//cmd=rl_gets("dwarf> ");
		//sprintf(prompt,"%sdwarf>%s ",ptr_colors[C_YELLOW],ptr_colors[C_RESET]);
		make_prompt(prompt);
		cmd=rl_gets(prompt);
		#else
		cmd=(char*)malloc(MAX_CMD);
		fgets(cmd,MAX_CMD,stdin);
		#endif
		execute(cmd);
		#ifndef HAVE_LIBREADLINE
		free(cmd);
		#endif
	}
	free(prompt);
}
void funny_shell_disclaimer()
{
printf("   ________________________________________  \n");
printf("  / dwarf-ng Copyright (C) 2007-2013-2017  \\ \n");
printf("  | Fernando Iazeolla. This program comes  | \n");
printf("  | with ABSOLUTELY NO WARRANTY. This is   | \n");
printf("  | free software (GNU GPLv2), and you are | \n");
printf("  | welcome to redistribute it under GNU   | \n");
printf("  | GPL version 2 (see                     | \n");
printf("  \\ <http://www.gnu.org/licenses/>).       / \n");
printf("   ----------------------------------------  \n");
printf("          \\   ^__^                           \n");
printf("           \\  (oo)\\_______                   \n");
printf("              (__)\\       )\\/\\               \n");
printf("                  ||----w |                  \n");
printf("                  ||     ||                  \n");

}
void normal_shell_disclaimer()
{
	printf("dwarf-ng Copyright (C) 2007-2013-2017 Fernando Iazeolla.\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software (GNU GPLv2), and you are welcome to redistribute it\n");
	printf("under GNU GPL version 2 (see <http://www.gnu.org/licenses/>).\n");
}
int check_funny()
{
	time_t now;
	div_t d;
	
	time(&now);
	d=div(now,10);
	if(d.rem==7) return 1; else return 0;
}
void make_prompt(char *s){
	if(!fc_ptr) {sprintf(s,"%sdwarf>%s ",ptr_colors[C_YELLOW],ptr_colors[C_RESET]);return;}
	sprintf(s,"%s[0x%.10llx]>%s ",ptr_colors[C_YELLOW],fc_ptr->seek,ptr_colors[C_RESET]);
}
