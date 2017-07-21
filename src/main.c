/*
 main.c : dwarf's main file.

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
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<getopt.h>
#include"libdwarf.h"
#include"repl.h"
#include "config.h"
#include "dw_readline_completion.h"
#include "sh_switchers.h"
#include "utils.h"


struct m_action{
	int file;
	int shell;
	int script;
	int exec;
	int stdin;
};
void usage()
{
  printf("dwarf-ng-%s (c) Fernando Iazeolla 2007-2013-2017.\n",VERSION);
	printf("\nUSAGE: ");
	printf("dwarf [options] [file]\n");
	printf("valid options:\n");
	printf("-s              --stdin                      file from stdin\n");
	printf("-i              --shell --interactive        interactive (shell mode)\n");
	printf("-h              --help                       show this help\n");
	printf("-c '<commands>' --command '<commands>'       command  mode (execute commands)\n");
	printf("-x <file>       --execute <file>             execute file script\n");
	printf("-C		--colors		     colored output\n");
	printf("-B		--nocolors		     nocolored output\n");
	exit(1);
}
void usage_b()
{
	//char *s="VERSION";
	printf("dwarf-ng-%s (c) Fernando Iazeolla 2007-2013-2017\n",VERSION);
	printf("for help type: dwarf --help\n");
}
void parse_args(int argc,char **argv,struct m_action *action,char *f2,char *cmds)
{
	//char *s1;
	int c;
	action->shell=0;
	action->script=0;
	action->file=0;
	action->exec=0;
	action->stdin=0;
	while (1)
	{
		static struct option long_options[] =
		{
			{"execute",required_argument,0,'x'},
			{"command",required_argument,0,'c'},
			{"interactive",no_argument,0,'i'},
			{"shell",no_argument,0,'i'},
			{"help",no_argument,0,'h'},
			{"version",no_argument,0,'v'},
			{"stdin",no_argument,0,'s'},
			{"colors",no_argument,0,'C'},
			{"nocolors",no_argument,0,'B'},
			{0,0,0,0,}
			
		};
		int option_index = 0;
		c = getopt_long (argc, argv, "CBsvhic:x:",long_options, &option_index);
		if (c == -1) break;
		switch(c)
		{
			case 'i':
				action->shell=1;
				//shell();
				//strncpy(cmd,"quit;",1024);
				break;
			case 'x':
				//printf("input script file:%s\n",optarg);
				action->script=1;
				strncpy(f2,optarg,FILENAME_LEN);
				//run_script(optarg);
				//execute("quit");
				break;
			case 'v':
				usage_b();
				break;
			case 'c':
				//printf("to execute:%s ....testing..\n",optarg);
				action->exec=1;
				strncpy(cmds,optarg,FILENAME_LEN);
				break;
			case 's':
				action->stdin=1;
				break;
			case 'C':
				cfg.colors=true;
				set_colors(cfg.colors);
				break;
			case 'B':
				cfg.colors=false;
				set_colors(cfg.colors);
				break;
			case 'h':
			case '?':
				usage();
				break;
			default:
				usage_b();
				break;
		}
		
	}
}
int main(int argc,char **argv)
{
	struct m_action action;
	char filename[FILENAME_LEN],filescript[FILENAME_LEN],cmd[FILENAME_LEN];
	dw_init();
	initialize_readline();
	add_cmds_completions();
	parse_args(argc,argv,&action,&filescript[0],&cmd[0]);
	
	if(argc<2) usage_b();
	if(optind<argc) {strncpy(filename,argv[optind],FILENAME_LEN);action.file=1;}
	else{strncpy(filename,"<NULL>",FILENAME_LEN);action.file=0;}
	//if(ilook_debug) look_debug();
	//execute(cmd);
	if(action.file)
	{
		//sprintf(fileopen,"open %s",filename);
		//execute(fileopen);
		file_open(filename);
		//free_completion();
		add_sh_completion();
	}
	if(action.stdin){
		printf("from stdin...\n");
		open_stdin();
		reset_stdin();
		add_sh_completion();
	}
	if(action.script)
	{
		run_script(filescript);
	}
	if(action.exec)
	{
		execute(cmd);
	}
	if(action.shell)
	{
		shell();
	}
	
	printf("Bye.\n");
	//free(cmd);
	//file_close();
	return 0;
}
